// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
//#include "Service.hpp"

// QT
#include <QMessageBox>
#include <QLineEdit>
#include <QFileDialog>

#include <iostream>


ConfigAppWeatherTab::ConfigAppWeatherTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)//, controller)
{
  ui_->weatherDataTree->header()->hide();
  ui_->weatherDataTree->setCurrentItem(app_->ui()->weatherDataTree->topLevelItem(0));
  ui_->weatherDataTree->setExpanded(app_->ui()->weatherDataTree->model()->index(0, 0), true);

  connect(ui_->saveBtn, SIGNAL(clicked()), SLOT(onSaveRequested()));
  connect(ui_->insertServerBtn, SIGNAL(clicked()), SLOT(onEnteredWeatherTab()));
  connect(ui_->cancelBtn, SIGNAL(clicked()), SLOT(onCancelRequested()));
  connect(ui_->importServerBtn, SIGNAL(clicked()), SLOT(onImportServer()));
  connect(ui_->serverCheckConnectionBtn, SIGNAL(clicked()), SLOT(onCheckConnection()));

  // Bind the inputs
  connect(ui_->serverName, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->serverDescription, SIGNAL(textChanged(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionAddress, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionPort, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionUserName, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionPassword, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionProtocol, SIGNAL(currentIndexChanged(int)), SLOT(onWeatherTabEdited()));
  connect(ui_->serverDataBasePath, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

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
  // HardCode
  for(QLineEdit* widget: ui_->ServerPage->findChildren<QLineEdit*>())
  {
    if (widget->text() == "")
      return false;
  }
  return true;
}

void ConfigAppWeatherTab::save()
{
  if (!validate())
  {
    throw terrama2::Exception() << terrama2::ErrorDescription(tr("Could not save. There are empty fields!!"));
  }
  // Apply save process
  discardChanges(false);
  QMessageBox::information(app_, tr("TerraMA2"), tr("Save successfully"));
}

void ConfigAppWeatherTab::discardChanges(bool restore_data)
{
  if (restore_data)
  {
    // Make the save procedure
  }

// Clear all inputs
  const auto& tab = ui_->ServerPage;

// Clear QLineEdits
  for(QLineEdit* widget: tab->findChildren<QLineEdit*>())
  {
    widget->clear();
  }

  //Clear TextEdit (server description)
  ui_->serverDescription->clear();

// Hide the form
  ui_->ServerPage->hide();
  ui_->ServerGroupPage->show();
  changed_ = false;

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

void ConfigAppWeatherTab::onImportServer()
{
  QString file = QFileDialog::getOpenFileName(app_, tr("Choose file"), ".", tr("TerraMA2 ( *.terrama2"));
  if (!file.isEmpty())
  {
    QMessageBox::information(app_, tr("TerraMA2 Server"), tr("Opened Server File"));
  }
}

void ConfigAppWeatherTab::onCheckConnection()
{
  // For handling error message
  QString message;
  try
  {
    switch ((terrama2::gui::core::ConnectionType)ui_->connectionProtocol->currentIndex())
    {
      case terrama2::gui::core::FTP:
        terrama2::gui::core::checkFTPConnection(ui_->connectionAddress->text(),
                                                ui_->connectionPort->text().toInt(),
                                                ui_->serverDataBasePath->text(),
                                                ui_->connectionUserName->text(),
                                                ui_->connectionPassword->text());
        break;
      case terrama2::gui::core::WEBSERVICE:
        terrama2::gui::core::checkServiceConnection(ui_->connectionAddress->text(),
                                                    ui_->connectionPort->text().toInt(),
                                                    ui_->connectionUserName->text(),
                                                    ui_->connectionPassword->text());
        break;
      case terrama2::gui::core::LOCALFILES:
        terrama2::gui::core::checkLocalFilesConnection(ui_->serverDataBasePath->text());
        break;
      default:
        throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Invalid protocol selected"));
        break;
    }
    return;
  }
  catch(const terrama2::Exception& e)
  {
    message.append(boost::get_error_info<terrama2::ErrorDescription>(e));
  }
  catch(const std::exception& e)
  {
    message.fromUtf8(e.what());
  }
  catch(...)
  {
    message.append("Unknown Error");
  }
  QMessageBox::critical(app_, tr("TerraMA2 Error"), message);
}
