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
  \file terrama2/gui/config/ConfigAppWeatherServer.hpp

  \brief Class responsible for handling DataProvider actions inside WeatherTab

  \author Raphael Willian da Costa
*/


#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"

// Forward declaration
class QTreeWidgetItem;


namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      /*!
        \class ConfigAppWeatherServer
        \brief Class responsible for handling DataProvider actions inside WeatherTab
      */
      class ConfigAppWeatherServer : public ConfigAppTab
      {

        Q_OBJECT
        public:
          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::ConfigAppTab(app, ui);
          */
          ConfigAppWeatherServer(ConfigApp* app, Ui::ConfigAppForm* ui);

          //! Destructor
          ~ConfigAppWeatherServer();

          //! It is used for connect QTextEdit to allow datachanged
          void load();

          //! Save dataprovider in database
          void save();

          //! Discard changes and restore the widget to the initial state
          void discardChanges(bool restore);

          //! Validates the fields in server insertion
          bool validate();

        private:
          /*!
            \brief It validates the connection type and stores the URI value in uri_ member.
            \exception terrama2::gui::DirectoryException Raised when an invalid path is set
            \exception terrama2::gui::ValueException Raised because a type different from FTP, FILE and HTTP is not supported yet.
            \exception terrama2::gui::URLException Raised when a URI is invalid.
            \exception terrama2::gui::ConnectionException Raised when curl response is Timeout, Login Denied or URL Malformated.
            \todo Support OGC type, SOS type.
          */
          void validateConnection();

        private slots:
          //! Slot triggered when the user clicks in insertServerbtn to display ServerPage
          void onServerTabRequested();

          //! Slot triggered when some widget is changed.
          void onServerEdited();

          //! Slot triggered for detect if the QTextEdit is changed
          void onTextEditChanged();

          //! Slot for handling if it is valid connection. TODO: ftp
          void onCheckConnectionClicked();

          /*!
            \brief Slot triggered when connection type combobox index has been changed and it displays hidden widgets
            \param index A int value representing index of connection type combobox. It is converted to terrama2::core::DataProvider::Kind
          */
          void onConnectionTypeChanged(int index);

          /*!
            \brief Slot triggered when connection address button has been clicked. It opens FileDialog
                   to select specific folder and fill connection address out
          */
          void onAddressFileBtnClicked();

        private:
          QString uri_; //!< Value used for store parsed uri value
      };
    }
  }
}

#endif //__TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
