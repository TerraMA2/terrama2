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
  \file terrama2/gui/config/ConfigApp.cpp

  \brief Main GUI for TerraMA2 Config application.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ConfigApp.hpp"
#include "ui_ConfigAppForm.h"
#include "../../core/Utils.hpp"
#include "Exception.hpp"

// Qt
#include <QStringList>
#include <QTranslator>


struct ConfigApp::Impl
{
  Ui::ConfigAppForm* ui_;
  
  Impl()
    : ui_(new Ui::ConfigAppForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }
};

ConfigApp::ConfigApp(QWidget* parent)
  : QMainWindow(parent),
    pimpl_(new ConfigApp::Impl)
{
// Find TerraMA2 icon theme library
  std::string icon_path = terrama2::core::FindInTerraMA2Path("share/terrama2/icons");

  if (icon_path.empty())
  {
    throw terrama2::InitializationError() << terrama2::ErrorDescription(tr("Could not find TerraMA2 icons library folder."));
  }

// load icon theme
  QStringList ithemes = QIcon::themeSearchPaths();

  ithemes.push_back(icon_path.c_str());

  QIcon::setThemeSearchPaths(ithemes);

  QIcon::setThemeName("terrama2");

// Set Default Language
  std::string language_path = terrama2::core::FindInTerraMA2Path("share/terrama2/translations/terrama2_config_en_US.qm");

  if (!language_path.empty())
  {
    QTranslator translator;
    translator.load(language_path.c_str());
    qApp->installTranslator(&translator);
  }

  pimpl_->ui_->setupUi(this);

  QObject::connect(pimpl_->ui_->insertServerBtn, SIGNAL(clicked()), this, SLOT(onInsertServerClick()));
  QObject::connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), this, SLOT(onCancelClick()));
}

ConfigApp::~ConfigApp()
{
  delete pimpl_;
}

void ConfigApp::onInsertServerClick()
{
  pimpl_->ui_->ServerGroupPage->hide();
  pimpl_->ui_->ServerPage->show();
}

void ConfigApp::onCancelClick()
{
  pimpl_->ui_->ServerPage->hide();
  pimpl_->ui_->ServerGroupPage->show();
}
