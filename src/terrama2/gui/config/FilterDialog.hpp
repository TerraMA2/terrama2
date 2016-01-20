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

  \brief Class responsible for handling filter gui

  \author Raphael Willian da Costa  
*/

#ifndef __TERRAMA2_GUI_CONFIG_FILTERDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_FILTERDIALOG_HPP__

// TerraMA2
#include "ui_FilterDialogForm.h"

// boost
#include <boost/noncopyable.hpp>


// Forward declarations
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
        \class FilterDialog
        \brief Class responsible for handling filter gui
      */
      class FilterDialog : public QDialog, private boost::noncopyable
      {
        Q_OBJECT

        public:

          /*!
            \brief It defines which kind of filters must be visible.
          */
          enum FilterType
          {
            FULL, ///< Represents to enable all.
            AREA, ///< Represents to enable Area filter.
            BAND, ///< Represents to enable Band filter.
            DATE  ///< Represents to enable Date filter and Pre-Analyse filter.
          };

          /*!
            \brief Default constructor. It connecting slots and define how to load the interface.
            \param type A enum type for display which tabs will be visible.
            \param timezone A string timezone to be printed in the label. (default +00:00)
            \param parent A pointer to QWidget owner. (default nullptr)
            \param flag A const value to define kind of widget. (default 0)
          */
          FilterDialog(FilterType type, const QString& timezone = "+00:00", QWidget* parent = nullptr, Qt::WindowFlags flag = 0);

          //! Destructor
          ~FilterDialog();

          /*!
            \brief It checks if there any filter by date
            \return True if exists filter by date
          */
          bool isFilterByDate() const;

          /*!
            \brief It checks if there any filter by layer
            \return True if exists filter by layer
          */
          bool isFilterByLayer() const;

          /*!
            \brief It checks if there any filter by analyse before
            \return True if exists filter by pre-analyse
          */
          bool isFilterByPreAnalyse() const;

          /*!
            \brief It checks if there any filter by field
            \return True if exists filter by Area
          */
          bool isFilterByArea() const;

          /*!
            \brief It checks if there any filter active
            \return True if the user filled out some filter
          */
          bool isAnyFilter() const;

          //! It will fill up the gui fields with terrama2 filter object
          void fillGUI(const terrama2::core::Filter &filter);

          //! It will populate the filter object with filter values
          void fillObject(terrama2::core::Filter& filter);

        private:
          //! It disables pre analyses fields and set as filter by analyse
          void setFilterByPreAnalyse();

          //! It disables pre analyses fields
          void disablePreFields();

        private slots:
          //! Slot trigerred when the user specifies if there any data to skip. Therefore, it marks filter by date either true or false.
          void onFilteredByDate();

          //! Slot triggered whether filter by layer.
          void onFilteredByLayer();

          //! Slot triggered whether filter by area.
          void onFilteredByArea();

          //! Slot triggered when after btn has been clicked. It fills the after date input.
          void onAfterBtnClicked();

          //! Slot triggered when before btn has been clicked. It fills the before date input.
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

          /*!
            \brief Slot triggered when ok button has been clicked.
                   It applies validation and if there is not problem emits accept signal
          */
          void onOkBtnClicked();

        private:
          struct Impl; //!< Pimpl idiom
          Impl* pimpl_;
      };
    }
  }
}

#endif // __TERRAMA2_GUI_CONFIG_FILTERDIALOG_HPP__

