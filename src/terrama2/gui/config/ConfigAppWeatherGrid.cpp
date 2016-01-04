// TerraMA2
#include "ConfigAppWeatherGrid.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
#include "../../core/DataSet.hpp"
#include "../../core/Filter.hpp"
#include "../../core/DataSetItem.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/Utils.hpp"
#include "FilterDialog.hpp"

// STL
#include <inttypes.h>
#include "../../core/Logger.hpp"

// Terralib
#include <terralib/datatype/TimeInstant.h>
#include <terralib/qt/widgets/srs/SRSManagerDialog.h>

// QT
#include <QMessageBox>
#include <QUrl>


ConfigAppWeatherGridTab::ConfigAppWeatherGridTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui), filter_(new terrama2::core::Filter)
{
  connect(ui_->serverInsertGridBtn, SIGNAL(clicked()), SLOT(onDataGridClicked()));
  connect(ui_->filterGridBtn, SIGNAL(clicked()), SLOT(onFilterClicked()));
  connect(ui_->gridFormatDataName, SIGNAL(textEdited(QString)), SLOT(onSubTabEdited()));
  connect(ui_->gridFormatDataFormat, SIGNAL(currentIndexChanged(const QString&)), SLOT(onGridFormatChanged()));
  connect(ui_->gridFormatDataDeleteBtn, SIGNAL(clicked()), SLOT(onRemoveDataGridBtnClicked()));

  connect(ui_->projectionGridBtn, SIGNAL(clicked()), this, SLOT(onProjectionClicked()));


  // data frequency int validator
  ui_->gridFormatDataHour->setValidator(new QIntValidator(ui_->gridFormatDataHour));
  ui_->gridFormatDataMinute->setValidator(new QIntValidator(ui_->gridFormatDataMinute));
  ui_->gridFormatDataSecond->setValidator(new QIntValidator(ui_->gridFormatDataSecond));

  ui_->gridFormatDataTimeZoneCmb->setCurrentText("+00:00");

}

ConfigAppWeatherGridTab::~ConfigAppWeatherGridTab()
{
  delete filter_;
}

void ConfigAppWeatherGridTab::load()
{
  auto menuMask = terrama2::gui::core::makeMaskHelpers();

  ui_->fileGridMaskBtn->setMenu(menuMask);
  ui_->fileGridMaskBtn->setPopupMode(QToolButton::InstantPopup);

  // connecting the menumask to display mask field values
  connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(onMenuMaskClicked(QAction*)));
}

void ConfigAppWeatherGridTab::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
  std::string name = ui_->gridFormatDataName->text().toStdString();
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(selectedData_.toStdString());

  terrama2::core::DataSet::Kind kind = terrama2::core::DataSet::GRID_TYPE;

  dataset.setName(name);
  dataset.setKind(kind);
  dataset.setDescription(ui_->gridFormatDataDescription->toPlainText().toStdString());
  dataset.setStatus(terrama2::core::ToDataSetStatus(ui_->gridFormatStatus->isChecked()));

  terrama2::core::DataSetItem* datasetItem;
  if (dataset.dataSetItems().size() > 0)
    datasetItem = &dataset.dataSetItems()[0];
  else
    datasetItem = new terrama2::core::DataSetItem;

  datasetItem->setFilter(*filter_);
  datasetItem->setSrid(srid_);

  datasetItem->setKind(terrama2::core::DataSetItem::GRID_TYPE);
  datasetItem->setMask(ui_->gridFormatDataMask->text().toStdString());
  datasetItem->setStatus(terrama2::core::DataSetItem::ACTIVE);
  datasetItem->setTimezone(ui_->gridFormatDataTimeZoneCmb->currentText().toStdString());
  datasetItem->setPath(ui_->gridFormatDataPath->text().toStdString());

  dataset.add(*datasetItem);

  te::dt::TimeDuration dataFrequency(ui_->gridFormatDataHour->text().toInt(),
                                     ui_->gridFormatDataMinute->text().toInt(),
                                     ui_->gridFormatDataSecond->text().toInt());

  te::dt::TimeDuration schedule(0, ui_->gridFormatDataInterval->value(), 0);
  dataset.setDataFrequency(dataFrequency);

  // todo: define a common key pattern to metadata
  std::map<std::string, std::string> metadata;
  metadata["KIND"] = ui_->gridFormatDataFormat->currentText().toStdString();
  metadata["PREFIX"] = ui_->gridFormatDataPrefix->text().toStdString();
  metadata["UNIT"] = ui_->gridFormatDataUnit->text().toStdString();
  metadata["RESOLUTION"] = ui_->gridFormatDataResolution->text().toStdString();
  dataset.setMetadata(metadata);

  auto storageMetadata = terrama2::gui::core::makeStorageMetadata(provider.uri().c_str(), *app_->getConfiguration());

  datasetItem->setStorageMetadata(storageMetadata);

  // todo: get value from db
  dataset.setSchedule(schedule);
  if (dataset.id() >= 1)
  {
    datasetItem->setDataSet(dataset.id());
    app_->getClient()->updateDataSet(dataset);
    app_->getWeatherTab()->refreshList(ui_->weatherDataTree->currentItem(),
                                       selectedData_,
                                       ui_->gridFormatDataName->text());
    selectedData_ =  ui_->gridFormatDataName->text();
    TERRAMA2_LOG_INFO() << ("Dataset ID " + std::to_string(dataset.id()) + " updated!");
  }
  else
  {
    dataset.setProvider(provider.id());
    app_->getClient()->addDataSet(dataset);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("grid"));
    item->setText(0, ui_->gridFormatDataName->text());
    ui_->weatherDataTree->currentItem()->addChild(item);
    TERRAMA2_LOG_INFO() << "New Dataset " + dataset.name() + " saved!";
  }
  app_->getWeatherTab()->addCachedDataSet(dataset);
  changed_ = false;
}

void ConfigAppWeatherGridTab::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataGridPage->findChildren<QLineEdit*>())
    widget->clear();

  ui_->gridFormatDataDescription->setText("");
  ui_->gridFormatDataTimeZoneCmb->setCurrentText("+00:00");

  changed_ = false;
  ui_->dataSeriesBtnGroupBox->setVisible(false);
  ui_->updateDataGridBtn->setVisible(false);
  ui_->exportDataGridBtn->setVisible(false);
  ui_->gridFormatDataDeleteBtn->setVisible(false);
  app_->getWeatherTab()->showDataSeries(true);
}

void ConfigAppWeatherGridTab::fillFilter(const terrama2::core::Filter& filter)
{
  *filter_ = filter;

  if (filter.discardAfter() != nullptr || filter.discardBefore() != nullptr)
    ui_->dateFilterLabel->setText(tr("Yes"));
  else
    ui_->dateFilterLabel->setText(tr("No"));

  if (filter.geometry() != nullptr)
    ui_->areaFilterLabel->setText(tr("Yes"));
  else
    ui_->areaFilterLabel->setText(tr("No"));

  if (!filter.bandFilter().empty())
    ui_->bandFilterLabel->setText(tr("Yes"));
  else
    ui_->bandFilterLabel->setText(tr("No"));

  if (filter.value() != nullptr)
    ui_->preAnalysisLabel->setText(tr("Yes"));
  else
    ui_->preAnalysisLabel->setText(tr("No"));
}

bool ConfigAppWeatherGridTab::validate()
{
  if (ui_->gridFormatDataName->text().trimmed().isEmpty())
  {
    ui_->gridFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("The Data Set Item name cannot be empty."));
  }

  checkMask(ui_->gridFormatDataMask->text());

  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(ui_->gridFormatDataName->text().toStdString());

  if (dataset.id() != 0 && !selectedData_.isEmpty())
  {
    if (ui_->gridFormatDataName->text() != selectedData_)
    {
      ui_->gridFormatDataName->setFocus();
      throw terrama2::gui::FieldError() << terrama2::ErrorDescription(
          tr("The data set grid name has already been saved. Please change server name"));
    }
  }

  // TODO: Complete validation with another fields and Projection and Filter validation
  return true;
}

void ConfigAppWeatherGridTab::onDataGridClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
  {
    selectedData_.clear();
    app_->getWeatherTab()->changeTab(*this, *ui_->DataGridPage);

    ui_->dateFilterLabel->setText(tr("No"));
    ui_->areaFilterLabel->setText(tr("No"));
    ui_->bandFilterLabel->setText(tr("No"));
    ui_->preAnalysisLabel->setText(tr("No"));
    if (filter_ != nullptr)
      delete filter_;

    filter_ = new terrama2::core::Filter;
    ui_->gridProjectionTxt->setText("0");

  }
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void ConfigAppWeatherGridTab::onGridFormatChanged()
{
  switch (ui_->gridFormatDataFormat->currentIndex())
  {
    case 0:
      ui_->stkwGridTipoFormato->setCurrentWidget(ui_->pgGridAscGRID);
      break;
    case 1:
      ui_->stkwGridTipoFormato->setCurrentWidget(ui_->pgGridTIFF);
      break;
    case 2:
      ui_->stkwGridTipoFormato->setCurrentWidget(ui_->pgGridGrADS);
      break;
    default:
      ;
  }

  ui_->gridProjectionTxt->setText("0");
}

void ConfigAppWeatherGridTab::onRemoveDataGridBtnClicked()
{
  QTreeWidgetItem* currentItem = ui_->weatherDataTree->currentItem();
  if (currentItem != nullptr && currentItem->parent() != nullptr && currentItem->parent()->parent() != nullptr)
  {
    try
    {
      terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(currentItem->text(0).toStdString());

      if (removeDataSet(dataset))
      {
        app_->getClient()->removeDataSet(dataset.id());

        TERRAMA2_LOG_INFO() << ("Dataset ID " + std::to_string(dataset.id()) + " has been removed!");
        app_->getWeatherTab()->removeCachedDataSet(dataset);

        QMessageBox::information(app_, tr("TerraMA2"), tr("DataSet Grid successfully removed!"));
        delete currentItem;
      }
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      TERRAMA2_LOG_ERROR() << "DataSet Removing: " << *message;
      QMessageBox::warning(app_, tr("TerraMA2"), *message);
    }
  }
  ui_->cancelBtn->clicked();
}

void ConfigAppWeatherGridTab::onMenuMaskClicked(QAction* action)
{
  ui_->gridFormatDataMask->setText(
        ui_->gridFormatDataMask->text() + action->text().left(2));
}

void ConfigAppWeatherGridTab::onFilterClicked()
{
  FilterDialog dialog(FilterDialog::FULL, ui_->gridFormatDataTimeZoneCmb->currentText(), app_);

  dialog.fillGUI(*filter_);

  if (dialog.exec() == QDialog::Accepted)
    dialog.fillObject(*filter_);

  if (dialog.isFilterByDate())
    ui_->dateFilterLabel->setText(tr("Yes"));
  else
    ui_->dateFilterLabel->setText(tr("No"));

  if (dialog.isFilterByArea())
    ui_->areaFilterLabel->setText(tr("Yes"));
  else
    ui_->areaFilterLabel->setText(tr("No"));

  if (dialog.isFilterByLayer())
    ui_->bandFilterLabel->setText(tr("Yes"));
  else
    ui_->bandFilterLabel->setText(tr("No"));

  if (dialog.isFilterByPreAnalyse())
    ui_->preAnalysisLabel->setText(tr("Yes"));
  else
    ui_->preAnalysisLabel->setText(tr("No"));
}

void ConfigAppWeatherGridTab::onProjectionClicked()
{
  te::qt::widgets::SRSManagerDialog srsDialog(app_);
  srsDialog.setWindowTitle(tr("Choose the SRS"));

  if (srsDialog.exec() == QDialog::Rejected)
    return;


  srid_ = (uint64_t) srsDialog.getSelectedSRS().first;
  ui_->gridProjectionTxt->setText(std::to_string(srid_).c_str());

}

void ConfigAppWeatherGridTab::setSrid(const uint64_t srid)
{
  srid_ = srid;
}
