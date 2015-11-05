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


// Forward declaration
class QAction;
class QTableWidgetItem;
struct PCD;

class ConfigAppWeatherPcd : public ConfigAppTab
{
  Q_OBJECT
  public:
    ConfigAppWeatherPcd(ConfigApp* app, Ui::ConfigAppForm* ui);
    ~ConfigAppWeatherPcd();

    void load();
    bool validate();
    void save();
    void discardChanges(bool restore_data);

  private:
    void pcdFormCreation(PCD&, bool editing = false);

  private slots:
    void onInsertPointBtnClicked();
    void onDataPointBtnClicked();

    /*!
      \brief Slot triggered when the projection button has been clicked and it displays the DataSet Projection GUI
    */
    void onProjectionClicked();

    /*!
      \brief It populates the msak field with menu mask values, like "%d %a"
    */
    void onMenuMaskClicked(QAction*);

    /*!
      \brief Slot triggered when the PCD plus button has been clicked and it displays the Insertion/Modification PCD GUI
     */
    void onPCDInsertFileClicked();

    /*!
     * \brief Slot triggered when click on remove selected pcd from table widget
     */
    void onPCDRemoveClicked();

    /*!
      \brief Slot triggered when double clicked at pcd table widget and it displays the pcd dialog with filled fields
     */
    void onPCDTableDoubleClicked(QTableWidgetItem*);

    /*!
      \brief Slot triggered to display SurfaceDialog
     */
    void onSurfaceBtnClicked();

  private:
    QList<PCD> pcds_;
};

#endif // __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERPCD_HPP__
