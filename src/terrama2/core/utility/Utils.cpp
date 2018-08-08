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

// Boost
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <terralib/core/filesystem/FileSystem.h>
#include <terralib/core/plugin/PluginInfo.h>
#include <terralib/core/plugin/PluginManager.h>
#include <terralib/core/plugin/Serializers.h>
#include <terralib/core/plugin/Utils.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
// TerraLib
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/common/TerraLib.h>
#include <terralib/common/UnitsOfMeasureManager.h>
#include <terralib/core/logger/../utils/Platform.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/Enums.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <algorithm>
#include <fstream>
#include <limits>
#include <map>
#include <string>
#include <unordered_map>

#include "../Exception.hpp"
#include "../../Exception.hpp"
#include "../data-model/DataManager.hpp"
#include "../data-model/DataProvider.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-model/DataSet.hpp"
#include "../data-model/Filter.hpp"
#include "Logger.hpp"
#include "Raii.hpp"
// TerraMA2
#include "Utils.hpp"
#include "terrama2_config.hpp"

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
  boost::filesystem::path tma_path = te::core::FileSystem::executableDirectory();

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
  std::vector<std::string> secondAlternativeNames {"s", "sec", "second", "seconds"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomSecond, secondAlternativeNames);

  // minute
  te::common::UnitOfMeasurePtr uomMinute(new te::common::UnitOfMeasure(1, "MINUTE", "min", te::common::MeasureType::Time, te::common::UOM_second, 60.0, 0.0, 0.0, 1.0));
  std::vector<std::string> minuteAlternativeNames {"min", "minute", "minutes"};

  // hour
  te::common::UnitOfMeasurePtr uomHour(new te::common::UnitOfMeasure(2, "HOUR", "h", te::common::MeasureType::Time, te::common::UOM_second, 3600.0, 0.0, 0.0, 1.0));
  std::vector<std::string> hourAlternativeNames {"h", "hour", "hours"};

  // day
  te::common::UnitOfMeasurePtr uomDay(new te::common::UnitOfMeasure(3, "DAY", "d", te::common::MeasureType::Time, te::common::UOM_second, 86400.0, 0.0, 0.0, 1.0));
  std::vector<std::string> dayAlternativeNames {"d", "day", "days"};

  // week
  te::common::UnitOfMeasurePtr uomWeek(new te::common::UnitOfMeasure(4, "WEEK", "w", te::common::MeasureType::Time, te::common::UOM_second, 604800.0, 0.0, 0.0, 1.0));
  std::vector<std::string> weekAlternativeNames {"w", "wk", "week", "weeks"};

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
  std::vector<std::string> kmAlternativeNames {"km", "kilometer", "kilometers"};
  // centimeter
  te::common::UnitOfMeasurePtr uomCm(new te::common::UnitOfMeasure(6, "CENTIMETER", "cm", te::common::MeasureType::Length, te::common::UOM_Metre, 0.01, 0.0, 0.0, 1.0));
  std::vector<std::string> cmAlternativeNames {"cm", "centimeter", "centimeters"};


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
      // protection for invalid javascript data
     if(property=="null")
       property.clear();
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
      // protection for invalid javascript data
      if(property=="null")
        property.clear();
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

  if(!filter.byValue.empty())
  {
    boost::hash_combine(hash, filter.byValue);
  }

  if(filter.lastValues)
    boost::hash_combine(hash, *filter.lastValues);

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

std::string terrama2::core::getTimeInterval(terrama2::core::DataSetPtr dataset)
{
  try
  {
    std::string interval = dataset->format.at("time_interval");
    std::string unit = dataset->format.at("time_interval_unit");

    return interval+unit;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for time interval in dataset: %1.").arg(dataset->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

size_t terrama2::core::propertyPosition(const te::da::DataSet* dataSet, const std::string& propertyName)
{
  for(std::size_t i = 0; i < dataSet->getNumProperties(); i++)
  {
    if(dataSet->getPropertyName(i) == propertyName)
    {
      return i;
    }
  }

  return std::numeric_limits<size_t>::max();
}

std::string terrama2::core::createValidPropertyName(const std::string& oldName)
{
  std::string name = simplifyString(oldName);

  if(std::isdigit(name.at(0)))
    name ="_" + name;

  return name;
}

std::vector<std::string> terrama2::core::splitString(const std::string& text, char delim)
{
  std::stringstream ss;
  ss.str(text);

  std::vector<std::string> splittedString;

  std::string str;

  while(std::getline(ss, str, delim))
  {
    if(!str.empty())
    splittedString.push_back(str);
  }

  return splittedString;
}

std::vector<std::shared_ptr<te::dt::TimeInstantTZ> > terrama2::core::getAllDates(te::da::DataSet* teDataset,
                                                                                 const std::string& datetimeColumnName)
{
  std::vector<std::shared_ptr<te::dt::TimeInstantTZ> > vecDates;

  teDataset->moveBeforeFirst();
  while(teDataset->moveNext())
  {
    // Retrieve all execution dates of dataset
    std::shared_ptr<te::dt::TimeInstantTZ> executionDate(static_cast<te::dt::TimeInstantTZ*>(teDataset->getDateTime(datetimeColumnName).release()));

    auto it = std::lower_bound(vecDates.begin(), vecDates.end(), executionDate,
                               [&](std::shared_ptr<te::dt::TimeInstantTZ> const& first, std::shared_ptr<te::dt::TimeInstantTZ> const& second)
    {
              return *first < *second;
  });

    if (it != vecDates.end() && **it == *executionDate)
      continue;

    vecDates.insert(it, executionDate);
  }

  return vecDates;
}

std::string terrama2::core::getDCPPositionsTableName(DataSeriesPtr dataSeries)
{
  return "dcp_series_"+std::to_string(dataSeries->id);
}

std::string terrama2::core::getMask(DataSetPtr dataSet)
{
  try
  {
    return dataSet->format.at("mask");
  }
  catch(const std::out_of_range& /*e*/)
  {
    QString errMsg = QObject::tr("Undefined mask in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::getFileMask(DataSetPtr dataSet)
{
  std::string mask = getMask(dataSet);

  std::string fileMask = "";

  auto pos = mask.find_last_of("\\/");

  if(pos != std::string::npos)
  {
    fileMask = mask.substr(pos+1);
  }
  else
  {
    fileMask = mask;
  }

  return fileMask;
}

std::string terrama2::core::getFolderMask(DataSetPtr dataSet)
{
  std::string mask = getMask(dataSet);

  std::string folderMask = "";

  auto pos = mask.find_last_of("\\/");

  if(pos != std::string::npos)
  {
    for(size_t i = 0; i < pos; ++i)
      folderMask +=mask.at(i);
  }

  return folderMask;
}

std::string terrama2::core::readFileContents(const std::string& absoluteFilePath)
{
  // open file
  std::ifstream fileStream(absoluteFilePath, std::ios_base::in);
  if(!fileStream.is_open())
  {
    QString errMsg = QObject::tr("Unable to open file: %1.").arg(QString::fromStdString(absoluteFilePath));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  std::string str;

  // reserve content space at str
  fileStream.seekg(0, std::ios::end);
  str.reserve(fileStream.tellg());
  fileStream.seekg(0, std::ios::beg);

  // read file content
  str.assign((std::istreambuf_iterator<char>(fileStream)),
              std::istreambuf_iterator<char>());

  return str;
}

std::pair<std::shared_ptr<te::da::DataSetType>, std::shared_ptr<te::da::DataSet> >
terrama2::core::getDCPPositionsTable(std::shared_ptr<te::da::DataSource> datasource, const std::string& dataSetName)
{
  terrama2::core::OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasource); Q_UNUSED(openClose);

  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasource->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);

  std::shared_ptr<te::da::DataSet> teDataset = transactorDestination->getDataSet(dataSetName);
  std::shared_ptr<te::da::DataSetType> teDataSetType = transactorDestination->getDataSetType(dataSetName);

  return {teDataSetType, teDataset};
}

void terrama2::core::erasePreviousResult(DataManagerPtr dataManager, DataSeriesId dataSeriesId, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
{
  auto outputDataSeries = dataManager->findDataSeries(dataSeriesId);
  if(!outputDataSeries)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Invalid output data series for analysis.");
    return;
  }
  auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);
  if(!outputDataProvider)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Invalid output data provider for analysis.");
    return;
  }

  if(outputDataProvider->dataProviderType == "POSTGIS")
  {
    auto dataset = outputDataSeries->datasetList[0];
    std::string tableName;

    try
    {
      tableName = dataset->format.at("table_name");
    }
    catch(...)
    {
      QString errMsg = QObject::tr("Undefined table name in dataset: %1.").arg(dataset->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::UndefinedTagException() << ErrorDescription(errMsg);
    }

    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("POSTGIS", outputDataProvider->uri));

    // RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    if(!datasource->isOpened())
    {
      QString errMsg = QObject::tr("DataProvider could not be opened.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

    auto dataSetNames = transactor->getDataSetNames();

    if(std::find(dataSetNames.cbegin(), dataSetNames.cend(), tableName) != dataSetNames.cend() ||
       std::find(dataSetNames.cbegin(), dataSetNames.cend(), "public."+tableName) != dataSetNames.cend())
      transactor->execute("delete from " + tableName + " where execution_date = '" + startTime->toString() + "'");
  }
  else
  {
    QString errMsg = QObject::tr("Removing old results not implement for this dataseries format.");
    TERRAMA2_LOG_ERROR() << errMsg;
  }
}
