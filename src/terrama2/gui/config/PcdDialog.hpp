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
  \file terrama2/gui/config/PcdDialog.hpp

  \brief Class responsible to handle the PCD insertion/modification file

  \author Raphael Willian da Costa
*/


#ifndef __TERRAMA2_GUI_CONFIG_PCDDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_PCDDIALOG_HPP__

// Qt
#include <QDialog>

// Boost
#include <boost/noncopyable.hpp>


// Forward declarations
namespace Ui
{
  class PcdDialogForm;
}


namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      /*!
        \brief struct for stores the pcd meta
      */
      struct PCD
      {
        QString file; //!< Represents the pcd file
        QString latitude; //!< Represents latitude of pcd
        QString longitude; //!< Represents longitude of pcd
        bool active; //!< Represents pcd state
        uint64_t srid; //!< Represents Coordinate Reference System value
        QString timezone; //!< Represents PCD timezone
      };

      /*!
        \class PcdDialog
        \brief Class responsible to handle the pcd insertion/modification file
      */
      class PcdDialog : public QDialog, private boost::noncopyable
      {
        Q_OBJECT

        public:
          /*!
            \brief Default constructor. It prepare the interface, connecting slots in gui widgets
            \param parent A pointer to QWidget owner (default nullptr)
            \param f A constant value to define widget kind (default 0)
          */
          PcdDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

          //! Destructor
          ~PcdDialog();

          /*!
            \brief It fill the interface from a terrama2::gui::config::PCD object
            \param pcd A const PCD reference to fill out the interface
          */
          void fill(const PCD& pcd);

          /*!
            \brief It fill a terrama2::gui::config::PCD object from interface values
            \param pcd A PCD reference to be filled out
          */
          void fillObject(PCD& pcd);

        private slots:
          //! Slot triggered when some input widget has been changed. It enables the confirm button
          void onPcdChanged();

          /*!
            \brief Slot triggered when ok button has been clicked.
                   It validates the fields and emit signals to close window: accept and reject.
          */
          void onConfirmClicked();

          /*!
            \brief Slot triggered when projection button has been clicked.
                   It calls the terralib SRS dialog to srid be selected.
           */
          void onProjectionClicked();

        private:
          struct Impl; //!< Pimpl idiom
          Impl* pimpl_;
      };
    }
  }
}


#endif // __TERRAMA2_GUI_CONFIG_PCDDIALOG_HPP__

