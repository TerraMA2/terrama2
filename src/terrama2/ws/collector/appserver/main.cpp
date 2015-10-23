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
  \file terrama2/ws/collector/appserver/main.cpp

  \brief Main routine for TerraMA2 Collector Web Service.

  \author Vinicius Campanha
 */

// STL
#include <iostream>

// Qt
#include <QDebug>

// Terralib
#include "terralib/common/PlatformUtils.h"
#include "terralib/common.h"
#include "terralib/plugin.h"

// TerraMA2
#include "soapWebService.h"
#include "../core/Codes.hpp"
#include "../../../core/ApplicationController.hpp"
#include "../../../core/DataManager.hpp"
#include "../../../core/Utils.hpp"


int main(int argc, char* argv[])
{
  // VINICIUS: get the port number from the project file
  // check if the parameters was passed correctly

  if(argc < 3)
  {
    std::cerr << "Inform a valid port and a project file in order to run the collector application server." << std::endl;
    std::cerr << "Usage: terrama2_mod_ws_collector_appserver <port> <project_File>" << std::endl;

    return EXIT_FAILURE;
  }

  if(std::stoi(argv[1]) == 0)
  {
    std::cerr << "Inform a valid port (not " << argv[1] <<") and a project file in order to run the collector application server." << std::endl;
    std::cerr << "Usage: terrama2_mod_ws_collector_appserver <port> <project_File>" << std::endl;

    return EXIT_FAILURE;
  }

  qDebug() << "Starting Webservice...";

  qDebug() << "Initializating TerraLib...";

  // Initialize the Terralib support
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

  qDebug() << "Loading TerraMA2 Project...";

  if(!terrama2::core::ApplicationController::getInstance().loadProject(argv[2]))
  {
    qDebug() << "TerraMA2 Project File is invalid or don't exist!";
    return EXIT_FAILURE;
  }

  terrama2::core::DataManager::getInstance().load();

  WebService server;

  if(soap_valid_socket(server.master) || soap_valid_socket(server.bind(NULL, std::stoi(argv[1]), 100)))
  {
    qDebug() << "Webservice Started, running on port " << argv[1];

    for (;;)
    {
      if (!soap_valid_socket(server.accept()))
        break;

      int server_response = server.serve();

      if(server_response == terrama2::ws::collector::core::EXIT_REQUESTED)
        break;

      if(server_response != SOAP_OK)
      {
        server.soap_stream_fault(std::cerr);
      }

      server.destroy();
    }
  }

  qDebug() << "Shutdown Webservice...";

  server.destroy();
  TerraLib::getInstance().finalize();

  terrama2::core::DataManager::getInstance().unload();

  terrama2::core::ApplicationController::getInstance().getDataSource()->close();

  qDebug() << "Webservice finished!";

  return EXIT_SUCCESS;
}
