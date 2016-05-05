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
  \file terrama2/core/Utils.cpp

  \brief Utility functions for TerraMA2 SDK.

  \author Gilberto Ribeiro de Queiroz
*/

// TerraMA2
#include "Utils.hpp"
#include "DataAccessorFactory.hpp"
#include "SemanticsManager.hpp"
#include "Logger.hpp"
#include "../Exception.hpp"
#include "../../Config.hpp"



// TerraLib
#include <terralib/common/PlatformUtils.h>
#include <terralib/common/UnitsOfMeasureManager.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

#include <ctime>

// Boost
#include <boost/filesystem.hpp>

// QT
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

// Curl
#include <curl/curl.h>

namespace te
{
  namespace common
  {
    enum
    {
      UOM_second =  1040
    };
  } // end of namespace common
} // end of namespace te


std::string terrama2::core::FindInTerraMA2Path(const std::string& fileName)
{
  // 1st: look in the neighborhood of the executable
  boost::filesystem::path tma_path = boost::filesystem::current_path();

  boost::filesystem::path eval_path = tma_path / fileName;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  tma_path /= "..";

  eval_path = tma_path / fileName;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  // 2rd: look for an environment variable defined by macro TERRAMA2_DIR_VAR_NAME
  const char* tma_env = getenv(TERRAMA2_DIR_VAR_NAME);

  if(tma_env != nullptr)
  {
    tma_path = tma_env;

    eval_path = tma_path / fileName;

    if(boost::filesystem::exists(eval_path))
      return eval_path.string();
  }

  // 3th: look into install prefix-path
  tma_path = TERRAMA2_INSTALL_PREFIX_PATH;

  eval_path = tma_path / fileName;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  // 4nd: look into the codebase path
  tma_path = TERRAMA2_CODEBASE_PATH;

  eval_path = tma_path / fileName;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  return "";
}

void terrama2::core::initializeTerralib()
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

  // Base of Time measure: second
  te::common::UnitOfMeasurePtr uomSecond(new te::common::UnitOfMeasure(te::common::UOM_second,"second", "s", te::common::MeasureType::Time));
  std::vector<std::string> secondAlternativeNames {"s", "sec", "ss", "seconds"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomSecond, secondAlternativeNames);

  // minute
  te::common::UnitOfMeasurePtr uomMinute(new te::common::UnitOfMeasure(1, "minute", "min", te::common::MeasureType::Time, te::common::UOM_second, 60.0, 0.0, 0.0, 1.0));
  std::vector<std::string> minuteAlternativeNames {"min", "minutes"};

  // hour
  te::common::UnitOfMeasurePtr uomHour(new te::common::UnitOfMeasure(2, "hour", "h", te::common::MeasureType::Time, te::common::UOM_second, 3600.0, 0.0, 0.0, 1.0));
  std::vector<std::string> hourAlternativeNames {"hh", "h", "hours"};

  // day
  te::common::UnitOfMeasurePtr uomDay(new te::common::UnitOfMeasure(3, "day", "d", te::common::MeasureType::Time, te::common::UOM_second, 86400.0, 0.0, 0.0, 1.0));
  std::vector<std::string> dayAlternativeNames {"d", "dd", "days"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomMinute, minuteAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomHour, hourAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomDay, dayAlternativeNames);
}


void terrama2::core::finalizeTerralib()
{
  TerraLib::getInstance().finalize();


}

void terrama2::core::initializeTerraMA()
{
  curl_global_init(CURL_GLOBAL_ALL);
  terrama2::core::initializeTerralib();

  terrama2::core::initializeLogger("terrama2.log");


  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  semanticsManager.addSemantics("OCCURRENCE-postgis", terrama2::core::DataSeriesSemantics::OCCURRENCE, "POSTGIS");
  semanticsManager.addSemantics("OCCURRENCE-wfp", terrama2::core::DataSeriesSemantics::OCCURRENCE, "CSV");
  semanticsManager.addSemantics("STATIC_DATA-ogr", terrama2::core::DataSeriesSemantics::STATIC, "OGR");
  semanticsManager.addSemantics("DCP-inpe", terrama2::core::DataSeriesSemantics::DCP, "CSV");
  semanticsManager.addSemantics("DCP-toa5", terrama2::core::DataSeriesSemantics::DCP, "CSV");
  semanticsManager.addSemantics("DCP-postgis", terrama2::core::DataSeriesSemantics::DCP, "POSTGIS");
  semanticsManager.addSemantics("ANALYSIS_MONITORED_OBJECT-postgis", terrama2::core::DataSeriesSemantics::ANALYSIS_MONITORED_OBJECT, "POSTGIS");

}

void terrama2::core::finalizeTerraMA()
{
  terrama2::core::finalizeTerralib();
  curl_global_cleanup();
}

void terrama2::core::initializeLogger(const std::string& pathFile)
{
  terrama2::core::Logger::getInstance().addStream(pathFile);
  terrama2::core::Logger::getInstance().initialize();
}

void terrama2::core::disableLogger()
{
  terrama2::core::Logger::getInstance().disableLog();
}

void terrama2::core::enableLogger()
{
  terrama2::core::Logger::getInstance().enableLog();
}
