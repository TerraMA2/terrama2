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
  \file terrama2/gui/config/ConfigAppWeatherOccurrence.hpp

  \brief Class responsible for handling Occurrence data set inside WeatherTab

  \author Raphael Willian da Costa
*/


#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHEROCCURRENCE_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHEROCCURRENCE_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"
#include "../../core/Intersection.hpp"

// QT
#include <QSharedPointer>

// Forward Declaration
namespace terrama2
{
  namespace core
  {
    class Filter;
  }
}

class ConfigAppWeatherOccurrence : public ConfigAppTab
{
  Q_OBJECT
  public:
    ConfigAppWeatherOccurrence(ConfigApp* app, Ui::ConfigAppForm* ui);
    ~ConfigAppWeatherOccurrence();

    void load();

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

    //! It fills the terrama2 filter object
    void fillFilter(const terrama2::core::Filter&);

    //! Sets the intersection object
    void setIntersection(const terrama2::core::Intersection& intersection);

    //! Sets the projection
    void setSrid(const uint64_t srid);

  private:
    //! It reset filter label state
    void resetFilterState();

  public slots:
    void onFilterClicked();
    void onProjectionClicked();

  private slots:
    //! Slot triggered when insert occurrence data button has been clicked. It displays the Occurrence Tab.
    void onDataSetBtnClicked();

    //! Slot triggered when remove occurrence data button has been clicked. It removes selected dataset from database
    void onRemoveOccurrenceBtnClicked();

    /*!
      \brief Slot triggered when intersection button has been clicked. It displays the Intersection Gui Form.
    */
    void onIntersectionBtnClicked();

  private:
    QSharedPointer<terrama2::core::Filter> filter_; //!< Occurrence filter defined
    terrama2::core::Intersection intersection_; //! Intersection configuration
    uint64_t srid_; //! Data projection

};

#endif // __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHEROCCURRENCE_HPP__
