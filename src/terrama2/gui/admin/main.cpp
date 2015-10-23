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
  \file terrama2/gui/admin/main.cpp

  \brief Main routine for TerraMA2 Admin GUI.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "AdminApp.hpp"
#include "../Exception.hpp"

// TerraLib
#include <terralib/common/TerraLib.h>
#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

// Qt
#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[])
{
// initialize Qt
  QApplication app(argc, argv);

// initialize TerraLib
  TerraLib::getInstance().initialize();

  te::plugin::PluginInfo* info;
  std::string plugins_path = te::common::FindInTerraLibPath("share/terralib/plugins");
  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.pgis.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.gdal.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.ogr.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  te::plugin::PluginManager::getInstance().loadAll();

  try
  {
    AdminApp terrama2_admin;
    terrama2_admin.show();

    int retval = app.exec();

// finalize TerraLib
    TerraLib::getInstance().finalize();
  
    return retval;
  }
  catch(const terrama2::Exception& e)
  {
    QString messageError = "TerraMA2 finished with errors!\n\n";

    if (const QString* d = boost::get_error_info<terrama2::ErrorDescription>(e))
    {
      messageError.append(d);
    }

     QMessageBox::critical(nullptr, "TerraMA2", messageError);
   }

  catch(const std::exception& e)
  {      
    QString messageError = "TerraMA2 finished with erros!\n\n%1";
    messageError =  messageError.arg(e.what());
    QMessageBox::critical(nullptr, "TerraMA2", messageError);
  }

  catch(...)
  {
    QMessageBox::critical(nullptr, "TerraMA2", "Unknown Error");
  }

// finalize TerraLib
  TerraLib::getInstance().finalize();
  return EXIT_FAILURE;
}
