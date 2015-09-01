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
  \file terrama2/gui/config/IntersectionDialog.cpp

  \brief Definition of Class IntersectionDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

#include <qapplication.h>
#include <QIcon>
#include <QStringList>

#include "MainDialog.hpp"
// #include "Language.h"

int main( int argc, char** argv )
{
  // Load TerraMA2 icons and theme
  QStringList ithemes = QIcon::themeSearchPaths();

  ithemes.push_back("/home/raphael/Documents/my-devel/terrama2/share/icons/");
  QIcon::setThemeSearchPaths(ithemes);
  QIcon::setThemeName("terrama2");

  QApplication app( argc, argv );

  // Load the translators and system language 
  // loadLanguage("config");

  // Trick for avoid Mandriva problem
  // TerraLib uses sprintf to generate some SQL inserts
  // It depends of system idiom. It has been used a comma in float numbers and it turns out a
  // problem at runtime insert
#ifndef WIN32
  setlocale(LC_NUMERIC,"en_US.UTF-8");
#endif

  // Open main window
  MainDialog mainwindow;

  mainwindow.show();
  return app.exec();
}

