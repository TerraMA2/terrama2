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

// TerraMA2 Tab controls
#include "ConfigAppWeatherTab.hpp"

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
    pimpl_(new ConfigApp::Impl), currentTabIndex_(0)
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

// Init services for each tab
  ConfigAppWeatherTab* weatherTab = new ConfigAppWeatherTab(this, ui());

  tabList_.append(weatherTab);

  connect(pimpl_->ui_->mainTabWidget, SIGNAL(currentChanged(int)), SLOT(tabChangeRequested(int)));

  connect(weatherTab, SIGNAL(serverChanged()), this, SLOT(disableRefreshAction()));

}

ConfigApp::~ConfigApp()
{
  for (auto tab: tabList_)
    delete tab;
  delete pimpl_;
}

Ui::ConfigAppForm* ConfigApp::ui() const
{
  return pimpl_->ui_;
}

void ConfigApp::tabChangeRequested(int index)
{
  if(index != currentTabIndex_)
  {
    // Verifica se o tab pode ser trocado
    ConfigAppTab* tab = tabList_.at(currentTabIndex_);

    if(tab->verifyAndEnableChange(true))
      currentTabIndex_ = index;
    else
      pimpl_->ui_->mainTabWidget->setCurrentIndex(currentTabIndex_);
  }
}

void ConfigApp::disableRefreshAction()
{
  pimpl_->ui_->refreshAct->setEnabled(false);
}
