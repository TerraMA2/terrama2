// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
//#include "Service.hpp"

// QT
#include <QMessageBox>


ConfigAppWeatherTab::ConfigAppWeatherTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  ui_->weatherDataTree->header()->hide();
  ui_->weatherDataTree->setCurrentItem(app_->ui()->weatherDataTree->topLevelItem(0));
  ui_->weatherDataTree->setExpanded(app_->ui()->weatherDataTree->model()->index(0, 0), true);

  connect(ui_->saveBtn, SIGNAL(clicked()), SLOT(onSaveRequested()));
  connect(ui_->insertServerBtn, SIGNAL(clicked()), SLOT(onEnteredWeatherTab()));
  connect(ui_->cancelBtn, SIGNAL(clicked()), SLOT(onCancelRequested()));

  // Bind the inputs
  connect(ui_->serverName, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->serverDescription, SIGNAL(textChanged(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionAddress, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionPort, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionUserName, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionPassword, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionProtocol, SIGNAL(currentIndexChanged(int)), SLOT(onWeatherTabEdited()));
  connect(ui_->serverDataBasePath, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));

  ui_->weatherDataTree->clear();

  // Call the default configuration
  load();
}

ConfigAppWeatherTab::~ConfigAppWeatherTab()
{

}

void ConfigAppWeatherTab::load()
{
  // Disable series button
  ui_->exportServerBtn->setVisible(false);
  ui_->updateServerBtn->setVisible(false);
  ui_->serverDeleteBtn->setVisible(false);

  ui_->groupBox_25->hide();

  // Connect to database and list the values

}

bool ConfigAppWeatherTab::dataChanged()
{
  return changed_;
}

bool ConfigAppWeatherTab::validate()
{
  return false;
}

void ConfigAppWeatherTab::save()
{
  throw terrama2::Exception() << terrama2::ErrorDescription(tr("Could not save. There are empty fields!!"));
}

void ConfigAppWeatherTab::discardChanges(bool restore_data)
{
  if (restore_data)
  {
    // Make the save procedure
  }

  ui_->ServerPage->hide();
  ui_->ServerGroupPage->show();

// Set visible false on server buttons
  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

}

void ConfigAppWeatherTab::onEnteredWeatherTab()
{
  // Set visible the buttons
  ui_->saveBtn->setVisible(true);
  ui_->cancelBtn->setVisible(true);

  ui_->saveBtn->setEnabled(false);
  ui_->cancelBtn->setEnabled(false);

  ui_->ServerGroupPage->hide();
  ui_->ServerPage->show();
}

void ConfigAppWeatherTab::onWeatherTabEdited()
{
  ui_->saveBtn->setEnabled(true);
  ui_->cancelBtn->setEnabled(true);
  changed_ = true;
}
