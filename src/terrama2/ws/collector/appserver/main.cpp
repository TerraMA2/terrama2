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
#include "../../../core/Utils.hpp"
#include "../../../core/ApplicationController.hpp"


int main(int argc, char* argv[])
{

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

  std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/core/data/project.json");

  if(!terrama2::core::ApplicationController::getInstance().loadProject(path))
    return EXIT_FAILURE;

  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  if(!dataSource.get())
    return EXIT_FAILURE;

// check if a port number was passed as parameter
  if(argv[1] == 0)
  {
    std::cerr << "Inform a port in order to run the collector application server.";

    return EXIT_FAILURE;
  }

  WebService server;

// run iterative server on port until fatal error
  if( server.run( std::stoi(argv[1]) ) )
  {
    server.soap_stream_fault(std::cerr);

    TerraLib::getInstance().finalize();

    terrama2::core::ApplicationController::getInstance().getDataSource()->close();

    return EXIT_FAILURE;
  }

  TerraLib::getInstance().finalize();

  terrama2::core::ApplicationController::getInstance().getDataSource()->close();

  return EXIT_SUCCESS;
}
