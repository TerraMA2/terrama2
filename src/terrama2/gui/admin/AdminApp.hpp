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
  \file terrama2/gui/config/AdminApp.hpp

  \brief Main GUI for TerraMA2 Admin application.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_ADMIN_ADMINAPP_HPP__
#define __TERRAMA2_GUI_ADMIN_ADMINAPP_HPP__

// TerraMA2
#include "ui_AdminAppForm.h"
#include "../core/ConfigManager.hpp"

// Boost
#include <boost/noncopyable.hpp>

// Qt
#include <QMainWindow>
#include <QList>
#include <QSharedPointer>


namespace terrama2
{
  namespace gui
  {
    namespace admin
    {
      class AdminAppTab;
      /*!
        \class AdminApp

        \brief Main dialog for TerraMA2 Administration module.
         Before starting the implementation of TerraMA2 Configuration Module is
         necessary to define the connection to the database and the servers that will be holding the
         (collection, plans, analysis, reporting and animation) services. To facilitate the configuration
         process of the TerraMA2 execution environment, an administration module was developed. Through this
         module it can create, modify and consult a configuration file that will beused by modules TerraMA2.
       */

      class AdminApp : public QMainWindow, private boost::noncopyable
      {
          Q_OBJECT

        public:

          /*!
           * \brief Constructor.
           * Init variable, find icon theme library, load icon theme library, load idioms, load Tips,
           * init services for each tab and connects settings menu actions.
           * \exception - terrama2::InitializationError when the could not find TerraMA2 icon library folder.
           */
          AdminApp(QWidget* parent = 0);

          //! Destructor.
          ~AdminApp();

          //! Fill fields.
          void fillForm();

          //! It saves current data.
          void save();

          //! Get Configuration Manager.
          terrama2::gui::core::ConfigManager* getConfigManager();

        signals:

        protected:

          //! Event called when the user requests to quit the application.
          void closeEvent(QCloseEvent* close);

        private slots:

          //! Signal called when the user clicked on new button to create a new configuration document.
          void newRequested();

          //! Signal called when the user clicked on button open configuration to open a configuration document.
          void openRequested();

          //! Signal called when the user clicked on save button configuration to save a new document and save changes to an existing configuration document.
          void saveRequested();

          //! Signal called when the user clicked on saveAs button configuration to save a configuration file with a different name but with the same settings.
          void saveAsRequested();

          //! Signal called when the user clicked on button rename configuration to rename the name configuration document.
          void renameRequested();

          //! Signal called when the user clicked on remove button configuration to remove a configuration document.
          void removeRequested();

          //! Signal called when the user clicked on cancel button configuration to cancel changes to the configuration document.
          void cancelRequested();

          //! Signal called when the user clicked on Create Database button to create a new database.
          void dbCreateDatabaseRequested();

          //! Signal called when the user clicked on Check Connection button to check if there is a database.
          void dbCheckConnectionRequested();

          //! Manage Services Dialog
          void manageServices();

          void showConsoles();

          //! Enables buttons and indicates that the data has been modified.
          void setDataChanged();

          //! Disable buttons and indicates that the data was not modified.
          void clearDataChanged();

          //! Clean all fields of the form.
          void clearFormData();

          //! Add data patterns in a new setting.
          void newFormData();

          //! Validade fields of form.
          void ondbTab();

          //! Signal called when the user Item Clicked in List.
          void itemClicked();

          //! Updates the data in the form.
          void refresh();

        private:

          struct Impl;

          terrama2::gui::core::ConfigManager* configManager_; //!< Configuration Manager.

          QString nameConfig_; //!< Current Configuration Name.
          bool newData_; //!< Current new configuration.
          bool dataChanged_; //!< Indicates that the data has changed.
          int CurrentConfigIndex_; //!< Indice current configuration.

          QList<QSharedPointer<AdminAppTab>> tabs_; //!< List of TerraMA2 Administration Tabs.

          Impl* pimpl_;  //!< Pimpl idiom.

          //! Enable or Disable fields of form.
          void enableFields(bool mode);

          //! // Search Data ListWidget.
          bool searchDataList(int rowTotal, QString findName);

          //! Validate connection data in the database interface. Return true if ok to save.
          bool validateDbData(QString& err);

      };
    }
  }
}
#endif // __TERRAMA2_GUI_ADMIN_ADMINAPP_HPP__
