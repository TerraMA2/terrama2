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
  \file terrama2/gui/config/ConfigAppWeatherGrid.hpp

  \brief Class responsible for handling DataGrid inside WeatherTab

  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERGRID_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERGRID_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"

// STL
#include <utility>


// Forward declarations
class QAction;
namespace terrama2
{
  namespace core
  {
    class Filter;
  }
}

namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      /*!
        \class ConfigAppWeatherGridTab
        \brief Class responsible for handling DataGrid inside WeatherTab
      */
      class ConfigAppWeatherGridTab : public ConfigAppTab
      {

        Q_OBJECT

        public:

          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::ConfigAppTab(app, ui)
          */
          ConfigAppWeatherGridTab(ConfigApp* app, Ui::ConfigAppForm* ui);

          //! Destructor
          ~ConfigAppWeatherGridTab();

          //! It initializes the interface, mask button values and connect to onMenuMaskClicked
          void load();

          /*!
            \brief It validates each one gui fields
            \exception terrama2::gui::FieldError Raised when the dataset name already exist or dataset name is empty
            \exception terrama2::gui::ValueError Raised when mask value is invalid
           */
          bool validate();

          //! It applies save operation, sending the dataset to gsoap client
          void save();

          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::discardChanges(restore_data)

            Set it to initial state
          */
          void discardChanges(bool restore_data);

          /*!
            \brief It fills the terrama2 filter object
            \param filter A reference of terrama2::core::Filter
          */
          void fillFilter(const terrama2::core::Filter& filter);

          //! Sets the projection
          void setSrid(const uint64_t srid);

        public slots:
          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::onFilterClicked()
          */
          void onFilterClicked();

          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::onProjectionClicked()
          */
          void onProjectionClicked();

        private slots:
          //! Slot triggered when data grid btn is clicked to display the Grid Widget
          void onDataGridClicked();

          //! Slot triggered when combobox format has been changed. It displays an hidden widget containing format values
          void onGridFormatChanged();

          //! Slot triggered when remove button has been clicked. It removes dataset
          void onRemoveDataGridBtnClicked();

          //! Slot triggered when mask button has been clicked. It fills out the mask input from mask menu
          void onMenuMaskClicked(QAction* action);

        private:
          uint64_t srid_; //!< Projection
          terrama2::core::Filter* filter_; //!< Object used to store dataset grid filter
      };
    }
  }
}

#endif //__TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERGRID_HPP__
