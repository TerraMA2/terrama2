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
  \file terrama2/gui/config/ConfigAppWeatherPcd.hpp

  \brief Class responsible for handling PCD data inside WeatherTab

  \author Raphael Willian da Costa
*/


#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERPCD_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERPCD_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"

// QT
#include <QList>


// Forward declarations
class QAction;
class QTableWidgetItem;
struct PCD;

class ConfigAppWeatherPcd : public ConfigAppTab
{

  Q_OBJECT

  public:
    /*!
      @copydoc ConfigAppTab::ConfigAppTab(app, ui)
    */
    ConfigAppWeatherPcd(ConfigApp* app, Ui::ConfigAppForm* ui);

    //! Destructor
    ~ConfigAppWeatherPcd();

    //! It cleans up the fields to initial state
    void load();

    /*!
      \brief It will load the pcd values to QTableWidget
      \param dataset: terrama2::core::DataSet to fill out the table
      \todo It must be implemented following the new terrama2 models
    */
    void load(const terrama2::core::DataSet& dataset);

    /*!
      \brief It validates the GUI input values.
      \exception terrama2::gui::FieldError
      \return true if the whole interface is valid. Otherwise, it throws exception
    */
    bool validate();

    //! It applies save operation, sending the dataset to gsoap client
    void save();

    //! It discards the widget values and set to initial state
    void discardChanges(bool restore_data);

    /*!
      \brief It cleans up the pcd table widget
    */
    void tableClean();

    /*!
      \brief Sets the projection
      \param srid A Spatial Reference System Identifier to be filled up.
    */
    void setSrid(const uint64_t srid);

  private:
    /*!
      \brief Common function that displays PCD form to insert/modify elements at QTableWidget
      \param pcd A PCD reference to be filled out
      \param editing A bool value defining if which mode is (default is false).
    */
    void pcdFormCreation(PCD& pcd, bool editing = false);

  private slots:
    //! Slot triggered when insert pcd button has been clicked. It displays the PCD tab
    void onInsertPointBtnClicked();

    //! Slot triggered when remove button has been clicked. It removes dataset
    void onDataPointBtnClicked();

    //! Slot triggered when the collector rule form is requested and it displays GUI form
    void onCollectorRuleClicked();

    //! Slot triggered when the projection button has been clicked and it displays the DataSet Projection GUI
    void onProjectionClicked();

    /*!
      \brief It populates the msak field with menu mask values, like "%d %a"
      \param action A pointer to Action requested.
    */
    void onMenuMaskClicked(QAction* action);

    //! Slot triggered when the PCD plus button has been clicked and it displays the Insertion/Modification PCD GUI
    void onPCDInsertFileClicked();

    //! Slot triggered when click on remove selected pcd from table widget
    void onPCDRemoveClicked();

    /*!
      \brief Slot triggered when double clicked at pcd table widget and it displays the pcd dialog with filled fields
      \param item A pointer to requested item from list.
    */
    void onPCDTableDoubleClicked(QTableWidgetItem* item);

    //! Slot triggered to display SurfaceDialog
    void onSurfaceBtnClicked();

    //! Slot triggered at pcdexport button to export pcd in terrama2 format
    void onPCDExportClicked();

  private:
    QList<PCD> pcds_; //!< PCDs table list
    QString luaScript_; //!< It storages lua script from CollectorRuleDialog
    uint64_t srid_; //! Data projection
};

#endif // __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERPCD_HPP__
