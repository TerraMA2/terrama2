/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/gui/admin/AdminApp.cpp

  \brief Main GUI for TerraMA2 Config application.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "AdminApp.hpp"
#include "ui_AdminAppForm.h"
#include "../Exception.hpp"
#include "../../core/Utils.hpp"
#include "AdminAppDBTab.hpp"
#include "AdminAppCollectTab.hpp"
#include "ServicesDialog.hpp"
//#include "../core/ConsoleDialog.hpp"
#include "../../core/ApplicationController.hpp"

// Qt
#include <QIcon>
#include <QStringList>
#include <QToolBar>
#include <QTranslator>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QList>
#include <QHostInfo>
#include <QCloseEvent>

struct AdminApp::Impl
{
  Ui::AdminAppForm* ui_;
  
  Impl()
    : ui_(new Ui::AdminAppForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }
};

AdminApp::AdminApp(QWidget* parent)
  : QMainWindow(parent),
    pimpl_(new Impl)
{

// find icon theme library
  std::string icon_theme_path = terrama2::core::FindInTerraMA2Path("share/terrama2/icons");

  if(icon_theme_path.empty())
  {
    throw terrama2::InitializationError() << terrama2::ErrorDescription(tr("Could not find TerraMA2 icon library folder."));
  }
  
// load icon theme library
  QStringList ithemes = QIcon::themeSearchPaths();

  ithemes.push_back(icon_theme_path.c_str());

  QIcon::setThemeSearchPaths(ithemes);

  QIcon::setThemeName("terrama2");

// load idioms
  std::string idiom_path = terrama2::core::FindInTerraMA2Path("share/terrama2/translations/terrama2_admin_en_US.qm");
  
  if(idiom_path.empty())
  {
// LOG WARNING
  }
  else
  {
    QTranslator translator;
    
    translator.load(idiom_path.c_str());

    qApp->installTranslator(&translator);
  }

  pimpl_->ui_->setupUi(this);

  configManager_ = new ConfigManager(this);

// Init variable
  pimpl_->ui_->configListWidget->clear();
  pimpl_->ui_->analysisTab->setEnabled(false);
  pimpl_->ui_->layersTab->setEnabled(false);
  pimpl_->ui_->notificationTab->setEnabled(false);
  pimpl_->ui_->configTab->setEnabled(false);
  pimpl_->ui_->animationTab->setEnabled(false);
  pimpl_->ui_->webTab->setEnabled(false);

  nameConfig_ = "";
  CurrentConfigIndex_ = 0;  
  clearDataChanged();
  enableFields(false);

// Tips
  pimpl_->ui_->dbTab->setToolTip(tr("Dados necessários para conexão com o banco de dados."));
  pimpl_->ui_->dbTypeCmb->setToolTip(tr("Sistema Gerenciador de banco de dados."));
  pimpl_->ui_->dbAddressLed->setToolTip(tr("Endereço de acesso ao banco de dados."));
  pimpl_->ui_->dbPortLed->setToolTip(tr("Porta de acesso do Sistema Gerenciador de banco de dados."));
  pimpl_->ui_->dbUserLed->setToolTip(tr("Usuário do sistema gerenciador de banco de dados."));
  pimpl_->ui_->dbPasswordLed->setToolTip(tr("Senha do sistema gerenciador de banco de dados."));
  pimpl_->ui_->dbDatabaseLed->setToolTip(tr("Nome da base de dados."));
  pimpl_->ui_->dbCreateDatabaseBtn->setToolTip(tr("Criar base de dados."));
  pimpl_->ui_->dbCheckConnectionBtn->setToolTip(tr("Testa a conexão com o banco de dados."));
  pimpl_->ui_->dbStudyChk->setToolTip(tr("A Base de dados para estudo utilizam dados históricos."));
  pimpl_->ui_->aquisitionTab->setToolTip(tr("Dados necessários do servidor que irá realizar a coleta."));
  pimpl_->ui_->aqAddressLed->setToolTip(tr("Endereço do servidor de coleta."));
  pimpl_->ui_->aqPortLed->setToolTip(tr("Porta de serviço do servidor de coleta."));
  pimpl_->ui_->aqLogFileLed->setToolTip(tr("Nome do arquivo de log."));
  pimpl_->ui_->aqDirNameLed->setToolTip(tr("Diretório para salvar dados coletados."));
  pimpl_->ui_->aqTimeoutMinSpb->setToolTip(tr("Tempo máximo de espera por uma resposta do servidor."));
  pimpl_->ui_->aqTimeoutSecSpb->setToolTip(tr("Tempo máximo de espera por uma resposta do servidor."));
  pimpl_->ui_->analysisTab->setToolTip(tr("Configuração do servidor que irá realizar os serviços de análise."));
  pimpl_->ui_->layersTab->setToolTip(tr("Configuração do servidor que irá realizar o serviço de gerencia de planos."));
  pimpl_->ui_->notificationTab->setToolTip(tr("Configuração do servidor que irá realizar o serviço de notificação."));
  pimpl_->ui_->configTab->setToolTip(tr("Associação de cada regra de análise a uma instância do serviço de análise."));
  pimpl_->ui_->animationTab->setToolTip(tr("Configuração do servidor que irá realizar o serviço de animação."));
  pimpl_->ui_->webTab->setToolTip(tr("Customização do módulo de apresentação WEB."));

// Init services for each tab
  QSharedPointer<AdminAppTab> dbTab(new AdminAppDBTab(this,pimpl_->ui_));
  QSharedPointer<AdminAppTab> collectTab(new AdminAppCollectTab(this,pimpl_->ui_));

  tabs_.append(dbTab);
  tabs_.append(collectTab);

// Connects settings menu actions
  connect(pimpl_->ui_->newAct,    SIGNAL(triggered()), SLOT(newRequested()));
  connect(pimpl_->ui_->openAct,   SIGNAL(triggered()), SLOT(openRequested()));
  connect(pimpl_->ui_->saveAct,   SIGNAL(triggered()), SLOT(saveRequested()));
  connect(pimpl_->ui_->saveAsAct, SIGNAL(triggered()), SLOT(saveAsRequested()));
  connect(pimpl_->ui_->renameAct, SIGNAL(triggered()), SLOT(renameRequested()));
  connect(pimpl_->ui_->removeAct, SIGNAL(triggered()), SLOT(removeRequested()));
  connect(pimpl_->ui_->exitAct,   SIGNAL(triggered()), SLOT(close()));

// Bind the inputs
// Fields Database Module
  connect(pimpl_->ui_->dbTypeCmb,     SIGNAL(currentIndexChanged(int)), SLOT(ondbTab()));
  connect(pimpl_->ui_->dbAddressLed,  SIGNAL(textEdited(QString)),      SLOT(ondbTab()));
  connect(pimpl_->ui_->dbPortLed,     SIGNAL(textEdited(QString)),      SLOT(ondbTab()));
  connect(pimpl_->ui_->dbUserLed,     SIGNAL(textEdited(QString)),      SLOT(ondbTab()));
  connect(pimpl_->ui_->dbPasswordLed, SIGNAL(textEdited(QString)),      SLOT(ondbTab()));
  connect(pimpl_->ui_->dbDatabaseLed, SIGNAL(textEdited(QString)),      SLOT(ondbTab()));
  connect(pimpl_->ui_->dbStudyChk,    SIGNAL(clicked(bool)),            SLOT(ondbTab()));

// Fields Collection Module
  connect(pimpl_->ui_->aqAddressLed,    SIGNAL(textEdited(QString)), SLOT(ondbTab()));
  connect(pimpl_->ui_->aqPortLed,       SIGNAL(textEdited(QString)), SLOT(ondbTab()));
  connect(pimpl_->ui_->aqLogFileLed,    SIGNAL(textEdited(QString)), SLOT(ondbTab()));
  connect(pimpl_->ui_->aqTimeoutMinSpb, SIGNAL(valueChanged(int)),   SLOT(ondbTab()));
  connect(pimpl_->ui_->aqTimeoutSecSpb, SIGNAL(valueChanged(int)),   SLOT(ondbTab()));
  connect(pimpl_->ui_->aqDirNameLed,    SIGNAL(textEdited(QString)), SLOT(ondbTab()));

// Connects settings menu services
  connect(pimpl_->ui_->consoleAct,  SIGNAL(triggered()), SLOT(showConsoles()));
  connect(pimpl_->ui_->servicesAct, SIGNAL(triggered()), SLOT(manageServices()));

// Connects actions related to the interface
  connect(pimpl_->ui_->saveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));

  connect(pimpl_->ui_->dbCreateDatabaseBtn,  SIGNAL(clicked()), SLOT(dbCreateDatabaseRequested()));
  connect(pimpl_->ui_->dbCheckConnectionBtn, SIGNAL(clicked()), SLOT(dbCheckConnectionRequested()));
  connect(pimpl_->ui_->configListWidget,     SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked()));

// Prepare context menu to the list of settings
  pimpl_->ui_->configListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
  pimpl_->ui_->configListWidget->insertAction(NULL, pimpl_->ui_->renameAct);
  pimpl_->ui_->configListWidget->insertAction(NULL, pimpl_->ui_->removeAct); 
}

// Search Data List
bool AdminApp::searchDataList(int rowTotal, QString findName)
{
  bool find = true;

  if (rowTotal > 0)
  {
    for (int row = 0; row < rowTotal; row++)
      {
        QListWidgetItem *item = pimpl_->ui_->configListWidget->item(row);

        if (item->text() == findName)
        {
          find = false;
          break;
        }
      }
  }
 return find;
}

// New file
void AdminApp::newRequested()
{
  if (dataChanged_)
  {
     QMessageBox::warning(this, tr("TerraMA2"), tr("The configuration must be saved before performing this action."));
     pimpl_->ui_->saveBtn->setFocus();
  }
  else
  {
    nameConfig_ = tr("New Configuration");

    newData_ = true;

    pimpl_->ui_->configListWidget->setCurrentRow(pimpl_->ui_->configListWidget->count()+1);

    enableFields(true);
    newFormData();
    setDataChanged();

    bool ok;
    QString newname = QInputDialog::getText(this, tr("New configuration name..."),
                                         tr ("New configuration name: "),
                                         QLineEdit::Normal, nameConfig_, &ok);

    if (!ok || newname.isEmpty())
    {
      refresh();
      int row = pimpl_->ui_->configListWidget->count()-1;

      if (row < 0)
        enableFields(false);
      newData_ = false;
      dataChanged_ = false;
      pimpl_->ui_->renameAct->setEnabled(true);
      pimpl_->ui_->removeAct->setEnabled(true);
      return;
    }

    enableFields(true);

    nameConfig_ = newname;
// Checks if the project already exists in the ListWidget
    ok = searchDataList(pimpl_->ui_->configListWidget->count(), newname);

    if (ok)
    {
      nameConfig_ = newname;
      pimpl_->ui_->configListWidget->addItem(nameConfig_);
      setDataChanged();
      pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(false);
      pimpl_->ui_->dbCheckConnectionBtn->setEnabled(false);
      dataChanged_ = true;
      pimpl_->ui_->dbAddressLed->setFocus();
    }
    else
    {
      newData_ = false;
      dataChanged_ = false;
      QMessageBox::information(this, tr("TerraMA2"), tr("Configuration Name Exists!"));
      auto items = pimpl_->ui_->configListWidget->findItems(configManager_->getDatabase()->name_, Qt::MatchExactly);
      pimpl_->ui_->configListWidget->setCurrentItem(items[0]);
      itemClicked();
    }
  }
}

// Open file
void AdminApp::openRequested()
{
  if (dataChanged_)
  {
    QMessageBox::warning(this, tr("TerraMA2"), tr("The configuration must be saved before performing this action."));
    pimpl_->ui_->saveBtn->setFocus();
  }
  else
  {
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose file"), ".",
                                                        tr("TerraMA2 ( *.terrama2"));
    if (filename.isEmpty())
      return;

    configManager_->loadConfiguration(filename);

// Get file name
    QFileInfo info(filename);    
    nameConfig_ = info.baseName();

// Checks if the project already exists in the ListWidget
    auto status = pimpl_->ui_->configListWidget->findItems(configManager_->getDatabase()->name_, Qt::MatchContains);
    if (status.size() > 0)
      return;

// Fills fields
    fillForm();

    pimpl_->ui_->configListWidget->addItem(configManager_->getDatabase()->name_);

    enableFields(true);
    newData_ = false;
    pimpl_->ui_->saveBtn->setEnabled(false);
    pimpl_->ui_->saveAct->setEnabled(false);
    pimpl_->ui_->cancelBtn->setEnabled(false);
    pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(true);
    pimpl_->ui_->dbCheckConnectionBtn->setEnabled(true);
    auto items = pimpl_->ui_->configListWidget->findItems(configManager_->getDatabase()->name_, Qt::MatchExactly);
    pimpl_->ui_->configListWidget->setCurrentItem(items[0]);
// Check connection Database
    dbCheckConnectionRequested();
  }
}

// Rename file
void AdminApp::renameRequested()
{
  bool ok;
  QString newname = QInputDialog::getText(this, tr("Rename..."),
                                          tr ("New configuration name: "),
                                          QLineEdit::Normal, nameConfig_, &ok);

  if (!ok || newname.isEmpty())
  {
    clearDataChanged();
    return;
  }

  enableFields(true);
  QString selectedName = pimpl_->ui_->configListWidget->currentItem()->text();
  QJsonObject selectedMetadata = configManager_->getfiles().take(selectedName);

  configManager_->renameFile(selectedName, newname);

  QString version = "4.0.0-alpha1";
  selectedMetadata["name"] = newname;
  selectedMetadata["is_study"] = pimpl_->ui_->dbStudyChk->isChecked();
  selectedMetadata["version"] = version;

  for(QSharedPointer<AdminAppTab> tab: tabs_)
  {
    QMap<QString, QJsonObject> tabJson = tab->toJson();      
    selectedMetadata[tabJson.firstKey()] = tabJson.first();
  }

  nameConfig_ = newname;

  int row = pimpl_->ui_->configListWidget->currentRow();

  pimpl_->ui_->configListWidget->item(row)->setText(nameConfig_);

  dataChanged_ = true;

  setDataChanged();

  pimpl_->ui_->cancelBtn->setEnabled(false);
  pimpl_->ui_->dbAddressLed->setFocus();
}

// Save file
void AdminApp::saveRequested()
{ 
  save();
}

ConfigManager* AdminApp::getConfigManager()
{
 return configManager_;
}

void AdminApp::save()
{
    QJsonObject metadata;
    QString err;
    if (!validateDbData(err))
    {
      if(!err.isEmpty())
       {
        dataChanged_ = true;
        QMessageBox::warning(this, tr("Error validating data to save..."), err);
        return;
       }
    }
    QString newfilename;

    if (newData_) // Save new file or SaveAs file
    {
      newfilename = QFileDialog::getSaveFileName(this, tr("Enter the name for the configuration"),
                                                  ".", tr("Configuration (*.terrama2)"));
      if(newfilename.isEmpty())
        return;

      if(!newfilename.endsWith(".terrama2"))
           newfilename.append(".terrama2");

     }
     else
     { // Save file changed
       QString selectedname = pimpl_->ui_->configListWidget->currentItem()->text();
       QJsonObject fileSeleted = configManager_->getfiles().take(selectedname);
       newfilename = fileSeleted.take("path").toString();
     }

     QString version = "4.0.0-alpha1";
     metadata["is_study"] = pimpl_->ui_->dbStudyChk->isChecked();
     metadata["name"] = nameConfig_;
     metadata["version"] = version;

     if (configManager_->getCollection()->params_ == "")
       {
        QString params = "%c";
        QString cmd = "coleta";
        configManager_->getCollection()->params_ = params;
        configManager_->getCollection()->cmd_ = cmd;
      }

     for(QSharedPointer<AdminAppTab> tab: tabs_)
     {
       QMap<QString, QJsonObject> tabJson = tab->toJson();
       metadata[tabJson.firstKey()] = tabJson.first();
     }

     QFile saveFile(newfilename);

     saveFile.open(QIODevice::WriteOnly);

     QJsonDocument jsondoc(metadata);

     saveFile.write(jsondoc.toJson());

     saveFile.close();

     metadata.insert("path",newfilename);
     configManager_->insertFile(nameConfig_, metadata);

     pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(true);
     pimpl_->ui_->dbCheckConnectionBtn->setEnabled(true);
     pimpl_->ui_->saveBtn->setEnabled(false);
     pimpl_->ui_->saveAct->setEnabled(false);
     pimpl_->ui_->cancelBtn->setEnabled(false);

     pimpl_->ui_->configListWidget->setCurrentRow(0);

     dataChanged_ = false;
     newData_= false;
}

// SaveAs file
void AdminApp::saveAsRequested()
{
  newData_ = true;
  saveRequested();
}

// Cancel
void AdminApp::cancelRequested()
{
  if(newData_)
  {
   int row = pimpl_->ui_->configListWidget->count();
   delete pimpl_->ui_->configListWidget->takeItem(row-1);

   refresh();

   row = pimpl_->ui_->configListWidget->count();

   if (row <= 0)
     enableFields(false);
  }
  else
  {
    refresh();
    pimpl_->ui_->dbAddressLed->setFocus();
  }
  newData_ = false;
  dataChanged_ = false;
}

// Create Database
void AdminApp::dbCreateDatabaseRequested()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  qApp->processEvents();

  try
  {
    QString err;
    if (!validateDbData(err))
    {
      if(!err.isEmpty())
        QMessageBox::warning(this, tr("Error validating data for connection database..."), err);
        return;
    }

    configManager_->setDatabase(tabs_[0]->toJson().first());

    Database* database = configManager_->getDatabase();

    terrama2::core::ApplicationController::getInstance().createDatabase(database->dbName_.toStdString(),
                                                                        database->user_.toStdString(),
                                                                        database->password_.toStdString(),
                                                                        database->host_.toStdString(),
                                                                        database->port_ );
  }

  catch(const terrama2::Exception& e)
  {
    QString messageError = "TerraMA2\n\n";

    if (const QString* d = boost::get_error_info<terrama2::ErrorDescription>(e))
    {
      messageError.append(d);
    }

    QMessageBox::warning(nullptr, "TerraMA2", messageError);
  }

  catch(const te::common::Exception& e)
  {
    QString messageError = "TerraMA2\n\n";

    messageError.append(e.what());

    QMessageBox::warning(nullptr, "TerraMA2", messageError);
  }

  catch(const std::exception& e)
  {
    QString messageError = "TerraMA2\n\n";

    if (const QString* d = boost::get_error_info<terrama2::ErrorDescription>(e))
    {
      messageError.append(d);
    }

    QMessageBox::warning(nullptr, "TerraMA2", messageError);
  }

  catch(...)
  {
    //TODO: log
  }

  dbCheckConnectionRequested();
  QApplication::restoreOverrideCursor();
}

// Check connection Database
void AdminApp::dbCheckConnectionRequested()
{
  bool ok;
  try
  {
    QString err;
    if (!validateDbData(err))
    {
      if(!err.isEmpty())
        QMessageBox::warning(this, tr("Error validating data for connection database..."), err);
        return;
    }

    configManager_->setDatabase(tabs_[0]->toJson().first());

    Database* database = configManager_->getDatabase();

    ok = terrama2::core::ApplicationController::getInstance().checkConnectionDatabase(database->dbName_.toStdString(),
                                                                                 database->user_.toStdString(),
                                                                                 database->password_.toStdString(),
                                                                                 database->host_.toStdString(),
                                                                                 database->port_ );

    if (ok)
      pimpl_->ui_->dbCheckConnectionBtn->setIcon(QIcon::fromTheme("confirm"));
    else
      pimpl_->ui_->dbCheckConnectionBtn->setIcon(QIcon::fromTheme("cancel"));

  }

  catch(const terrama2::Exception& e)
  {
    QString messageError = "TerraMA2 finished with errors!\n\n";

    if (const QString* d = boost::get_error_info<terrama2::ErrorDescription>(e))
    {
      messageError.append(d);
    }

    QMessageBox::critical(nullptr, "TerraMA2", messageError);
  }

  catch(const te::common::Exception& e)
  {
    QString messageError;

    messageError.append(e.what());

    QMessageBox::critical(nullptr, "TerraMA2", messageError);
  }

  catch(...)
  {
    //TODO: log
  }

}

// Refresh
void AdminApp::refresh()
{
  int row = pimpl_->ui_->configListWidget->count()-1;

  if (row < 0)
  {
    clearFormData();
  }
  else
  {
    pimpl_->ui_->configListWidget->setCurrentRow(row);
    QString selectedname = pimpl_->ui_->configListWidget->currentItem()->text();
    QJsonObject selectedMetadata = configManager_->getfiles().take(selectedname);

    configManager_->setDataForm(selectedMetadata);

    fillForm();
  }
}

// Remove
void AdminApp::removeRequested()
{
  QMessageBox::StandardButton answer;
  answer = QMessageBox::question(this, tr("Remove configuration..."),
                                       tr("Do you Want to remove this configuration?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
  if(answer == QMessageBox::No)
   return;

  QString filename = pimpl_->ui_->configListWidget->currentItem()->text();

  configManager_->removeFile(filename);

  delete pimpl_->ui_->configListWidget->currentItem();

// Refresh list
  refresh();

  int row = pimpl_->ui_->configListWidget->count();

  if (row <= 0)
    enableFields(false);

  newData_= false;
  dataChanged_ = false;
}

// Service Dialog
void AdminApp::manageServices()
{
  if (dataChanged_)
  {
    QMessageBox::warning(this, tr("TerraMA2"), tr("The configuration must be saved before performing this action."));
    pimpl_->ui_->saveBtn->setFocus();
  }
  else
  {
    QString nameConfig = pimpl_->ui_->configListWidget->currentItem()->text();

    ServicesDialog dlg(this, *configManager_, nameConfig);

    dlg.exec();
  }
}

// Console Dialog
// TODO: use the show consoles when you have logs;
void AdminApp::showConsoles()
{
// ConsoleDialog dlg(this);
// dlg.exec();
}

//! Validate connection data in the database interface. Return true if ok to save.
bool AdminApp::validateDbData(QString& err)
{
  err = "";
// Database
  if(pimpl_->ui_->dbAddressLed->text().trimmed().isEmpty())
    err = tr("Please fill in the 'Address' field Database'.");
  else if(pimpl_->ui_->dbDatabaseLed->text().trimmed().isEmpty())
    err = tr("Please fill in the 'Database' field Database'.");
  else if(!pimpl_->ui_->dbPortLed->hasAcceptableInput())
    err = tr("The field 'Port' of 'Database' must be filled\n"
             "with an integer between 0 and 65535.");
  else if(pimpl_->ui_->dbUserLed->text().trimmed().isEmpty())
    err = tr("Please fill in the 'Username' field Database'.");
  else if(pimpl_->ui_->dbPasswordLed->text().trimmed().isEmpty())
    err = tr("Please fill in the 'Password' field Database'.");

// Collection
  else if(pimpl_->ui_->aqLogFileLed->text().trimmed().isEmpty())
    err = tr("Please fill in the 'Log file' field 'Collection'.");
  else if(!pimpl_->ui_->aqPortLed->hasAcceptableInput())
    err = tr("The field 'Port' of 'Collection' must be filled\n"
               "with an integer between 0 and 65535.");

// Directory to save data collected
  QString collectionDirPath = pimpl_->ui_->aqDirNameLed->text().trimmed();
  collectionDirPath = collectionDirPath.replace(QChar('\\'), QChar('/'));
  pimpl_->ui_->aqDirNameLed->setText(collectionDirPath);

// This directory is optional. If the User informed, then try to create it
  if(!collectionDirPath.isEmpty())
  {
    QDir dir(collectionDirPath);
    if(!dir.exists() && !dir.mkdir(collectionDirPath))
    {
      err = tr("Cannot create the directory: ") + collectionDirPath;
    }
// Create Log File
    QString collectionFile = pimpl_->ui_->aqLogFileLed->text().trimmed();
    if (!collectionFile.isEmpty())
    {
      QFile file(dir.filePath(collectionFile));
      if (!file.open(QIODevice::ReadWrite))
        err = tr("Cannot create the file: ") + collectionFile;
    }
  }

  return (err == "");
}

//! Enable or Disable fields of form
void AdminApp::enableFields(bool mode)
 {
// action
  pimpl_->ui_->saveAct->setEnabled(mode);
  pimpl_->ui_->renameAct->setEnabled(mode);
  pimpl_->ui_->removeAct->setEnabled(mode);
  pimpl_->ui_->saveAsAct->setEnabled(mode);
  pimpl_->ui_->servicesAct->setEnabled(mode);
  pimpl_->ui_->consoleAct->setEnabled(false);
  pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(mode);
  pimpl_->ui_->dbCheckConnectionBtn->setEnabled(mode);
  pimpl_->ui_->saveBtn->setEnabled(mode);
  pimpl_->ui_->cancelBtn->setEnabled(mode);

// fields Database
  pimpl_->ui_->dbTypeCmb->setEnabled(mode);
  pimpl_->ui_->dbAddressLed->setEnabled(mode);
  pimpl_->ui_->dbPortLed->setEnabled(mode);
  pimpl_->ui_->dbUserLed->setEnabled(mode);
  pimpl_->ui_->dbPasswordLed->setEnabled(mode);
  pimpl_->ui_->dbDatabaseLed->setEnabled(mode);
  pimpl_->ui_->dbStudyChk->setEnabled(mode);

// fields Collect
  pimpl_->ui_->aqAddressLed->setEnabled(mode);
  pimpl_->ui_->aqPortLed->setEnabled(mode);
  pimpl_->ui_->aqLogFileLed->setEnabled(mode);
  pimpl_->ui_->aqTimeoutMinSpb->setEnabled(mode);
  pimpl_->ui_->aqTimeoutSecSpb->setEnabled(mode);
  pimpl_->ui_->aqDirNameLed->setEnabled(mode);

// TabWidget
  pimpl_->ui_->tabWidget->setEnabled(mode);
}

void AdminApp::setDataChanged()
{
  dataChanged_ = true;
  pimpl_->ui_->saveBtn->setEnabled(true);
  pimpl_->ui_->saveAct->setEnabled(true);
  pimpl_->ui_->cancelBtn->setEnabled(true);
  pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(true);
  pimpl_->ui_->dbCheckConnectionBtn->setEnabled(true);
}

void AdminApp::clearDataChanged()
{
  dataChanged_ = false;
  pimpl_->ui_->saveBtn->setEnabled(false);
  pimpl_->ui_->saveAct->setEnabled(false);
  pimpl_->ui_->cancelBtn->setEnabled(false);
  pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(false);
  pimpl_->ui_->dbCheckConnectionBtn->setEnabled(false);
}

void AdminApp::ondbTab()
{
  if (pimpl_->ui_->dbAddressLed->text().isEmpty() || pimpl_->ui_->dbUserLed->text().isEmpty()
                                                  || pimpl_->ui_->dbPasswordLed->text().isEmpty()
                                                  || pimpl_->ui_->dbDatabaseLed->text().isEmpty()
                                                  || pimpl_->ui_->dbPortLed->text().isEmpty())
  {
    pimpl_->ui_->saveBtn->setEnabled(true);
    pimpl_->ui_->saveAct->setEnabled(true);
    pimpl_->ui_->cancelBtn->setEnabled(true);
    pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(false);
    pimpl_->ui_->dbCheckConnectionBtn->setEnabled(false);
  }
  else
  {
    pimpl_->ui_->saveBtn->setEnabled(true);
    pimpl_->ui_->saveAct->setEnabled(true);
    pimpl_->ui_->cancelBtn->setEnabled(true);
    pimpl_->ui_->dbCreateDatabaseBtn->setEnabled(true);
    pimpl_->ui_->dbCheckConnectionBtn->setEnabled(true);
  }
  dataChanged_ = true;
}

// Clear Form Data
void AdminApp::clearFormData()
{
// fields tab Database
  pimpl_->ui_->dbTypeCmb->setCurrentIndex(0);
  pimpl_->ui_->dbAddressLed->setText("");
  pimpl_->ui_->dbPortLed->setText("");
  pimpl_->ui_->dbUserLed->setText("");
  pimpl_->ui_->dbPasswordLed->setText("");
  pimpl_->ui_->dbDatabaseLed->setText("");
  pimpl_->ui_->dbStudyChk->setChecked(false);

// fields tab collect
  pimpl_->ui_->aqAddressLed->setText("");
  pimpl_->ui_->aqPortLed->setText("");
  pimpl_->ui_->aqLogFileLed->setText("");
  pimpl_->ui_->aqTimeoutMinSpb->setValue(3);
  pimpl_->ui_->aqTimeoutSecSpb->setValue(0);
  pimpl_->ui_->aqDirNameLed->setText("");
}

// Add data patterns in a new setting
void AdminApp::newFormData()
{
  QString hostname = QHostInfo::localHostName();

// fields tab Database
  pimpl_->ui_->dbTypeCmb->setCurrentIndex(0);
  pimpl_->ui_->dbAddressLed->setText("localhost");
  pimpl_->ui_->dbPortLed->setText("5432");
  pimpl_->ui_->dbUserLed->setText("");
  pimpl_->ui_->dbPasswordLed->setText("");
  pimpl_->ui_->dbDatabaseLed->setText("");
  pimpl_->ui_->dbStudyChk->setChecked(false);

// fields tab collect
  pimpl_->ui_->aqAddressLed->setText(hostname);
  pimpl_->ui_->aqPortLed->setText("32100");
  pimpl_->ui_->aqLogFileLed->setText("log_coleta.txt");
  pimpl_->ui_->aqTimeoutMinSpb->setValue(3);
  pimpl_->ui_->aqTimeoutSecSpb->setValue(0);
  pimpl_->ui_->aqDirNameLed->setText("");
}

// Item Clicked ListWidget
void AdminApp::itemClicked()
{
  QString selectedName = pimpl_->ui_->configListWidget->currentItem()->text();
  QMap<QString,QJsonObject> fileList = configManager_->getfiles();

  nameConfig_ = selectedName;

  if (!fileList.contains(selectedName))
    return;

  QJsonObject selectedMetadata = configManager_->getfiles().take(selectedName);

  configManager_->setDataForm(selectedMetadata);

  fillForm();

  dbCheckConnectionRequested();
}

// Destructor
AdminApp::~AdminApp()
{
  delete pimpl_;
}

//! Event called when the user requests to quit the application
void AdminApp::closeEvent(QCloseEvent* close)
{
  if (dataChanged_)
  {
   int ret = QMessageBox::warning(this, tr("TerraMA2"),
                                  tr("Save changes to configuration before closing?"),
                                  QMessageBox::Save | QMessageBox::Discard
                                  | QMessageBox::Cancel,
                                  QMessageBox::Save);
  switch (ret)
  {
    case QMessageBox::Save:
    {
      // Save was clicked
      saveRequested();
      if (dataChanged_)
      {
        close->ignore();
        pimpl_->ui_->dbAddressLed->setFocus();
        break;
      }
      else
      {
        close->accept();
        break;
      }
    }
    case QMessageBox::Discard:
      // Don't Save was clicked
      close->accept();
      break;
    case QMessageBox::Cancel:
      // Cancel was clicked
      close->ignore();
      pimpl_->ui_->dbAddressLed->setFocus();
      break;
    default:
      // should never be reached
      break;
   }
 }
}

// fills fields
void AdminApp::fillForm()
{
// Database tab
  pimpl_->ui_->dbTypeCmb->setCurrentIndex(pimpl_->ui_->dbTypeCmb->findText(configManager_->getDatabase()->driver_));
  pimpl_->ui_->dbAddressLed->setText(configManager_->getDatabase()->host_);
  pimpl_->ui_->dbUserLed->setText(configManager_->getDatabase()->user_);
  pimpl_->ui_->dbDatabaseLed->setText(configManager_->getDatabase()->dbName_);
  pimpl_->ui_->dbPortLed->setText(QString::number(configManager_->getDatabase()->port_));
  pimpl_->ui_->dbPasswordLed->setText(configManager_->getDatabase()->password_);
  pimpl_->ui_->dbStudyChk->setChecked(configManager_->getDatabase()->study_.toLower() == "true");

// Collect tab
  pimpl_->ui_->aqAddressLed->setText(configManager_->getCollection()->address_);
  pimpl_->ui_->aqPortLed->setText(QString::number(configManager_->getCollection()->servicePort_));
  pimpl_->ui_->aqLogFileLed->setText(configManager_->getCollection()->logFile_);
  pimpl_->ui_->aqDirNameLed->setText(configManager_->getCollection()->dirPath_);
  pimpl_->ui_->aqTimeoutMinSpb->setValue(configManager_->getCollection()->timeout_ / 60);
  pimpl_->ui_->aqTimeoutSecSpb->setValue(configManager_->getCollection()->timeout_ % 60);

  clearDataChanged();
  enableFields(true);
}

