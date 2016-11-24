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
#include <terralib/core/plugin.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/sa/core/Utils.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>
#include <terralib/geometry/WKTReader.h>
#include <terralib/geometry/MultiPolygon.h>

#include <ctime>
#include <unordered_map>
#include <functional>

// Boost
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>

// QT
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

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
  te::core::plugin::InitializePluginSystem();

  std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");
  // Load all the config files for the plugins.
  std::vector<te::core::PluginInfo> v_pInfo;

  v_pInfo.push_back(te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.pgis.teplg.json"));
  v_pInfo.push_back(te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.gdal.teplg.json"));
  v_pInfo.push_back(te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.ogr.teplg.json"));

  // Insert all the plugins stored in the vector from a given PluginInfo.
  v_pInfo = te::core::plugin::TopologicalSort(v_pInfo);

  for(const te::core::PluginInfo& pinfo : v_pInfo)
  {
    te::core::PluginManager::instance().insert(pinfo);
    te::core::PluginManager::instance().load(pinfo.name);
  }

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
  // get all the loaded plugins
  std::vector<te::core::PluginInfo> pVec = te::core::PluginManager::instance().getLoadedPlugins();
    // unload it in the reverse order
  for (auto plugin = pVec.rbegin(); plugin != pVec.rend(); ++plugin)
  {
    te::core::PluginManager::instance().stop(plugin->name);
    te::core::PluginManager::instance().unload(plugin->name);
  }
  te::core::PluginManager::instance().clear();

  te::core::plugin::FinalizePluginSystem();
  TerraLib::getInstance().finalize();
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
   te::gm::Coord2D coord = GetCentroidCoord(geom);

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

te::gm::Coord2D terrama2::core::GetCentroidCoord(te::gm::Geometry* geom)
{
  assert(geom);

  te::gm::Coord2D coord;

  if(geom->getGeomTypeId() == te::gm::PointType)
  {
    te::gm::Point* p = (static_cast<te::gm::Point*>(geom));

    coord.x = p->getX();
    coord.y = p->getY();
  }
  else if(geom->getGeomTypeId() == te::gm::PolygonType)
  {
    te::gm::Point* p = (static_cast<te::gm::Polygon*>(geom))->getCentroid();

    coord.x = p->getX();
    coord.y = p->getY();

    delete p;
  }
  else if(geom->getGeomTypeId() == te::gm::MultiPolygonType)
  {
    te::gm::Point* p = (static_cast<te::gm::MultiPolygon*>(geom))->getCentroid();

    coord.x = p->getX();
    coord.y = p->getY();

    delete p;
  }

  return coord;
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
  else if(type == "GEOMETRIC_OBJECT")
    return terrama2::core::DataSeriesType::GEOMETRIC_OBJECT;
  else if(type == "ANALYSIS_MONITORED_OBJECT")
    return terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT;
  else
  {
    QString errMsg = QObject::tr("Unknown DataSeriesType: %1.").arg(QString::fromStdString(type));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataModelException() << terrama2::ErrorDescription(errMsg);
  }
}

terrama2::core::DataSeriesTemporality terrama2::core::dataSeriesTemporalityFromString(const std::string& temporality)
{
  if(temporality == "STATIC")
    return terrama2::core::DataSeriesTemporality::STATIC;
  else if(temporality == "DYNAMIC")
    return terrama2::core::DataSeriesTemporality::DYNAMIC;
  else
  {
    QString errMsg = QObject::tr("Unknown DataSeriesTemporality: %1.").arg(QString::fromStdString(temporality));
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
  if(dataSeries)
  {
    try
    {
      auto semantics = dataSeries->semantics;
      property = semantics.metadata.at(tag);
    }
    catch(...)  //exceptions will be treated later
    {
    }
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

std::shared_ptr<te::gm::Geometry> terrama2::core::ewktToGeom(const std::string& ewkt)
{
  auto pos = ewkt.find(";")+1;
  auto wkt = ewkt.substr(pos);
  auto sridStr = ewkt.substr(0, pos-1);
  pos = sridStr.find("=")+1;
  int srid = std::stoi(sridStr.substr(pos));

  auto geom = std::shared_ptr<te::gm::Geometry>(te::gm::WKTReader::read(wkt.c_str()));
  geom->setSRID(srid);

  return geom;
}

std::string terrama2::core::simplifyString(std::string text)
{
  boost::trim(text);
  text.erase(std::remove_if(text.begin(), text.end(), [](char x){return !(std::isalnum(x) || x == ' ');}), text.end());
  std::replace(text.begin(), text.end(), ' ', '_');
  return text;
}

size_t std::hash<terrama2::core::Filter>::operator()(terrama2::core::Filter const& filter) const
{
  size_t hash = 0;

  if(filter.discardBefore)
  {
    std::string discardBefore = filter.discardBefore->toString();
    size_t const hBefore = std::hash<std::string>()(discardBefore);
    boost::hash_combine(hash, hBefore);
  }

  if(filter.discardAfter)
  {
    std::string discardAfter = filter.discardAfter->toString();
    size_t const hAfter = std::hash<std::string>()(discardAfter);
    boost::hash_combine(hash, hAfter);
  }

  if(filter.region)
  {
    std::string region = filter.region->toString();
    size_t const hRegion = std::hash<std::string>()(region);
    boost::hash_combine(hash, hRegion);
  }

  if(filter.value)
  {
    boost::hash_combine(hash, *filter.value);
  }

  boost::hash_combine(hash, filter.lastValue);

  return hash;
}


bool terrama2::core::isTemporal(terrama2::core::DataSetPtr dataset)
{
  try
  {
    return dataset->format.at("temporal") == "true";
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for temporal field in dateset: %1.").arg(dataset->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::getTimeIntervalUnit(terrama2::core::DataSetPtr dataset)
{
  try
  {
    return dataset->format.at("time_interval_unit");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for time interval unit in dataset: %1.").arg(dataset->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

double terrama2::core::getTimeInterval(terrama2::core::DataSetPtr dataset)
{
  try
  {
    return std::stod(dataset->format.at("time_interval"), nullptr);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for time interval in dataset: %1.").arg(dataset->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}


std::string terrama2::core::getFolderMask(DataSetPtr dataSet, DataSeriesPtr dataSeries)
{
  return getProperty(dataSet, dataSeries, "folder", false);
}
