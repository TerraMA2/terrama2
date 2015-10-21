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

// Terralib
#include "terralib/common/PlatformUtils.h"
#include "terralib/common.h"
#include "terralib/plugin.h"

// TerraMA2
#include "soapWebService.h"
#include "../../../core/ApplicationController.hpp"
#include "../../../core/DataManager.hpp"
#include "../../../core/Utils.hpp"


int main(int argc, char* argv[])
{

  // check if the parameters was passed correctly
    if(argc < 3)
    {
      std::cerr << "Inform a port and a project file in order to run the collector application server." << std::endl;
      std::cerr << "Usage: terrama2_mod_ws_collector_appserver <port> <project_File>" << std::endl;

      return EXIT_FAILURE;
    }

  std::cerr << "Starting Webservice..." << std::endl;

  std::cerr << "Initializating TerraLib..." << std::endl;

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

  std::cerr << "Loading TerraMA2 Project..." << std::endl;

  if(!terrama2::core::ApplicationController::getInstance().loadProject(argv[2]))
  {
    std::cerr << "TerraMA2 Project File is invalid or don't exist!" << std::endl;
    return EXIT_FAILURE;
  }

  terrama2::core::DataManager::getInstance().load();

  WebService server;

  if(soap_valid_socket(server.master) || soap_valid_socket(server.bind(NULL, std::stoi(argv[1]), 100)))
  {
    std::cerr << "Webservice Started, running on port " << argv[1] << std::endl;

    for (;;)
    {
      if (!soap_valid_socket(server.accept()))
        break;

      server.serve();
      server.destroy();
    }
  }

  server.soap_stream_fault(std::cerr);

  std::cerr << "Closing Webservice..." << std::endl;
  TerraLib::getInstance().finalize();

  terrama2::core::DataManager::getInstance().unload();

  terrama2::core::ApplicationController::getInstance().getDataSource()->close();

  std::cerr << "Webservice finished!" << std::endl;

  return EXIT_SUCCESS;
}
