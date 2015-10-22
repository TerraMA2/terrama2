#include "ConfigAppWeatherOccurrence.hpp"
#include "Exception.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"

// Qt
#include <QMessageBox>

ConfigAppWeatherOccurrence::ConfigAppWeatherOccurrence(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertPointDiffBtn, SIGNAL(clicked()), SLOT(onDataSetBtnClicked()));
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
    item->setIcon(0, QIcon::fromTheme("occurrence"));
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

void ConfigAppWeatherOccurrence::onDataSetBtnClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
    app_->getWeatherTab()->changeTab(*this, *ui_->DataPointDiffPage);
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}
