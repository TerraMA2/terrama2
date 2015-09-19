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
  \file terrama2/gui/config/AdminApp.hpp

  \brief Main GUI for TerraMA2 Admin application.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_ADMIN_ADMINAPP_HPP__
#define __TERRAMA2_GUI_ADMIN_ADMINAPP_HPP__

// TerraMA2
#include "ui_AdminAppForm.h";
#include "../core/ConfigManager.hpp"

// Boost
#include <boost/noncopyable.hpp>

// Qt
#include <QMainWindow>

/*!
  \class AdminApp
 
  \brief Main dialog for TerraMA2 Administration module.
 */

class AdminApp : public QMainWindow, private boost::noncopyable
{
  Q_OBJECT

  public:

//! Default constructor.
    AdminApp(QWidget* parent = 0);

//! Destructor.
    ~AdminApp();

//! Fill fields
    void fillForm();

  private slots:

    void openRequested();
  
  private:
  
    struct Impl;

    ConfigManager* configManager_;

    Impl* pimpl_;  //!< Pimpl idiom.
};

#endif // __TERRAMA2_GUI_ADMIN_ADMINAPP_HPP__
