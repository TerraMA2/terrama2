#include "ConfigAppWeatherPcd.hpp"
#include "Exception.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"


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
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(provider, selectedData_.toStdString());

  dataset.setName(ui_->pointFormatDataName->text().toStdString());
  dataset.setKind(terrama2::core::DataSet::PCD_TYPE);
  //TODO: add checkbox in UI for data series status
  dataset.setStatus(terrama2::core::DataSet::ACTIVE);
  if (dataset.id() > 0)
  {
    app_->getClient()->updateDataSet(dataset);
  }
  else
  {
    app_->getClient()->addDataSet(dataset);
  }
}

void ConfigAppWeatherPcd::discardChanges(bool restore_data)
{

}

void ConfigAppWeatherPcd::onInsertPointBtnClicked()
{
  ui_->weatherPageStack->setCurrentWidget(ui_->DataPointPage);
}
