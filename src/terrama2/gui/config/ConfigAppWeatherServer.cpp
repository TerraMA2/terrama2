// TerraMA2
#include "ConfigAppWeatherServer.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "../core/Utils.hpp"
#include "../../core/DataManager.hpp"
#include "Exception.hpp"
#include "../../core/Utils.hpp"

// QT
#include <QMessageBox>
#include <QUrl>
#include <QDir>
#include <QFileDialog>

// libcurl
#include <curl/curl.h>


ConfigAppWeatherServer::ConfigAppWeatherServer(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->insertServerBtn, SIGNAL(clicked()), this, SLOT(onServerTabRequested()));
  connect(ui_->serverName, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->serverDescription->document(), SIGNAL(contentsChanged()), SLOT(onTextEditChanged()));
  connect(ui_->connectionAddress, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->connectionPort, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->connectionAddress, SIGNAL(textEdited(QString)), SLOT(onServerEdited()));
  connect(ui_->serverCheckConnectionBtn, SIGNAL(clicked()), SLOT(onCheckConnectionClicked()));
  connect(ui_->connectionProtocol, SIGNAL(currentIndexChanged(int)), SLOT(onConnectionTypeChanged(int)));

  connect(ui_->fileServerButton, SIGNAL(clicked()), SLOT(onAddressFileBtnClicked()));

  // temp code
  ui_->serverIntervalData->setEnabled(false);

  ui_->fileServerButton->setVisible(false);

  QIntValidator* portValidator = new QIntValidator(ui_->connectionPort);
  portValidator->setRange(0, 65535);
  ui_->connectionPort->setValidator(portValidator);
}

ConfigAppWeatherServer::~ConfigAppWeatherServer()
{

}

void ConfigAppWeatherServer::load()
{
  connect(ui_->serverDescription->document(), SIGNAL(contentsChanged()), SLOT(onTextEditChanged()));
}

void ConfigAppWeatherServer::save()
{

  validateConnection();

  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(selectedData_.toStdString());

  provider.setName(ui_->serverName->text().toStdString());
  provider.setDescription(ui_->serverDescription->toPlainText().toStdString());
  provider.setKind(terrama2::core::ToDataProviderKind(ui_->connectionProtocol->currentIndex()+2));
  provider.setUri(uri_.toStdString());
  provider.setStatus(terrama2::core::ToDataProviderStatus(ui_->serverActiveServer->isChecked()));

  if (provider.id() > 0)
  {
    app_->getClient()->updateDataProvider(provider);

    // Refresh the weather list giving top level item and search for dataprovider selected
    app_->getWeatherTab()->refreshList(ui_->weatherDataTree->topLevelItem(0), selectedData_, ui_->serverName->text());

    selectedData_ = ui_->serverName->text();
  }
  else
  {
    app_->getClient()->addDataProvider(provider);

    QTreeWidgetItem* newServer = new QTreeWidgetItem();
    newServer->setText(0, ui_->serverName->text());
    newServer->setIcon(0, QIcon::fromTheme("server"));
    ui_->weatherDataTree->topLevelItem(0)->addChild(newServer);
  }
  app_->getWeatherTab()->addCachedProvider(provider);
  changed_ = false;
}

void ConfigAppWeatherServer::discardChanges(bool restore)
{
  const auto* tab = ui_->ServerPage;

// Clear QLineEdits
  for(QLineEdit* widget: tab->findChildren<QLineEdit*>())
    widget->clear();

  ui_->serverDescription->clear();
  selectedData_.clear();
  changed_ = false;
}

bool ConfigAppWeatherServer::validate()
{
  if (ui_->serverName->text().trimmed().isEmpty())
  {
    ui_->serverName->setFocus();
    return false;
  }

  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->serverName->text().toStdString());

  if (provider.id() != 0 && !selectedData_.isEmpty())
  {
    if (selectedData_ != ui_->serverName->text())
    {
      ui_->serverName->setFocus();
      throw terrama2::gui::DataProviderError() << terrama2::ErrorDescription(tr("The server name has already been saved. Please change server name"));
    }
  }
  return true;
}

void ConfigAppWeatherServer::onServerTabRequested()
{
  app_->getWeatherTab()->displayOperationButtons(true);
  app_->getWeatherTab()->changeTab(*this, *ui_->ServerPage);
}

void ConfigAppWeatherServer::onServerEdited()
{
  changed_ = true;
}

void ConfigAppWeatherServer::onCheckConnectionClicked()
{
  // For handling error message
  QString message;
  try
  {
    validateConnection();
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

void ConfigAppWeatherServer::onConnectionTypeChanged(int index)
{
  bool mode = false;
  if (index == 2) // FILE
  {
    mode = false;
    ui_->serverPath->setEnabled(true);
    ui_->fileServerButton->setVisible(true);
  }
  else if (index == 0) // ftp
  {
    mode = true;
    ui_->serverPath->setEnabled(true);
    ui_->fileServerButton->setVisible(false);
  }
  else
  {
    mode = true;
    ui_->serverPath->setEnabled(false);
    ui_->fileServerButton->setVisible(false);
  }

  ui_->connectionAddress->setEnabled(mode);
  ui_->connectionUserName->setEnabled(mode);
  ui_->connectionPort->setEnabled(mode);
  ui_->connectionPassword->setEnabled(mode);
}

void ConfigAppWeatherServer::onAddressFileBtnClicked()
{
  QString dir = QFileDialog::getExistingDirectory(app_, tr("Open Directory"),
                                               ui_->connectionAddress->text(),
                                               QFileDialog::ShowDirsOnly);
  if (dir.isEmpty())
    return;
  ui_->connectionAddress->setText(dir);
}

void ConfigAppWeatherServer::validateConnection()
{
  QUrl url;

  switch (terrama2::core::ToDataProviderKind(ui_->connectionProtocol->currentIndex()+2))
  {
    case terrama2::core::DataProvider::FILE_TYPE:
      {
        QDir directory(ui_->serverPath->text());
        if (!directory.exists())
        {
          ui_->serverPath->setFocus();
          throw terrama2::gui::DirectoryError() << terrama2::ErrorDescription(tr("It is an invalid path"));
        }

        url.setPath(ui_->connectionAddress->text());

        return;
      }
      break;

    case terrama2::core::DataProvider::HTTP_TYPE:
      url.setHost(ui_->connectionAddress->text());

      break;
    case terrama2::core::DataProvider::FTP_TYPE:
      url.setPath(ui_->serverPath->text());

      break;
    default:
      throw terrama2::gui::ValueError() << terrama2::ErrorDescription(tr("Not implemented yet."));
  }

  if (ui_->connectionAddress->text().trimmed().isEmpty())
  {
    ui_->connectionAddress->setFocus();
    throw terrama2::gui::DirectoryError() << terrama2::ErrorDescription(tr("Address field cannot be empty"));
  }

  if (!url.isValid())
  {
    ui_->connectionAddress->setFocus();
    throw terrama2::gui::URLError() << terrama2::ErrorDescription(QObject::tr("Invalid URL address typed"));
  }

  url.setHost(ui_->connectionAddress->text());
  url.setScheme(ui_->connectionProtocol->currentText().toLower());
  url.setUserName(ui_->connectionUserName->text());
  url.setPassword(ui_->connectionPassword->text());
  url.setPort(ui_->connectionPort->text().toInt()); // 0

  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();

  if(curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url.toString(QUrl::RemovePassword | QUrl::RemoveUserInfo | QUrl::RemovePort).toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_PORT, url.port());
    curl_easy_setopt(curl, CURLOPT_USERNAME, url.userName().toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, url.password().toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    res = curl_easy_perform(curl);

    // check: should it error handling each one common code?
    switch(res)
    {
      case CURLE_OK:
        break;

      case CURLE_OPERATION_TIMEDOUT:
        throw terrama2::gui::ConnectionError() << terrama2::ErrorDescription(QObject::tr("Error while connecting.. Timeout!"));
        break;

      case CURLE_LOGIN_DENIED:
        throw terrama2::gui::ConnectionError() << terrama2::ErrorDescription(QObject::tr("Error while connecting.. Login denied!"));
        break;
      default:
        throw terrama2::gui::URLError() << terrama2::ErrorDescription(QObject::tr("Error in connection..."));
    }

    curl_easy_cleanup(curl);
  }
  else
    throw terrama2::gui::URLError() << terrama2::ErrorDescription(QObject::tr("Error to ping"));

  uri_ = url.url();
}

void ConfigAppWeatherServer::onTextEditChanged()
{
  changed_ = true;
}
