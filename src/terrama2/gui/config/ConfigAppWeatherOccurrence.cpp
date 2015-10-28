#include "ConfigAppWeatherOccurrence.hpp"
#include "Exception.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "FilterDialog.hpp"

// Qt
#include <QMessageBox>

ConfigAppWeatherOccurrence::ConfigAppWeatherOccurrence(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertPointDiffBtn, SIGNAL(clicked()), SLOT(onDataSetBtnClicked()));
  connect(ui_->serverRemovePointDiffBtn, SIGNAL(clicked()), SLOT(onRemoveOccurrenceBtnClicked()));
  connect(ui_->filterPointDiffBtn, SIGNAL(clicked()), SLOT(onFilterClicked()));

  ui_->updateDataPointDiffBtn->setEnabled(false);
  ui_->exportDataPointDiffBtn->setEnabled(false);
}

ConfigAppWeatherOccurrence::~ConfigAppWeatherOccurrence()
{

}

void ConfigAppWeatherOccurrence::load()
{

}

bool ConfigAppWeatherOccurrence::validate()
{
  if (ui_->pointDiffFormatDataName->text().trimmed().isEmpty())
  {
    ui_->pointDiffFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Occurence Name is invalid"));
  }
  //TODO: validate correctly all fields
  return true;
}

void ConfigAppWeatherOccurrence::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(selectedData_.toStdString());

  dataset.setName(ui_->pointDiffFormatDataName->text().toStdString());
  dataset.setKind(terrama2::core::DataSet::OCCURENCE_TYPE);
  dataset.setDescription(ui_->pointDiffFormatDataDescription->toPlainText().toStdString());

  terrama2::core::DataSetItem datasetItem;
  // TODO: fix it with datasetitem value
  datasetItem.setKind(terrama2::core::ToDataSetItemKind(ui_->pointDiffFormatDataType->currentIndex()+4));
  datasetItem.setMask(ui_->pointDiffFormatDataMask->text().toStdString());
  datasetItem.setTimezone(ui_->pointDiffFormatDataTimeZoneCmb->currentText().toStdString());
  dataset.add(datasetItem);

  dataset.setStatus(terrama2::core::DataSet::ACTIVE);
  if (dataset.id() > 0)
  {
    app_->getClient()->updateDataSet(dataset);
    app_->getWeatherTab()->refreshList(ui_->weatherDataTree->currentItem(),
                                       selectedData_,
                                       ui_->pointDiffFormatDataName->text());
    selectedData_ =  ui_->pointDiffFormatDataName->text();
  }
  else
  {
    dataset.setProvider(provider.id());
    app_->getClient()->addDataSet(dataset);
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("ocurrence-data"));
    item->setText(0, ui_->pointDiffFormatDataName->text());
    ui_->weatherDataTree->currentItem()->addChild(item);
  }
  app_->getWeatherTab()->addCachedDataSet(dataset);
  changed_ = false;
}

void ConfigAppWeatherOccurrence::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataPointDiffPage->findChildren<QLineEdit*>())
    widget->clear();
  changed_ = false;
}

void ConfigAppWeatherOccurrence::onFilterClicked()
{
  FilterDialog dialog(app_);
  dialog.exec();

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

void ConfigAppWeatherOccurrence::onDataSetBtnClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
    app_->getWeatherTab()->changeTab(*this, *ui_->DataPointDiffPage);
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void ConfigAppWeatherOccurrence::onRemoveOccurrenceBtnClicked()
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

        QMessageBox::information(app_, tr("TerraMA2"), tr("DataSet Occurrence successfully removed!"));
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
