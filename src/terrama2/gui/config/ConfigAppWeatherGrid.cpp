// TerraMA2
#include "ConfigAppWeatherGrid.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "../../core/DataSet.hpp"
#include "../../core/Filter.hpp"
#include "../../core/DataSetItem.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/Utils.hpp"
#include "FilterDialog.hpp"
#include "ProjectionDialog.hpp"

// QT
#include <QMessageBox>


ConfigAppWeatherGridTab::ConfigAppWeatherGridTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui), filter_(new terrama2::core::Filter)
{
  connect(ui_->serverInsertGridBtn, SIGNAL(clicked()), SLOT(onDataGridClicked()));
  connect(ui_->filterGridBtn, SIGNAL(clicked()), SLOT(onFilterClicked()));
  connect(ui_->gridFormatDataName, SIGNAL(textEdited(QString)), SLOT(onSubTabChanged()));
  connect(ui_->gridFormatDataFormat, SIGNAL(currentIndexChanged(const QString&)), SLOT(onGridFormatChanged()));
  connect(ui_->gridFormatDataDeleteBtn, SIGNAL(clicked()), SLOT(onRemoveDataGridBtnClicked()));

  connect(ui_->projectionGridBtn, SIGNAL(clicked()), this, SLOT(onProjectionClicked()));
}

ConfigAppWeatherGridTab::~ConfigAppWeatherGridTab()
{
  delete filter_;
}

bool ConfigAppWeatherGridTab::dataChanged()
{
  return active_ && changed_;
}

void ConfigAppWeatherGridTab::load()
{
  QMenu* menuMask = new QMenu(tr("Máscaras"));
  menuMask->addAction(tr("%a - ano com dois digitos"));
  menuMask->addAction(tr("%A - ano com quatro digitos"));
  menuMask->addAction(tr("%d - dia com dois digitos"));
  menuMask->addAction(tr("%M - mes com dois digitos"));
  menuMask->addAction(tr("%h - hora com dois digitos"));
  menuMask->addAction(tr("%m - minuto com dois digitos"));
  menuMask->addAction(tr("%s - segundo com dois digitos"));
  menuMask->addAction(tr("%. - um caracter qualquer"));

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
  dataset.setDescription(ui_->gridFormatDataDescription->toPlainText().toStdString());

  terrama2::core::DataSetItem datasetItem;

  // temp code
  datasetItem.setFilter(*filter_);
  datasetItem.setKind(terrama2::core::DataSetItem::UNKNOWN_TYPE);
  datasetItem.setMask(ui_->gridFormatDataMask->text().toStdString());
  datasetItem.setStatus(terrama2::core::DataSetItem::ACTIVE);
  datasetItem.setTimezone(ui_->gridFormatDataTimeZoneCmb->currentText().toStdString());

  dataset.add(datasetItem);

  te::dt::TimeDuration dataFrequency(ui_->gridFormatDataFrequency->text().toInt(), 0, 0);
  te::dt::TimeDuration schedule(0, ui_->gridFormatDataInterval->value(), 0);
  dataset.setDataFrequency(dataFrequency);

  // todo: get value from db
  dataset.setSchedule(schedule);
  if (dataset.id() >= 1)
  {
    app_->getClient()->updateDataSet(dataset);
    app_->getWeatherTab()->refreshList(ui_->weatherDataTree->currentItem(),
                                       selectedData_,
                                       ui_->gridFormatDataName->text());
    selectedData_ =  ui_->gridFormatDataName->text();
  }
  else
  {
    dataset.setProvider(provider.id());
    app_->getClient()->addDataSet(dataset);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("grid"));
    item->setText(0, ui_->gridFormatDataName->text());
    ui_->weatherDataTree->currentItem()->addChild(item);
  }
  app_->getWeatherTab()->addCachedDataSet(dataset);
  changed_ = false;
}

void ConfigAppWeatherGridTab::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataGridPage->findChildren<QLineEdit*>())
    widget->clear();

  changed_ = false;
}

bool ConfigAppWeatherGridTab::validate()
{
  if (ui_->gridFormatDataName->text().trimmed().isEmpty())
  {
    ui_->gridFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("The Data Set Item name cannot be empty."));
  }

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
  }
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void ConfigAppWeatherGridTab::onSubTabChanged()
{
  changed_ = true;
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
        app_->getWeatherTab()->removeCachedDataSet(dataset);

        QMessageBox::information(app_, tr("TerraMA2"), tr("DataSet Grid successfully removed!"));
        delete currentItem;
      }
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
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
  FilterDialog dialog(FilterDialog::FULL, app_);

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
  ProjectionDialog projectionDialog(app_);

  projectionDialog.show();
  projectionDialog.exec();
}
