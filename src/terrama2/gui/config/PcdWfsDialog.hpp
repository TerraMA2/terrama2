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
  \file terrama2/gui/config/PcdWfsDialog.hpp

  \brief Class responsible for handling WFS among PCD

  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CONFIG_PCDWFSDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_PCDWFSDIALOG_HPP__

// QT
#include <QDialog>

// Boost
#include <boost/noncopyable.hpp>

// Forward declaration
class QWidget;
class QAction;

namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      /*!
        \brief Class responsible for handling WFS among PCD
      */
      class PcdWfsDialog : public QDialog, private boost::noncopyable
      {
        Q_OBJECT

        public:
          PcdWfsDialog(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
          ~PcdWfsDialog();

        private slots:
          void onOkBtnClicked();
          void onMenuMaskClicked(QAction*);

        private:
          struct Impl; //!< Pimpl idiom

          Impl* pimpl_; //!< Pimpl idiom
      };
    }
  }
}

#endif // __TERRAMA2_GUI_CONFIG_PCDWFSDIALOG_HPP__
