// TerraMA2
#include "ConfigAppWeatherServer.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "../core/Utils.hpp"
#include "../../core/DataManager.hpp"
#include "Exception.hpp"
#include "../../core/Utils.hpp"
#include "../../core/Logger.hpp"

// QT
#include <QMessageBox>
#include <QUrl>
#include <QDir>
#include <QFileDialog>
#include <QProgressBar>
#include <QDebug>

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
  QMessageBox::warning(app_, tr("TerraMA2 Error"), message);
}

void ConfigAppWeatherServer::onConnectionTypeChanged(int index)
{
  bool mode = false;
  if (index == 2) // FILE
  {
    mode = false;
    ui_->serverPath->setVisible(true);
    ui_->fileServerButton->setVisible(true);
    ui_->labelSearchPath->setVisible(true);
  }
  else if (index == 0) // ftp
  {
    mode = true;
    ui_->serverPath->setVisible(true);
    ui_->fileServerButton->setVisible(false);
    ui_->labelSearchPath->setVisible(true);
  }
  else
  {
    mode = true;
    ui_->serverPath->setVisible(false);
    ui_->fileServerButton->setVisible(false);
    ui_->labelSearchPath->setVisible(false);
  }

  ui_->labelAddress->setVisible(mode);
  ui_->connectionAddress->setVisible(mode);

  ui_->labelUser->setVisible(mode);
  ui_->connectionUserName->setVisible(mode);

  ui_->labelPort->setVisible(mode);
  ui_->connectionPort->setVisible(mode);

  ui_->labelPasswd->setVisible(mode);
  ui_->connectionPassword->setVisible(mode);
}

void ConfigAppWeatherServer::onAddressFileBtnClicked()
{
  QString dir = QFileDialog::getExistingDirectory(app_, tr("Open Directory"),
                                               ui_->connectionAddress->text(),
                                               QFileDialog::ShowDirsOnly);
  if (dir.isEmpty())
    return;
  ui_->serverPath->setText(dir);
}

void ConfigAppWeatherServer::validateConnection()
{
  QUrl url;
  url.setScheme(ui_->connectionProtocol->currentText().toLower());

  switch (terrama2::core::ToDataProviderKind(ui_->connectionProtocol->currentIndex()+2))
  {
    case terrama2::core::DataProvider::FILE_TYPE:
      {
        QDir directory;
        directory.setPath(ui_->serverPath->text());

        if (!directory.exists())
        {
          ui_->serverPath->setFocus();
          throw terrama2::gui::DirectoryError() << terrama2::ErrorDescription(tr("It is an invalid path"));
        }

        url.setPath(ui_->serverPath->text());
        uri_ = url.url();

        return;
      }
      break;

    case terrama2::core::DataProvider::HTTP_TYPE:
      url.setHost(ui_->connectionAddress->text());

      break;
    case terrama2::core::DataProvider::FTP_TYPE:
      {
        QString path(ui_->serverPath->text());

        // to check base dir
        if (!path.trimmed().isEmpty())
        {
          if (!path.startsWith("/"))
            path.prepend("/");

          if (!path.endsWith("/"))
            path.append("/");
        }
        else
          path = "/";

        url.setPath(path);
      }
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
    throw terrama2::gui::URLError() << terrama2::ErrorDescription(tr("Invalid URL address typed"));
  }

  url.setHost(ui_->connectionAddress->text());
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
        {
          const QString message = tr("Error while connecting.. Timeout!");
          TERRAMA2_LOG_WARNING() << "DataProvider Connection: " << message;
          throw terrama2::gui::ConnectionError() << terrama2::ErrorDescription(message);
        }
        break;

      case CURLE_LOGIN_DENIED:
        {
          const QString message = tr("Error while connecting.. Login denied!");
          TERRAMA2_LOG_WARNING() << "DataProvider Connection: " << message;
          throw terrama2::gui::ConnectionError() << terrama2::ErrorDescription(message);
        }
        break;

      case CURLE_URL_MALFORMAT:
        {
          const QString message = tr("Error while connecting.. Invalid path!");
          TERRAMA2_LOG_WARNING() << "DataProvider Connection: " << message;
          throw terrama2::gui::ConnectionError() << terrama2::ErrorDescription(message);
        }
        break;

      default:
        {
          const QString message = tr("Error in connection...");
          TERRAMA2_LOG_WARNING() << "DataProvider Connection: " << message;
          throw terrama2::gui::ConnectionError() << terrama2::ErrorDescription(message);
        }
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
