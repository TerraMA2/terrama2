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
  \file terrama2/gui/config/CollectorRuleDialog.hpp

  \brief It defines the CollectorRule GUI interface to enable a collector lua script filter

  \author Raphael Willian da Costa
  \author Pedro Luchini de Moraes
*/

#ifndef __TERRAMA2_GUI_CONFIG_COLLECTORRULEDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_COLLECTORRULEDIALOG_HPP__

// QT
#include <QDialog>
#include <QSharedPointer>

// boost
#include <boost/noncopyable.hpp>

// Forward declarations
namespace Ui
{
  class CollectorRuleDialogForm; //! It contains QT GUI Widgets.
}
class QAction;
namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      class LuaSyntaxHighlighter;
    }
  }
}


namespace terrama2
{
  namespace gui
  {
    namespace config
    {
      /*!
        \class CollectorRuleDialog
        \brief It defines the CollectorRule GUI interface to enable a collector lua script filter

               Contains methods with helper functions for lua script and common terrama2 functions supported
      */
      class CollectorRuleDialog : public QDialog, private boost::noncopyable
      {
        Q_OBJECT

        public:
          /*!
            \brief Constructor
            \param parent A pointer to QWidget object owner (default nullptr)
            \param f A constant window flags to define kind of window (default 0)
          */
          CollectorRuleDialog(QWidget* parent = nullptr, Qt::WindowFlags f = 0);

          //! Destructor
          ~CollectorRuleDialog();

          /*!
            \brief It fill lua script plain text of interface
            \param script: const string variable containing lua script and it fill the plain text
           */
          void fillGUI(const QString script);

          /*!
            \brief Returns the collect rule.
           */
          std::string getCollectRule();

        private:
          /*!
            \brief It initialize all lua collector help functions, such math expressions and lua blocks
          */
          void init();

        private slots:
          /*!
            \brief It is triggered when some of help scripts buttons has been clicked and fill up the plain text
           */
          void onAddScript(QAction*);

          /*!
            \brief It is triggered when the OK btn is clicked and it will checks if there any syntax error or empty value
            \todo validate lua syntax
          */
          void onOkBtnClicked();

        private:
          struct Impl;
          Impl* pimpl_; //!< pimpl idiom
          QSharedPointer<LuaSyntaxHighlighter> luaHighlighter_; //!< it contains lua syntax highlighter commands
      };
    }
  }
}

#endif // __TERRAMA2_GUI_CONFIG_COLLECTORRULEDIALOG_HPP__

