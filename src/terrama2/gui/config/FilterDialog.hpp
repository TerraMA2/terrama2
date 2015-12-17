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
  \file terrama2/gui/config/FilterDialog.hpp

  \brief Definition of Class FilterDialog.hpp

  \author Raphael Willian da Costa  
*/

#ifndef __TERRAMA2_GUI_CONFIG_FILTERDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_FILTERDIALOG_HPP__

// TerraMA2
#include "ui_FilterDialogForm.h"

// boost
#include <boost/noncopyable.hpp>


// Forward
namespace terrama2
{
  namespace core
  {
    class Filter;
  }
}

class FilterDialog : public QDialog, private boost::noncopyable
{
  Q_OBJECT

  public:

    enum FilterType
    {
      FULL,
      AREA,
      BAND,
      DATE
    };

    FilterDialog(FilterType type, const QString& timezone = "+00:00", QWidget* parent = nullptr, Qt::WindowFlags = 0);
    ~FilterDialog();

    //! It checks if there any filter by date
    bool isFilterByDate() const;

    //! It checks if there any filter by layer
    bool isFilterByLayer() const;

    //! It checks if there any filter by analyse before
    bool isFilterByPreAnalyse() const;

    //! It checks if there any filter by field
    bool isFilterByArea() const;

    //! It checks if there any filter active
    bool isAnyFilter() const;

    //! It will fill up the gui fields with terrama2 filter object
    void fillGUI(const terrama2::core::Filter &filter);

    //! It will populate the filter object with filter values
    void fillObject(terrama2::core::Filter& filter);

  private:
    void setFilterByPreAnalyse();
    void disablePreFields();

  private slots:
    //! Slot trigerred when the user specifies if there any data to skip. Therefore, it marks filter by date either true or false.
    void onFilteredByDate();

    //! Slot triggered whether filter by layer.
    void onFilteredByLayer();

    //! Slot triggered whether filter by area.
    void onFilteredByArea();

    void onAfterBtnClicked();

    void onBeforeBtnClicked();

    //! Slot triggered when there is no pre analyse filter
    void onNoPreAnalyse();

    //! Slot triggered when there filter by less than values
    void onFilterByLessThan();

    //! Slot triggered when there filter by greater than values
    void onFilterByGreaterThan();

    //! Slot triggered when there filter by mean less than values
    void onFilterByMeanLessThan();

    //! Slot triggered when there filter by mean greater than values
    void onFilterByMeanGreaterThan();

    void onOkBtnClicked();

  private:
    struct Impl; //!< Pimpl idiom
    Impl* pimpl_;
};


#endif // __TERRAMA2_GUI_CONFIG_FILTERDIALOG_HPP__

