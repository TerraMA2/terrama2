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
  \file terrama2/gui/config/ConfigApp.hpp

  \brief Main GUI for TerraMA2 Config application.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPP_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPP_HPP__

// TerraMA2
#include "ui_ConfigAppForm.h"
#include "ServiceHandler.hpp"

// Boost
#include <boost/noncopyable.hpp>

// Qt
#include <QMainWindow>

/*!
  \class ConfigApp
 
  \brief Main dialog for TerraMA2 Configuration module.
 */

 // Foward definition
// namespace Ui
// {
//   class ConfigAppForm;
// }

class ConfigAppTab;

class ConfigApp : public QMainWindow, private  boost::noncopyable
{
  Q_OBJECT

  public:
  
    /*! Default constructor.
      \exception terrama2::InitializationError <It may be raised when cannot find TerraMA2 icons library folder>

    */
    ConfigApp(QWidget* parent = 0);

    //! Destructor.
    ~ConfigApp();

    //! It retrieves the ui from pimpl_
    Ui::ConfigAppForm* ui() const;

  private slots:
    void tabChangeRequested(int);
    void disableRefreshAction();
    void openRequested();
  
  private:
    struct Impl;

    Impl* pimpl_;

    int currentTabIndex_;

    ServiceHandler* services_;

    QList<ConfigAppTab*> tabList_; //< List of TerraMA2 Configuration Tabs
};

#endif // __TERRAMA2_GUI_CONFIG_CONFIGAPP_HPP__
