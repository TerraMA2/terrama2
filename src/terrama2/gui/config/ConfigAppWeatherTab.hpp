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


class QTreeWidgetItem;
class ConfigAppWeatherServer;
class ConfigAppWeatherGridTab;

namespace terrama2
{
  namespace core
  {
    class DataProvider;
    class DataSet;
  }
}


class ConfigAppWeatherTab : public ConfigAppTab
{
  Q_OBJECT

  public:
    ConfigAppWeatherTab(ConfigApp* app, Ui::ConfigAppForm* ui);
    ~ConfigAppWeatherTab();

    //! Implemented from ConfigAppTab. It is loading each subtab
    void load();

    //! Implemented from ConfigAppTab. It detects if the there any data changed at form
    bool dataChanged();

    //! Implemented from ConfigAppTab. Validate each widget filled
    bool validate();

    //! Implemented from ConfigAppTab.
    void save();

    //! Discard changes of form and clean all
    void discardChanges(bool restore_data);

    //! It remove all children from QWidgetTree
    void clearList();

    //! Set visible of operations buttons
    void displayOperationButtons(bool state);

    //! It handles when the subtab can change
    void changeTab(ConfigAppTab &sender, QWidget &widget);

    //! Retrieve cached map of providers
    QMap<std::string,terrama2::core::DataProvider> providers();

    //! It retrieves dataprovider from cached list
    terrama2::core::DataProvider getProvider(const std::string& identifier);

    //! It retrieves dataset from dataprovider list
    terrama2::core::DataSet getDataSet(const std::string& identifier);

    //! It is used for insert and update cached dataprovider list
    void addCachedProvider(const terrama2::core::DataProvider& provider);

    //! Remove provider from cached map
    void removeCachedDataProvider(const terrama2::core::DataProvider& provider);

    //! Add a dataset in cached map of datasets
    void addCachedDataSet(const terrama2::core::DataSet& dataset);

    //! Remove a dataset from cached dataset map
    void removeCachedDataSet(const terrama2::core::DataSet& dataset);

    //! It refresh the weatherdatalist from widget and string for replace
    void refreshList(QTreeWidgetItem* widget, QString searchFor, QString replace);

  private:
    void showDataSeries(bool state);
    void hideDataSetButtons();
    void hidePanels(QWidget* except);

  private slots:
    //! Slot for handling importServer btn
    void onImportServer();

    //! Triggered when click serverDeleteBtn to remove data provider
    void onDeleteServerClicked();

    //! Triggered when click in weatherTree to enable data series button
    void onWeatherDataTreeClicked(QTreeWidgetItem*);

    //! Triggered when click exportServerBtn to export DataProvider as TerraMA2 File
    void onExportServerClicked();

  private:
    QList<QSharedPointer<ConfigAppTab>> subTabs_; //!< Defines subtabs for data grid, tiff, and servers
    QMap<std::string,terrama2::core::DataProvider> providers_; //!< Cached providers map
    QMap<std::string,terrama2::core::DataSet> datasets_; //!< Cached datasets map
};

#endif
