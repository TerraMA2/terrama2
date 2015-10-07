// TerraMA2
#include "ConfigAppWeatherGrid.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"

// QT
#include <QMessageBox>


ConfigAppWeatherGridTab::ConfigAppWeatherGridTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertGridBtn, SIGNAL(clicked()), SLOT(onDataGridClicked()));
  connect(ui_->gridFormatDataName, SIGNAL(textEdited(QString)), SLOT(onSubTabChanged()));
}

ConfigAppWeatherGridTab::~ConfigAppWeatherGridTab()
{

}

bool ConfigAppWeatherGridTab::dataChanged()
{
  return active_ && changed_;
}

void ConfigAppWeatherGridTab::load()
{
}

void ConfigAppWeatherGridTab::save()
{
  QMessageBox::information(app_, tr("TerraMA2"), tr("Saved Grid"));
}

void ConfigAppWeatherGridTab::discardChanges(bool restore_data)
{
  if (restore_data)
  {

  }
  changed_ = false;
}

bool ConfigAppWeatherGridTab::validate()
{
  if (ui_->gridFormatDataName->text().isEmpty())
  {
    ui_->gridFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("The Data Set Item name cannot be empty."));
  }
  return true;
}

void ConfigAppWeatherGridTab::onDataGridClicked()
{
  app_->getWeatherTab()->changeTab(*this, *ui_->DataGridPage);
}

void ConfigAppWeatherGridTab::onSubTabChanged()
{
  changed_ = true;
}