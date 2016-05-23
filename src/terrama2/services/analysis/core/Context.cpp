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
  \file terrama2/services/analysis/core/Context.cpp

  \brief Class to store the context of execution of an python script.

  \author Paulo R. M. Oliveira
*/

#include "Context.hpp"
#include "Analysis.hpp"
#include "Exception.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/Typedef.hpp"

// QT
#include <QString>
#include <QObject>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/sam/kdtree.h>
#include <terralib/common/UnitsOfMeasureManager.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>
#include <terralib/srs/Converter.h>

#include <ctime>
#include <iomanip>
#include <mutex>



std::map<std::string, std::map<std::string, double> > terrama2::services::analysis::core::Context::analysisResult(AnalysisId analysisId)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return analysisResult_[analysisId];
}

void terrama2::services::analysis::core::Context::setAnalysisResult(AnalysisId analysisId, const std::string& geomId, const std::string& attribute, double result)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto& geomIdMap = analysisResult_[analysisId];
  auto& attributeMap = geomIdMap[geomId];
  attributeMap[attribute] = result;
}


std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> terrama2::services::analysis::core::Context::getContextDataset(const AnalysisId analysisId, const DataSetId datasetId, const std::string& dateFilter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  if(it == datasetMap_.end())
  {
    return std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries>();
  }

  return it->second;
}

void terrama2::services::analysis::core::Context::loadMonitoredObject(const terrama2::services::analysis::core::Analysis &analysis)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
    auto datasets = dataSeriesPtr->datasetList;
    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(datasets.size() == 1);
      auto dataset = datasets[0];

      auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
      terrama2::core::Filter filter;

      //accessing data
      terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
      auto seriesMap = accessor->getSeries(filter);
      auto series = seriesMap[dataset];

      auto format = dataset->format;
      std::string identifier = format["identifier"];

      std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

      if(!series.syncDataSet->dataset())
      {
        QString msg(QObject::tr("Analysis: %1 -> Adding an invalid dataset to the analysis context: DataSeries %2").arg(analysis.id).arg(dataSeriesPtr->id));
        TERRAMA2_LOG_ERROR() << msg;
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);
      }

      std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

      dataSeriesContext->series = series;
      dataSeriesContext->identifier = identifier;
      dataSeriesContext->geometryPos = geomPropertyPosition;

      ContextKey key;
      key.datasetId_ = dataset->id;
      key.analysisId_ = analysis.id;
      datasetMap_[key] = dataSeriesContext;
    }
    else if(analysisDataSeries.type == DATASERIES_PCD_TYPE)
    {
      for(auto dataset : dataSeriesPtr->datasetList)
      {
        auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
        terrama2::core::Filter filter;

        //accessing data
        terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
        auto seriesMap = accessor->getSeries(filter);
        auto series = seriesMap[dataset];

        auto format = dataset->format;
        std::string identifier = format["identifier"];

        std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

        std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

        dataSeriesContext->series = series;
        dataSeriesContext->identifier = identifier;
        dataSeriesContext->geometryPos = geomPropertyPosition;

        ContextKey key;
        key.datasetId_ = dataset->id;
        key.analysisId_ = analysis.id;
        datasetMap_[key] = dataSeriesContext;
      }
    }
  }
}

void terrama2::services::analysis::core::Context::addDCP(const AnalysisId analysisId, terrama2::core::DataSeriesPtr dataSeries, const std::string& dateFilter, const bool lastValue)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(auto dataset : dataSeries->datasetList)
  {
    if(!exists(analysisId, dataset->id, dateFilter))
    {
      needToAdd = true;
      break;
    }
  }

  if(!needToAdd)
    return;

  time_t ts = 0;
  struct tm t;
  char buf[16];
  ::localtime_r(&ts, &t);
  ::strftime(buf, sizeof(buf), "%Z", &t);


  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(buf));
  boost::local_time::local_date_time ldt = boost::local_time::local_microsec_clock::local_time(zone);

  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  auto dataProvider = dataManagerPtr->findDataProvider(dataSeries->dataProviderId);
  terrama2::core::Filter filter;
  filter.lastValue = lastValue;

  if(!dateFilter.empty())
  {
    double minutes = terrama2::core::TimeUtils::convertTimeStringToSeconds(dateFilter, "MINUTE");

    ldt -= boost::posix_time::minutes(minutes);

    std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
    filter.discardBefore = std::move(titz);
  }

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries, filter);
  std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter);

  for(auto mapItem : seriesMap)
  {
    auto series = mapItem.second;

    auto format = series.dataSet->format;
    std::string identifier = format["identifier"];

    std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);
    dataSeriesContext->series = series;
    dataSeriesContext->identifier = identifier;

    terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(series.dataSet);
    if(!dcpDataset->position)
    {
      QString msg(QObject::tr("Invalid location for DCP."));
      TERRAMA2_LOG_ERROR() << msg;
      throw InvalidDataSetException() << terrama2::ErrorDescription(msg);
    }

    int srid  = dcpDataset->position->getSRID();
    if(srid == 0)
    {
      if(format.find("srid") != format.end())
      {
        srid = std::stoi(format["srid"]);
        dcpDataset->position->setSRID(srid);
      }
    }

    // if data projection is in decimal degrees we need to convert it to a meter projection.
    auto spatialReferenceSystem = te::srs::SpatialReferenceSystemManager::getInstance().getSpatialReferenceSystem(dcpDataset->position->getSRID());
    std::string unitName = spatialReferenceSystem->getUnitName();
    if(unitName == "degree")
    {
      // Converts the data to UTM
      int sridUTM = terrama2::core::getUTMSrid(dcpDataset->position.get());
      dcpDataset->position->transform(sridUTM);
    }

    dataSeriesContext->rtree.insert(*dcpDataset->position->getMBR(), dcpDataset->id);


    ContextKey key;
    key.datasetId_ = series.dataSet->id;
    key.analysisId_ = analysisId;
    key.dateFilter_ = dateFilter;
    datasetMap_[key] = dataSeriesContext;
  }
}

bool terrama2::services::analysis::core::Context::exists(const AnalysisId analysisId, const DataSetId datasetId, const std::string& dateFilter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  return it != datasetMap_.end();
}

terrama2::services::analysis::core::Analysis terrama2::services::analysis::core::Context::getAnalysis(AnalysisId analysisId) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  return dataManagerPtr->findAnalysis(analysisId);
}

void terrama2::services::analysis::core::Context::addDataset(const AnalysisId analysisId, terrama2::core::DataSeriesPtr dataSeries, const std::string& dateFilter, bool createSpatialIndex)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(auto dataset : dataSeries->datasetList)
  {
    if(!exists(analysisId, dataset->id, dateFilter))
    {
      needToAdd = true;
      break;
    }
  }

  if(!needToAdd)
    return;

  time_t ts = 0;
  struct tm t;
  char buf[16];
  ::localtime_r(&ts, &t);
  ::strftime(buf, sizeof(buf), "%Z", &t);


  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(buf));
  boost::local_time::local_date_time ldt = boost::local_time::local_microsec_clock::local_time(zone);

  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  auto dataProvider = dataManagerPtr->findDataProvider(dataSeries->dataProviderId);


  terrama2::core::Filter filter;
  if(!dateFilter.empty())
  {
    double minutes = terrama2::core::TimeUtils::convertTimeStringToSeconds(dateFilter, "MINUTE");

    ldt -= boost::posix_time::minutes(minutes);

    std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
    filter.discardBefore = std::move(titz);
  }

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries, filter);
  std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter);

  for(auto mapItem : seriesMap)
  {
    auto series = mapItem.second;

    auto format = series.dataSet->format;
    std::string identifier = format["identifier"];

    std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

    std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

    dataSeriesContext->series = series;
    dataSeriesContext->identifier = identifier;
    dataSeriesContext->geometryPos = geomPropertyPosition;

    if(createSpatialIndex)
    {
      int size = series.syncDataSet->size();
      for(std::size_t i = 0; i < size; ++i)
      {

        auto geom = series.syncDataSet->getGeometry(i, geomPropertyPosition);
        dataSeriesContext->rtree.insert(*geom->getMBR(), i);
      }
    }


    ContextKey key;
    key.datasetId_ = series.dataSet->id;
    key.analysisId_ = analysisId;
    key.dateFilter_ = dateFilter;
    datasetMap_[key] = dataSeriesContext;
  }
}

void terrama2::services::analysis::core::Context::setDataManager(std::weak_ptr<terrama2::services::analysis::core::DataManager> dataManager)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  dataManager_ = dataManager;
}


std::weak_ptr<terrama2::services::analysis::core::DataManager> terrama2::services::analysis::core::Context::getDataManager()
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return dataManager_;
}

std::set<std::string> terrama2::services::analysis::core::Context::getAttributes(AnalysisId analysisId) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return attributes_.at(analysisId);
}
void terrama2::services::analysis::core::Context::addAttribute(AnalysisId analysisId, const std::string& attribute)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  attributes_[analysisId].insert(attribute);
}
