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
#include "../../core/Utils.hpp"
#include "Exception.hpp"
#include "../../core/ApplicationController.hpp"

// TerraMA2 Tab controls
#include "ConfigAppWeatherTab.hpp"

// Qt
#include <QTranslator>
#include <QFileDialog>
#include <QMessageBox>


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

ConfigApp::ConfigApp(QWidget* parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags),
    pimpl_(new ConfigApp::Impl), currentTabIndex_(0),
    weatherTab_(nullptr)
    //client_() // Harded code
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

// Initialize services
  services_ = new ServiceHandler(this);

  weatherTab_.reset(new ConfigAppWeatherTab(this, pimpl_->ui_));

// Connect tabs to changing index
  connect(pimpl_->ui_->mainTabWidget, SIGNAL(currentChanged(int)), SLOT(tabChangeRequested(int)));
  connect(pimpl_->ui_->openAct, SIGNAL(triggered()), SLOT(openRequested()));

  // Disable form until load terrama2 config file
  pimpl_->ui_->centralwidget->setEnabled(false);

}

ConfigApp::~ConfigApp()
{
  delete services_;
  delete pimpl_;
}

Ui::ConfigAppForm* ConfigApp::ui() const
{
  return pimpl_->ui_;
}

void ConfigApp::setCurrentTabIndex(const int& index)
{
  currentTabIndex_ = index;
}

int ConfigApp::getCurrentTabIndex() const
{
  return currentTabIndex_;
}

void ConfigApp::tabChangeRequested(int index)
{
  if(index != currentTabIndex_)
  {
    // Check if the tab may be changed
    switch(index)
    {
      case 0:
        weatherTab_->askForChangeTab(index);
        break;
      default:
      {
        //TODO: tab handling
      }
//        QMessageBox::information(this, tr("TerraMA2"), tr("Not Implemented yet"));
    }
  }
}

void ConfigApp::openRequested()
{
  try
  {
    QString file = QFileDialog::getOpenFileName(this, tr("Choice TerraMA2 file"),
                                               ".", tr("TerraMA2 (*.terrama2)"));
    if (!file.isEmpty())
    {
      services_->loadConfiguration(file);

      terrama2::core::ApplicationController::getInstance().loadProject(file.toStdString());

      pimpl_->ui_->centralwidget->setEnabled(true);

      if (client_ != nullptr)
        client_.clear();

      // TODO: temp code for dev. It will get port from file future commits
      client_.reset(new terrama2::ws::collector::Client("http://localhost:17000"));

      weatherTab_->load();

      // TODO: load configuration to the client
    }
  }
  catch(const terrama2::Exception& e)
  {
    const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::critical(this, tr("TerraMA2"), *message);
  }
}

QSharedPointer<ConfigAppWeatherTab> ConfigApp::getWeatherTab() const
{
  return weatherTab_;
}

QSharedPointer<terrama2::ws::collector::Client> ConfigApp::getClient() const
{
  return client_;
}
