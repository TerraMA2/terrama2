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
  \file terrama2/gui/config/IntersectionDialog.hpp

  \brief It defines the Intersection Form Dialog for Occurrence datasets

  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CONFIG_INTERSECTIONDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_INTERSECTIONDIALOG_HPP__

// STL
#include <memory>
#include "../../core/Intersection.hpp"


// Forward declaration
struct Database;

namespace terrama2
{
  namespace core
  {
    class Intersection;
  }
}

namespace te
{
  namespace da
  {
    class DataSetType;
    class DataSourceTransactor;
  }
}

namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      /*
        \class IntersectionDialog

        \brief Dialog to configure the intersection.
       */
      class IntersectionDialog : public QDialog
      {
        Q_OBJECT

        public:
          /*!
            \brief Constructor
            \param intersection Intersection information from the dataset.
            \param database Database connection parameters.
            \param parent Parent widget.
            \param f Window flags.
          */
          IntersectionDialog(const terrama2::core::Intersection& intersection, Database* database, QWidget* parent = 0, Qt::WindowFlags f = 0);

          //! Destructor.
          ~IntersectionDialog();

          /*!
            \brief Fills the attribute table with the columns from the given dataset type.
            \param dataset Dataset type from the selected data.
          */
          void fillAttributeTable(std::auto_ptr<te::da::DataSetType> dataset);

          //! Fills the vectorial list with all vectorial data stored in the database, it doesn't list the collected data.
          void fillVectorialList();

          //! Fills the list of grid with the datasets of the type GRID.
          void fillRasterList();

          //! Returns the intersection information filled in this dialog.
          terrama2::core::Intersection getIntersection() const;

        private slots:
          //! Slot called when OK button is pressed.
          void onOkBtnClicked();

          //! Slot called when a vectorial dataset is selected, it fills the list of columns.
          void onDatasetSelected();

          //! Slot called when a grid dataset is selected.
          void onRasterSelected(int row, int column);

          //! Slot called when an attribute is selected.
          void onAttributeSelected(int row, int column);

          //! Slot called to show the vectorial page.
          void showVectorialPage();

          //! Slot called to show the grid page.
          void showGridPage();

        private:
          struct Impl;
          Impl* pimpl_; //!< Pimpl idiom.

      };
    } // end namespace config
  }   // end namespace gui
}     // end namespace terrama2

#endif // __TERRAMA2_GUI_CONFIG_INTERSECTIONDIALOG_HPP__

