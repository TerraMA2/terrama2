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
  \file terrama2/gui/config/ConfigAppWeatherTab.hpp

  \brief Class responsible for handling WeatherTab actions

  \author Raphael Willian da Costa
*/


#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERTAB_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERTAB_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"

// QT
#include <QString>
#include <QList>
#include <QMap>
#include <QSharedPointer>


// Forward declarations
class QTreeWidgetItem;

namespace terrama2
{
  namespace core
  {
    class DataProvider;
    class DataSet;
  }

  namespace gui
  {
    namespace config
    {
      class ConfigAppWeatherServer;
      class ConfigAppWeatherGridTab;
    }
  }
}

namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      /*!
        \class ConfigAppWeatherTab
        \brief Class responsible for handling WeatherTab actions
      */
      class ConfigAppWeatherTab : public ConfigAppTab
      {

        Q_OBJECT

        public:
          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::ConfigAppTab(app, ui)
          */
          ConfigAppWeatherTab(ConfigApp* app, Ui::ConfigAppForm* ui);

          //! Destructor
          ~ConfigAppWeatherTab();

          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::load()

            It loads each subtab
          */
          void load();

          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::dataChanged()

            It detects if the there any data changed at form
          */
          bool dataChanged();

          /*!
            @copydoc terrama2::gui::config::ConfigAppTab::validate()

            Validate each widget filled out
          */
          bool validate();

          /*!
            @copybrief terrama2::gui::config::ConfigAppTab::save()

            It calls each one of subTabs_ and check which is active and is data changed and then try to save.

            \exception terrama2::gui::FieldException Raised when in tab widget validate method is false.
          */
          void save();

          //! Discard changes of form and clean all
          void discardChanges(bool restore_data);

          //! It remove all children from QWidgetTree
          void clearList();

          /*!
            \brief Set visible of operations buttons
            \param state A bool state to hide or to show ok and cancel buttons
          */
          void displayOperationButtons(bool state);

          /*!
            \brief It handles when the subtab can change
            \param sender A ConfigAppTab pointer representing who requested for change
            \param widget A QWidget instance representing the object to be set
          */
          void changeTab(ConfigAppTab* sender, QWidget& widget);

          /*!
            \brief Retrieve cached map of providers
            \return A copy of QMap<std::string,terrama2::core::DataProvider>
          */
          QMap<std::string,terrama2::core::DataProvider> providers();

          /*!
            \brief It retrieves dataprovider from cached list
            \param identifier A string to be searched in terrama2 providers list
            \return A copy of terrama2::core::DataProvider object found
          */
          terrama2::core::DataProvider getProvider(const std::string& identifier);

          /*!
            \brief It retrieves dataset from dataprovider list
            \param identifier A string to be searched in dataset list
            \return A copy of terrama2::core::DataSet object found
          */
          terrama2::core::DataSet getDataSet(const std::string& identifier);

          /*!
            \brief It is used for insert and update cached dataprovider list
            \param provider A terrama2::core::DataProvider reference to be saved in cache list
          */
          void addCachedProvider(const terrama2::core::DataProvider& provider);

          /*!
            \brief Remove provider from cached map
            \param provider A terrama2::core::DataProvider reference to be searched in cached list and to be removed
          */
          void removeCachedDataProvider(const terrama2::core::DataProvider& provider);

          /*!
            \brief Add a dataset in cached map of datasets
            \param dataset A terrama2::core::DataSet reference to be saved in cache list
          */
          void addCachedDataSet(const terrama2::core::DataSet& dataset);

          /*!
            \brief Remove a dataset from cached dataset map
            \param dataset A terrama2::core::DataSet reference to be searched in cached list and to be removed
          */
          void removeCachedDataSet(const terrama2::core::DataSet& dataset);

          /*!
            \brief It refresh the weatherdatalist from widget and string for replace
            \param widget A pointer to QTreeWidgetItem defining where is to start.
            \param searchFor A QString value to be searched in list
            \param replace A QString value to be replaced if found it
          */
          void refreshList(QTreeWidgetItem* widget, QString searchFor, QString replace);

          /*!
            \brief It handles visibility of dataseries buttons.
            \param state A bool state to set visible of dataseries buttons
          */
          void showDataSeries(bool state);

        private:
          //! It hides each one data series button
          void hideDataSetButtons();

          /*!
            \brief It hides set of widgets except the passed argument
            \param except A pointer to QWidget to skip
          */
          void hidePanels(QWidget* except);

        private slots:
          //! Slot for handling importServer button
          void onImportServer();

          //! Triggered when click serverDeleteBtn to remove data provider
          void onDeleteServerClicked();

          /*!
            \brief Triggered when click in weatherTree to enable data series button
            \param selectedItem A pointer to QTreeWidgetItem representing the current item selected.
          */
          void onWeatherDataTreeClicked(QTreeWidgetItem* selectedItem);

          //! Triggered when click exportServerBtn to export DataProvider as TerraMA2 File
          void onExportServerClicked();

          /*!
            \brief It tries to update an data provider from terrama2 configuration file
            \exception terrama2::gui::ValueException Raised when there is not data provider selected
          */
          void onUpdateServerClicked();

        private:
          QList<QSharedPointer<ConfigAppTab>> subTabs_; //!< Defines subtabs for data grid, tiff, and servers
          QMap<std::string,terrama2::core::DataProvider> providers_; //!< Cached providers map
          QMap<std::string,terrama2::core::DataSet> datasets_; //!< Cached datasets map
      };
    }
  }
}

#endif
