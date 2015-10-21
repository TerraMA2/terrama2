#include "ConfigAppWeatherPcd.hpp"
#include "Exception.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"

// Qt
#include <QMessageBox>


ConfigAppWeatherPcd::ConfigAppWeatherPcd(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
}

ConfigAppWeatherPcd::~ConfigAppWeatherPcd()
{

}

void ConfigAppWeatherPcd::load()
{

}

bool ConfigAppWeatherPcd::validate()
{
  if (ui_->pointFormatDataName->text().trimmed().isEmpty())
  {
    ui_->pointFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Name is invalid"));
  }
  //TODO: validate correctly all fields
  return true;
}

void ConfigAppWeatherPcd::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(selectedData_.toStdString());

  dataset.setName(ui_->pointFormatDataName->text().toStdString());
  dataset.setKind(terrama2::core::DataSet::PCD_TYPE);
  //TODO: add checkbox in UI for data series status
  dataset.setStatus(terrama2::core::DataSet::ACTIVE);
  if (dataset.id() > 0)
  {
    app_->getClient()->updateDataSet(dataset);
    app_->getWeatherTab()->refreshList(ui_->weatherDataTree->currentItem(), selectedData_, ui_->pointFormatDataName->text());
    selectedData_ = ui_->pointFormatDataName->text();
  }
  else
  {
    dataset.setProvider(provider.id());
    app_->getClient()->addDataSet(dataset);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("pcd"));
    item->setText(0, ui_->pointFormatDataName->text());
    ui_->weatherDataTree->currentItem()->addChild(item);
  }
  app_->getWeatherTab()->addCachedDataSet(dataset);
  changed_ = false;
}

void ConfigAppWeatherPcd::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataPointPage->findChildren<QLineEdit*>())
    widget->clear();
  changed_ = false;
}

void ConfigAppWeatherPcd::onInsertPointBtnClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
    app_->getWeatherTab()->changeTab(*this, *ui_->DataPointPage);
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}
