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

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

#ifndef _FilterDialog_H_
#define _FilterDialog_H_

// TerraMA2
#include "ui_FilterDialogForm.h"

// boost
#include <boost/noncopyable.hpp>


// Forward
class QDialog;

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

    FilterDialog(FilterType type, QWidget* parent = 0, Qt::WindowFlags f = 0 );
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

  private slots:
    //! Slot trigerred when the user specifies if there any data to skip. Therefore, it marks filter by date neither true or false.
    void onFilteredByDate();

    //! Slot triggered whether filter by layer.
    void onFilteredByLayer();

    //! Slot triggered whether filter by area.
    void onFilteredByArea();

    void onAfterBtnClicked();

    void onBeforeBtnClicked();

  private:
    struct Impl; //!< Pimpl idiom
    Impl* pimpl_;
};


#endif

