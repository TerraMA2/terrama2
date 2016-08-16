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
#include <terralib/core/utils/Platform.h>
#include <terralib/common/PlatformUtils.h>
#include <terralib/common/UnitsOfMeasureManager.h>
#include <terralib/common.h>
#include <terralib/plugin.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/sa/core/Utils.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>

#include <ctime>
#include <unordered_map>

// Boost
#include <boost/filesystem.hpp>

// QT
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

// Curl
#include <curl/curl.h>

namespace te
{
  namespace common
  {
    enum
    {
      UOM_second = 1040
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

  // 2nd: look for an environment variable defined by macro TERRAMA2_DIR_VAR_NAME
  const char* tma_env = getenv(TERRAMA2_DIR_VAR_NAME.c_str());

  if(tma_env != nullptr)
  {
    tma_path = tma_env;

    eval_path = tma_path / fileName;

    if(boost::filesystem::exists(eval_path))
      return eval_path.string();
  }

  // 3rd: look into install prefix-path
  tma_path = TERRAMA2_INSTALL_PREFIX_PATH;

  eval_path = tma_path / fileName;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  // 4th: look into the codebase path
  tma_path = TERRAMA2_CODEBASE_PATH;

  eval_path = tma_path / fileName;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  return "";
}

void terrama2::core::initializeTerralib()
{
  // Initialize the Terralib support
  auto& terralib = TerraLib::getInstance();
  terralib.initialize();

  te::plugin::PluginInfo* info;
  std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");
  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.pgis.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.gdal.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.ogr.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  te::plugin::PluginManager::getInstance().loadAll();




  // Base of Time measure: second
  te::common::UnitOfMeasurePtr uomSecond(new te::common::UnitOfMeasure(te::common::UOM_second,"SECOND", "s", te::common::MeasureType::Time));
  std::vector<std::string> secondAlternativeNames {"s", "sec", "ss", "seconds"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomSecond, secondAlternativeNames);

  // minute
  te::common::UnitOfMeasurePtr uomMinute(new te::common::UnitOfMeasure(1, "MINUTE", "min", te::common::MeasureType::Time, te::common::UOM_second, 60.0, 0.0, 0.0, 1.0));
  std::vector<std::string> minuteAlternativeNames {"min", "minutes"};

  // hour
  te::common::UnitOfMeasurePtr uomHour(new te::common::UnitOfMeasure(2, "HOUR", "h", te::common::MeasureType::Time, te::common::UOM_second, 3600.0, 0.0, 0.0, 1.0));
  std::vector<std::string> hourAlternativeNames {"hh", "h", "hours"};

  // day
  te::common::UnitOfMeasurePtr uomDay(new te::common::UnitOfMeasure(3, "DAY", "d", te::common::MeasureType::Time, te::common::UOM_second, 86400.0, 0.0, 0.0, 1.0));
  std::vector<std::string> dayAlternativeNames {"d", "dd", "days"};

  // week
  te::common::UnitOfMeasurePtr uomWeek(new te::common::UnitOfMeasure(4, "WEEK", "w", te::common::MeasureType::Time, te::common::UOM_second, 604800.0, 0.0, 0.0, 1.0));
  std::vector<std::string> weekAlternativeNames {"w", "wk", "weeks"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomMinute, minuteAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomHour, hourAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomDay, dayAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomWeek, weekAlternativeNames);


  // Base of distance measure: meters
  te::common::UnitOfMeasurePtr uomMeter(new te::common::UnitOfMeasure(te::common::UOM_Metre, "METER", "m", te::common::MeasureType::Length));
  std::vector<std::string> meterSecondAlternativeNames {"m", "meter", "meters"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomMeter, meterSecondAlternativeNames);

  // kilometer
  te::common::UnitOfMeasurePtr uomKm(new te::common::UnitOfMeasure(5, "KILOMETER", "km", te::common::MeasureType::Length, te::common::UOM_Metre, 1000.0, 0.0, 0.0, 1.0));
  std::vector<std::string> kmAlternativeNames {"km", "kilometers"};
  // centimeter
  te::common::UnitOfMeasurePtr uomCm(new te::common::UnitOfMeasure(6, "CENTIMETER", "cm", te::common::MeasureType::Length, te::common::UOM_Metre, 0.01, 0.0, 0.0, 1.0));
  std::vector<std::string> cmAlternativeNames {"cm", "centimeters"};


  te::common::UnitsOfMeasureManager::getInstance().insert(uomKm, kmAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomCm, cmAlternativeNames);


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

  //read semantics from json file
  std::string semanticsPath = FindInTerraMA2Path("share/terrama2/semantics.json");
  QFile semantcisFile(QString::fromStdString(semanticsPath));
  semantcisFile.open(QFile::ReadOnly);
  QByteArray bytearray = semantcisFile.readAll();
  QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray);
  auto array = jsonDoc.array();
  for(const auto& json : array)
  {
    auto obj = json.toObject();
    auto jsonProvidersTypes = obj["providers_type_list"].toArray();
    std::vector<DataProviderType> providersTypes;
    for(const auto& providerType : jsonProvidersTypes)
      providersTypes.push_back(providerType.toString().toStdString());

    auto jsonMetadata = obj["metadata"].toObject();
    std::unordered_map<std::string, std::string> metadata;
    for(auto it = jsonMetadata.constBegin(); it != jsonMetadata.constEnd(); ++it)
      metadata.emplace(it.key().toStdString(), it.value().toString().toStdString()) ;

    semanticsManager.addSemantics(obj["code"].toString().toStdString(),
                                  obj["name"].toString().toStdString(),
                                  dataSeriesTypeFromString(obj["type"].toString().toStdString()),
                                  obj["format"].toString().toStdString(),
                                  providersTypes,
                                  metadata);
  }
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

int terrama2::core::getUTMSrid(te::gm::Geometry* geom)
{
  te::gm::Coord2D coord = te::sa::GetCentroidCoord(geom);

  // Calculates the UTM zone for the given coordinate
  int zoneNumber = floor((coord.getX() + 180)/6) + 1;

  if(coord.getY() >= 56.0 && coord.getY() < 64.0 && coord.getX() >= 3.0 && coord.getX() < 12.0)
    zoneNumber = 32;

  // Special zones for Svalbard
  if(coord.getY() >= 72.0 && coord.getY() < 84.0)
  {
    if(coord.getX() >= 0.0  && coord.getX() <  9.0)
      zoneNumber = 31;
    else if(coord.getX() >= 9.0  && coord.getX() < 21.0)
      zoneNumber = 33;
    else if(coord.getX() >= 21.0 && coord.getX() < 33.0)
      zoneNumber = 35;
    else if(coord.getX() >= 33.0 && coord.getX() < 42.0)
      zoneNumber = 37;
  }

  // Creates a Proj4 description and returns the SRID.
  std::string p4txt = "+proj=utm +zone=" + std::to_string(zoneNumber) + " +datum=WGS84 +units=m +no_defs ";

  try
  {
    auto srsPair = te::srs::SpatialReferenceSystemManager::getInstance().getIdFromP4Txt(p4txt);
    return srsPair.second;
  }
  catch(std::exception& e)
  {
    QString msg(QObject::tr("Could not determine the SRID for a UTM projection"));
    throw InvalidSRIDException() << terrama2::ErrorDescription(msg);
  }
}

double terrama2::core::convertDistanceUnit(double distance, const std::string& fromUnit, const std::string& targetUnit)
{
  auto it = te::common::UnitsOfMeasureManager::getInstance().begin();

  if(it == te::common::UnitsOfMeasureManager::getInstance().end())
  {
    QString msg(QObject::tr("There is no UnitOfMeasure registered."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InitializationException() << terrama2::ErrorDescription(msg);
  }

  return te::common::UnitsOfMeasureManager::getInstance().getConversion(fromUnit, targetUnit) * distance;

}

terrama2::core::DataSeriesType terrama2::core::dataSeriesTypeFromString(const std::string& type)
{
  if(type == "DCP")
    return terrama2::core::DataSeriesType::DCP;
  else if(type == "OCCURRENCE")
    return terrama2::core::DataSeriesType::OCCURRENCE;
  else if(type == "GRID")
    return terrama2::core::DataSeriesType::GRID;
  else if(type == "MONITORED_OBJECT")
    return terrama2::core::DataSeriesType::MONITORED_OBJECT;
  else if(type == "STATIC_DATA")
    return terrama2::core::DataSeriesType::STATIC;
  else if(type == "ANALYSIS_MONITORED_OBJECT")
    return terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT;
  else
  {
    QString errMsg = QObject::tr("Unknown DataSeriesType: %1.").arg(QString::fromStdString(type));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataModelException() << terrama2::ErrorDescription(errMsg);
  }
}

bool terrama2::core::isValidColumn(size_t value)
{
  return value != std::numeric_limits<size_t>::max();
}


std::string terrama2::core::getProperty(DataSetPtr dataSet, DataSeriesPtr dataSeries, std::string tag, bool logErrors)
{
  std::string property;
  try
  {
    auto semantics = dataSeries->semantics;
    property = semantics.metadata.at(tag);
  }
  catch(...)  //exceptions will be treated later
  {
  }

  if(property.empty())
  {
    try
    {
      property = dataSet->format.at(tag);
    }
    catch(...)  //exceptions will be treated later
    {
    }
  }


  if(property.empty())
  {
    QString errMsg = QObject::tr("Undefined %2 in dataset: %1.").arg(dataSet->id).arg(QString::fromStdString(tag));
    if(logErrors)
      TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }

  return property;
}
