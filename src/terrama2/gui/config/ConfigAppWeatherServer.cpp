// TerraMA2
#include "ConfigAppWeatherServer.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "../core/Utils.hpp"
#include "../../core/ApplicationController.hpp"
#include "../../core/DataManager.hpp"
#include "Exception.hpp"
#include "../../core/Utils.hpp"

// QT
#include <QMessageBox>

ConfigAppWeatherServer::ConfigAppWeatherServer(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->insertServerBtn, SIGNAL(clicked()), this, SLOT(onServerTabRequested()));
  connect(ui_->serverName, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->serverDescription, SIGNAL(textChanged(QString)), SLOT(onServerEdited()));
  connect(ui_->connectionAddress, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->connectionPort, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->connectionUserName, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->connectionPassword, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->connectionProtocol, SIGNAL(currentIndexChanged(int)), SLOT(onServerEdited()));
  connect(ui_->serverDataBasePath, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->serverCheckConnectionBtn, SIGNAL(clicked()), SLOT(onCheckConnectionClicked()));
}

ConfigAppWeatherServer::~ConfigAppWeatherServer()
{

}

void ConfigAppWeatherServer::load()
{
}

void ConfigAppWeatherServer::save()
{
  // Load dataproviders
  terrama2::core::DataManager::getInstance().load();

  // If there data provider in database
  if (terrama2::core::DataProviderPtr dataProvider = terrama2::core::DataManager::getInstance().findDataProvider(
      ui_->serverName->text().toStdString()))
  {
    dataProvider->setName(ui_->serverName->text().toStdString());
    dataProvider->setDescription(ui_->serverDescription->toPlainText().toStdString());
    dataProvider->setKind(static_cast<terrama2::core::DataProvider::Kind>(ui_->connectionProtocol->currentIndex()));
    dataProvider->setUri(ui_->connectionAddress->text().toStdString());
    dataProvider->setStatus(terrama2::core::BoolToDataProviderStatus(ui_->serverActiveServer->isChecked()));

    terrama2::core::DataManager::getInstance().update(dataProvider);
//    ui_->weatjerDataTree->find
    ui_->weatherDataTree->currentItem()->setText(0, ui_->serverName->text());
  }
  else
  {
    dataProvider.reset(new terrama2::core::DataProvider(ui_->serverName->text().toStdString(),
                                                        terrama2::core::IntToDataProviderKind(ui_->connectionProtocol->currentIndex())));
    dataProvider->setDescription(ui_->serverDescription->toPlainText().toStdString());
    dataProvider->setUri(ui_->connectionAddress->text().toStdString());
    dataProvider->setStatus(ui_->serverActiveServer->isChecked() ? terrama2::core::DataProvider::ACTIVE :
                            terrama2::core::DataProvider::INACTIVE);

    terrama2::core::DataManager::getInstance().add(dataProvider);

    QTreeWidgetItem* newServer = new QTreeWidgetItem();
    newServer->setText(0, ui_->serverName->text());
    newServer->setIcon(0, QIcon::fromTheme("server"));
    ui_->weatherDataTree->topLevelItem(0)->addChild(newServer);
  }
  changed_ = false;
}

void ConfigAppWeatherServer::discardChanges(bool restore)
{
  if (restore)
  {
    // DO SOME OPERATION
  }

  // Clear all inputs
  const auto* tab = ui_->ServerPage;

// Clear QLineEdits
  for(QLineEdit* widget: tab->findChildren<QLineEdit*>())
  {
    widget->clear();
  }

  ui_->serverDescription->clear();
}

bool ConfigAppWeatherServer::validate()
{
  if (ui_->serverName->text().isEmpty())
  {
    ui_->serverName->setFocus();
    return false;
  }

  // Check if has already been saved before
  std::shared_ptr<te::da::DataSource> ds = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::string sqlProvider("SELECT name FROM terrama2.data_provider WHERE name = '");
  sqlProvider += ui_->serverName->text().toStdString() + "'";
  std::auto_ptr<te::da::DataSet> dset = ds->query(sqlProvider);

  if (dset->size() > 0)
  {
    ui_->serverName->setFocus();
    throw terrama2::Exception() << terrama2::ErrorDescription(tr("The server name has already been saved. Please change server name"));
  }
  return true;
}

bool ConfigAppWeatherServer::dataChanged()
{
  return active_ && changed_;
}

void ConfigAppWeatherServer::onServerTabRequested()
{
//  ConfigAppWeatherTab::getInstance().displayOperationButtons(true);
  app_->getWeatherTab()->changeTab(*this, *ui_->ServerPage);
}

void ConfigAppWeatherServer::onServerEdited()
{
  changed_ = true;
  ui_->saveBtn->setEnabled(true);
}

void ConfigAppWeatherServer::onCheckConnectionClicked()
{
  // For handling error message
  QString message;
  try
  {
    validateConnection();
    // FIX
    QMessageBox::information(app_, tr("TerraMA2"), tr("Connection OK"));
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

void ConfigAppWeatherServer::validateConnection()
{
  switch (terrama2::core::IntToDataProviderKind(ui_->connectionProtocol->currentIndex()))
  {
    case terrama2::core::DataProvider::FTP_TYPE:
      terrama2::gui::core::checkFTPConnection(ui_->connectionAddress->text(),
                                              ui_->connectionPort->text().toInt(),
                                              ui_->serverDataBasePath->text(),
                                              ui_->connectionUserName->text(),
                                              ui_->connectionPassword->text());
      break;
    case terrama2::core::DataProvider::HTTP_TYPE:
      terrama2::gui::core::checkServiceConnection(ui_->connectionAddress->text(),
                                                  ui_->connectionPort->text().toInt(),
                                                  ui_->connectionUserName->text(),
                                                  ui_->connectionPassword->text());
      break;
    case terrama2::core::DataProvider::FILE_TYPE:
      terrama2::gui::core::checkLocalFilesConnection(ui_->serverDataBasePath->text());
      break;
    default:
      throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Not implemented yet"));
  }
}
