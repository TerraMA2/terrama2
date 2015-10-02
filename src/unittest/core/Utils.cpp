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
  \file unittest/core/Utils.cpp

  \brief Utility functions to initialize e finalize terralib and TerraMA2 for tests.

  \author Paulo R. M. Oliveira
*/


//TerrraMA2
#include "Utils.hpp"
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/ApplicationController.hpp>

//terralib
#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

// QT
#include <QTest>

// STL
#include <string>


void initializeTerralib()
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
}

void finalizeTerralib()
{
  TerraLib::getInstance().finalize();
}

void initializeTerraMA2()
{
  initializeTerralib();


  // TODO: Uncomment this after implementation of batch executor
  //createDatabase();

  // TODO: Remove this after implementation of batch executor
  std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/core/data/project.json");
  QCOMPARE(terrama2::core::ApplicationController::getInstance().loadProject(path), true);
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();
  QVERIFY(dataSource.get());
}

void finalizeTerraMA2()
{
  finalizeTerralib();

  if(terrama2::core::ApplicationController::getInstance().getDataSource().get())
  {
    terrama2::core::ApplicationController::getInstance().getDataSource()->close();
  }

}


void dropDatabase()
{
  if(terrama2::core::ApplicationController::getInstance().getDataSource().get())
  {
    terrama2::core::ApplicationController::getInstance().getDataSource()->close();
  }

  std::map<std::string, std::string> connInfo;

  connInfo["PG_HOST"] = "localhost";
  connInfo["PG_PORT"] = "5432";
  connInfo["PG_USER"] = "postgres";
  connInfo["PG_DB_NAME"] = "postgres";
  connInfo["PG_CONNECT_TIMEOUT"] = "4";
  connInfo["PG_CLIENT_ENCODING"] = "UTF-8";
  connInfo["PG_DB_TO_DROP"] = "terrama2_test";

  std::string dsType = "POSTGIS";

  // Check the data source existence
  connInfo["PG_CHECK_DB_EXISTENCE"] = "terrama2_test";
  bool dsExists = te::da::DataSource::exists(dsType, connInfo);

  if(dsExists)
  {
    te::da::DataSource::drop(dsType, connInfo);
  }
}

void createDatabase()
{
  dropDatabase();

  terrama2::core::ApplicationController::getInstance().createDatabase("terrama2_test", "postgres", "postgres", "localhost", 5432);
}
