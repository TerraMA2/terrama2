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
  \file unittest/ws/collector/core/Utils.cpp

  \brief Utility functions to initialize e finalize terralib and TerraMA2 for tests.

  \author Vinicius Campanha
*/


//TerrraMA2
#include "Utils.hpp"
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/core/Utils.hpp>

//terralib
#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

// QT
#include <QTest>
#include <QJsonDocument>

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

  std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/core/data/project.json");
  QJsonDocument jdoc = terrama2::core::ReadJsonFile(path);
  QJsonObject project = jdoc.object();
  QCOMPARE(terrama2::core::ApplicationController::getInstance().loadProject(project), true);
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();
  QVERIFY(dataSource.get());
}

void finalizeTerraMA2()
{
  finalizeTerralib();

  terrama2::core::ApplicationController::getInstance().getDataSource()->close();
}
