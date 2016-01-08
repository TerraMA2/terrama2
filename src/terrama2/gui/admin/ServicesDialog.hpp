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
  \file terrama2/gui/admin/ServicesDialog.hpp

  \brief Services QT Dialog Header

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_ADMIN_SERVICESDIALOG_HPP__
#define __TERRAMA2_GUI_ADMIN_SERVICESDIALOG_HPP__  

// TerraMA2
#include "../../ws/collector/client/Client.hpp"

// QT
#include "ui_ServicesDialogForm.h"

// Boost
#include <boost/noncopyable.hpp>  

namespace terrama2
{
  namespace gui
  {
    namespace core
    {
      class ConfigManager;
      struct CommonData;
    }
  }
}

namespace terrama2
{
  namespace gui
  {
    namespace admin
    {
      class AdminApp;

      /*!
        \class ServicesDialog

        \brief Class that manages the execution to services.
         The Service Manager assists you in activating, checking and completion
         the services associated with a configuration.
         The manager window has the table
         services. This table contains entries for collection services, plans,
         notification, animation, and an entry for each instance of analysis service,
         informing the user about the activation status, location (IP address and port) and settings for each service.
       */

      class ServicesDialog : public QDialog, private boost::noncopyable
      {
          Q_OBJECT

        public:

          /*!
           * \brief Constructor
           * \param adminapp - main window administration.
           * \param ConfigManager - configuration manager.
           * \param nameConfig - name of the current configuration.
           */
          ServicesDialog(terrama2::gui::admin::AdminApp *adminapp, terrama2::gui::core::ConfigManager&, QString nameConfig);

          //! Destructor.
          ~ServicesDialog();

        private slots:

          //! Signal called when the user requests that marked lines are "checked with ping".
          void verifyRequested();

          //! Save changed data in the dialog.
          void saveRequested();

          //! Signal called when the user requests that marked lines are executed.
          void execRequested();

          //! Signal called when the user requests that marked lines are "closed".
          void closeRequested();

          //! Mark the table data has changed.
          void setDataChanged(int row, int col);

          //! Mark the data have not changed.
          void clearDataChanged();

        private:

          //! Fills a line of the data table.
          void setLine(int line, const QString& module, const terrama2::gui::core::CommonData& data);

          //! Structure fills with data from field commands and table line parameters.
          void getLine(int line, terrama2::gui::core::CommonData& data);

          //! Returns list with indices of selected lines.
          void getSelectedLines(QList<int>& list);

          //! Fill table with configuration data.
          void setDialogData(QString nameConfig);

          /*!
           * \brief Executes the command received as a parameter.
           * \param line - type service. Ex. Collect.
           * \param cmd - command executable. Ex. ./terrama2_mod_ws_collect
           * \param param - path configuration file - %c. Ex. /Desktop/fire.terrama2.
           * \param err - information error.
           */
          bool runCmd(int line, QString cmd, QString param, QString& err);

          struct Impl;

          Impl* pimpl_;  //!< Pimpl idiom.

          terrama2::gui::core::ConfigManager& configManager_; //!< Configuration Manager.
          QString idNameConfig_; //!<  current identifier in config settings manager.
          bool changed_; //!< Flag indicating the data have been changed.
          terrama2::gui::admin::AdminApp* adminapp_; //!< Main Window Administration.
          terrama2::ws::collector::client::Client* client; //!< gsoap collector client.
      };
    }
  }
}

#endif // __TERRAMA2_GUI_ADMIN_SERVICESDIALOG_HPP__

