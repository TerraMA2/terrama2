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
  \class ConfigApp

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
#include "../core/ConfigManager.hpp"

// Boost
#include <boost/noncopyable.hpp>

// Qt
#include <QMainWindow>
#include <QSharedPointer>


// Forward declarations

namespace terrama2
{
  namespace gui
  {
    namespace core
    {
      class ConfigManager;
    }

    namespace config
    {
      // Forward declaration
      class ConfigAppTab;
      class ConfigAppWeatherTab;

      /*!
        \class ConfigApp

        \brief TerraMA2 Configuration Window

               It initializes entire TerraM
      */
      class ConfigApp : public QMainWindow, private  boost::noncopyable
      {
        Q_OBJECT

        public:

          /*! Default constructor.
            \param parent Represents the QT Widget owner (default nullptr)
            \param flags  It handling gui window kind (0 defined as Widget)
            \exception terrama2::InitializationException It may be raised when cannot find TerraMA2 icons library folder
          */
          ConfigApp(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);

          //! Destructor.
          ~ConfigApp();

          /*!
            \brief It retrieves the ui from pimpl_
            \return A pointer to ConfigAppTab GUI members containing qt widgets
          */
          Ui::ConfigAppForm* ui() const;

          /*!
           \brief It resets the gui application to initial state
          */
          void unload();

          /*!
           \brief It sets the current tab index
          */
          void setCurrentTabIndex(const int& index);

          /*!
           \brief It retrieves the current tab index in application runtime
           \return A int value containing gui active tab index
          */
          int getCurrentTabIndex() const;

          /*!
           \brief It retrieves the weather tab
           \return A shared pointer to ConfigAppWeatherTab class
          */
          QSharedPointer<ConfigAppWeatherTab> getWeatherTab() const;

          /*!
           \brief It retrieves the collector client
           \return A shared pointer to terrama2::ws::collector::client::Client class.
          */
          QSharedPointer<terrama2::ws::collector::client::Client> getClient() const;

          /*!
           \brief It retrieves the terrama2::gui::core::ConfigManager instance containing application settings
           \return A shared pointer to terrama2::gui::core::ConfigManager class. It contains application config.
          */
          QSharedPointer<terrama2::gui::core::ConfigManager> getConfiguration() const;

        private slots:
          /*!
           \brief Slot triggered when tab index has been clicked. It handles global tabs among application runtime
           \param index Contains a requested tab index value
          */
          void tabChangeRequested(int index);

          /*!
           \brief Slot triggered in open button click. It load the TerraMA2 configuration and it dispatches load for each tab
          */
          void openRequested();

        private:
          struct Impl;

          Impl* pimpl_; //!< Pimpl idiom

          int currentTabIndex_; //!< index of active tab

          QSharedPointer<terrama2::gui::core::ConfigManager> configManager_; //!< It contains metadata from TerraMA2 administration file

          QSharedPointer<ConfigAppWeatherTab> weatherTab_; //!< Attribute for handling WeatherTab

          QSharedPointer<terrama2::ws::collector::client::Client> client_;  //!< gSOAP collector client
      };
    }
  }
}

#endif // __TERRAMA2_GUI_CONFIG_CONFIGAPP_HPP__
