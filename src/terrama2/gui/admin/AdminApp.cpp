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

// Qt
#include <QIcon>
#include <QStringList>
#include <QToolBar>
#include <QTranslator>
#include <QFileDialog>

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

  connect(pimpl_->ui_->openAct, SIGNAL(triggered()), SLOT(openRequested()));
 
}

void AdminApp::openRequested()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Choose file"), ".", tr("TerraMA2 ( *.terrama2"));

  if (filename.isEmpty())
     return;

  configManager_->loadConfiguration(filename);

// fills fields
  fillForm();
}

AdminApp::~AdminApp()
{
  delete pimpl_;
}

// fills fields
void AdminApp::fillForm()
{
//  pimpl_->ui_->dbTypeCmb->setText(configManager_->getDatabase()->driver_);
//  pimpl_->ui_->dbAddressLed->setText(configManager_->getDatabase()->host);
  pimpl_->ui_->dbUserLed->setText(configManager_->getDatabase()->user_);
  pimpl_->ui_->dbDatabaseLed->setText(configManager_->getDatabase()->dbName_);
//  pimpl_->ui_->dbPortLed->setText(configManager_->getDatabase()->port_);
  pimpl_->ui_->dbPasswordLed->setText(configManager_->getDatabase()->password_);
  pimpl_->ui_->dbStudyChk->setChecked(configManager_->getDatabase()->study_);
}

