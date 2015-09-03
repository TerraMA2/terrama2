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
  \file terrama2/gui/admin/AdminApp.cpp

  \brief Main GUI for TerraMA2 Config application.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "AdminApp.hpp"
#include "ui_AdminAppForm.h"

// Qt
#include <QIcon>
#include <QStringList>
#include <QToolBar>

struct AdminApp::Impl
{
  Ui::AdminAppForm* ui_;
  
  Impl()
    : ui_(new Ui::AdminAppForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }
};

AdminApp::AdminApp(QWidget* parent)
  : QMainWindow(parent),
    pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);
}

AdminApp::~AdminApp()
{
  delete pimpl_;
}

void
AdminApp::init()
{
// load icon theme
  QStringList ithemes = QIcon::themeSearchPaths();
  
  ithemes.push_back("/home/terrama2/Projeto/terrama2/codebase/share/icons");
  
  QIcon::setThemeSearchPaths(ithemes);
  
  QIcon::setThemeName("terrama2");
  
}


