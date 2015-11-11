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
  \file terrama2/gui/config/ConfigApp.hpp

  \brief Main GUI for TerraMA2 Config application.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPP_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPP_HPP__

// TerraMA2
#include "ui_ConfigAppForm.h"

// TerraMA2 Services
#include "../../ws/collector/client/Client.hpp"

// Boost
#include <boost/noncopyable.hpp>

// Qt
#include <QMainWindow>
#include <QSharedPointer>


class ConfigAppTab;
class ConfigAppWeatherTab;
class ConfigManager;

class ConfigApp : public QMainWindow, private  boost::noncopyable
{
  Q_OBJECT

  public:
  
    /*! Default constructor.
      \exception terrama2::InitializationError <It may be raised when cannot find TerraMA2 icons library folder>

    */
    ConfigApp(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    //! Destructor.
    ~ConfigApp();

    //! It retrieves the ui from pimpl_
    Ui::ConfigAppForm* ui() const;

    //! It sets the current tab index
    void setCurrentTabIndex(const int& index);

    //! It retrieves the current tab index in application runtime
    int getCurrentTabIndex() const;

    //! It retrieves the weather tab
    QSharedPointer<ConfigAppWeatherTab> getWeatherTab() const;

    //! It retrieves the collector client
    QSharedPointer<terrama2::ws::collector::client::Client> getClient() const;

  signals:
    //! It
    void notifyActiveTab(ConfigAppTab&, QWidget&);

  private slots:
    //! Slot triggered when tab index clicked. It handles global tabs among application
    void tabChangeRequested(int);

    //! Slot triggered in open button click. It load the terrama2 configuration and it dispatches load for each tab
    void openRequested();
  
  private:
    struct Impl;

    Impl* pimpl_; //!< Pimpl idiom

    int currentTabIndex_; //!< index of active tab

    QSharedPointer<ConfigManager> configManager_; //! It contains metadata from terrama2 administration file

    QSharedPointer<ConfigAppWeatherTab> weatherTab_; //! Attribute for handling WeatherTab

    QSharedPointer<terrama2::ws::collector::client::Client> client_;  //! gsoap collector client
};

#endif // __TERRAMA2_GUI_CONFIG_CONFIGAPP_HPP__
