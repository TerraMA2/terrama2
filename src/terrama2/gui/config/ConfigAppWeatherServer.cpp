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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileDialog>

ConfigAppWeatherServer::ConfigAppWeatherServer(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui), manager_(new QNetworkAccessManager(this))
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
}

ConfigAppWeatherServer::~ConfigAppWeatherServer()
{
  delete manager_;
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
  if (index == 2)
    ui_->fileServerButton->setVisible(true);
  else
    ui_->fileServerButton->setVisible(false);
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
  url.setScheme(ui_->connectionProtocol->currentText().toLower());

  if (ui_->connectionAddress->text().trimmed().isEmpty())
  {
    ui_->connectionAddress->setFocus();
    throw terrama2::gui::DirectoryError() << terrama2::ErrorDescription(tr("Address field cannot be empty"));
  }

  switch (terrama2::core::ToDataProviderKind(ui_->connectionProtocol->currentIndex()+2))
  {
    case terrama2::core::DataProvider::FILE_TYPE:
      {
        QDir directory(ui_->connectionAddress->text());
        if (!directory.exists())
          throw terrama2::gui::DirectoryError() << terrama2::ErrorDescription(tr("It is an invalid path"));

        url.setPath(ui_->connectionAddress->text());
      }
      break;

    case terrama2::core::DataProvider::SOS_TYPE:
    case terrama2::core::DataProvider::POSTGIS_TYPE:
      throw terrama2::gui::ValueError() << terrama2::ErrorDescription(tr("Not implemented yet."));
      break;

    default:
      url.setHost(ui_->connectionAddress->text());
      url.setUserName(ui_->connectionUserName->text());
      url.setPassword(ui_->connectionPassword->text());
      url.setPort(ui_->connectionPort->text().toInt());

      if (!url.isValid())
        throw terrama2::gui::URLError() << terrama2::ErrorDescription(QObject::tr("Invalid URL address typed"));

      QNetworkReply* reply = manager_->get(QNetworkRequest(url));

      if (reply->error() != QNetworkReply::NoError)
        QMessageBox::warning(app_, tr("TerraMA2 Connection Error"), tr("Error connecting"));

  }

  uri_ = url.url();
}

void ConfigAppWeatherServer::onTextEditChanged()
{
  changed_ = true;
}
