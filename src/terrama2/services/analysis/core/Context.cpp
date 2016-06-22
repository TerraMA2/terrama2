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
#include <pystate.h>


std::map<std::string, std::map<std::string, double> > terrama2::services::analysis::core::Context::analysisResult(AnalysisHashCode analysisHashCode)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return analysisResult_[analysisHashCode];
}

void terrama2::services::analysis::core::Context::setAnalysisResult(AnalysisHashCode analysisHashCode, const std::string& geomId, const std::string& attribute, double result)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto& geomIdMap = analysisResult_[analysisHashCode];
  auto& attributeMap = geomIdMap[geomId];
  attributeMap[attribute] = result;
}


std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> terrama2::services::analysis::core::Context::getContextDataset(const AnalysisHashCode analysisHashCode, const DataSetId datasetId, const std::string& dateFilter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.analysisHashCode_ = analysisHashCode;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  if(it == datasetMap_.end())
  {
    return std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries>();
  }

  return it->second;
}

void terrama2::services::analysis::core::Context::loadMonitoredObject(AnalysisHashCode analysisHashCode)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto analysis = getAnalysis(analysisHashCode);

  for(auto analysisDataSeries : analysis->analysisDataSeriesList)
  {
    auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
    auto datasets = dataSeriesPtr->datasetList;
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(datasets.size() == 1);
      auto dataset = datasets[0];

      auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
      terrama2::core::Filter filter;

      //accessing data
      terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
      auto seriesMap = accessor->getSeries(filter);
      auto series = seriesMap[dataset];

      std::string identifier = analysisDataSeries.metadata["identifier"];

      std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

      if(!series.syncDataSet)
      {
        QString errMsg(QObject::tr("No data available for DataSeries %1").arg(dataSeriesPtr->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      if(!series.syncDataSet->dataset())
      {
        QString errMsg(QObject::tr("Adding an invalid dataset to the analysis context: DataSeries %1").arg(dataSeriesPtr->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

      dataSeriesContext->series = series;
      dataSeriesContext->identifier = identifier;
      dataSeriesContext->geometryPos = geomPropertyPosition;

      ContextKey key;
      key.datasetId_ = dataset->id;
      key.analysisHashCode_ = analysisHashCode;
      datasetMap_[key] = dataSeriesContext;
    }
    else if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_PCD_TYPE)
    {
      for(auto dataset : dataSeriesPtr->datasetList)
      {
        auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
        terrama2::core::Filter filter;

        //accessing data
        terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
        auto seriesMap = accessor->getSeries(filter);
        auto series = seriesMap[dataset];

        std::string identifier = analysisDataSeries.metadata["identifier"];

        std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

        std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

        dataSeriesContext->series = series;
        dataSeriesContext->identifier = identifier;
        dataSeriesContext->geometryPos = geomPropertyPosition;

        ContextKey key;
        key.datasetId_ = dataset->id;
        key.analysisHashCode_ = analysisHashCode;
        datasetMap_[key] = dataSeriesContext;
      }
    }
  }
}

void terrama2::services::analysis::core::Context::addDCPDataSeries(const AnalysisHashCode analysisHashCode,
                                                                   terrama2::core::DataSeriesPtr dataSeries,
                                                                   const std::string& dateFilter, const bool lastValue)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(auto dataset : dataSeries->datasetList)
  {
    if(!exists(analysisHashCode, dataset->id, dateFilter))
    {
      needToAdd = true;
      break;
    }
  }

  auto analysis = Context::getInstance().getAnalysis(analysisHashCode);

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
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto dataProvider = dataManagerPtr->findDataProvider(dataSeries->dataProviderId);
  terrama2::core::Filter filter;
  filter.lastValue = lastValue;
  filter.discardAfter = analysisStartTime_[analysisHashCode];

  if(!dateFilter.empty())
  {
    double seconds = terrama2::core::TimeUtils::convertTimeString(dateFilter, "SECOND", "h");

    ldt -= boost::posix_time::seconds(seconds);

    std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
    filter.discardBefore = std::move(titz);
  }

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries, filter);
  std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter);

  if(seriesMap.empty())
  {
    QString errMsg(QObject::tr("The data series %1 does not contain data").arg(dataSeries->id));
    throw EmptyDataSeriesException() << terrama2::ErrorDescription(errMsg);
  }

  for(auto mapItem : seriesMap)
  {
    auto series = mapItem.second;

    std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);
    dataSeriesContext->series = series;

    terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(series.dataSet);
    if(!dcpDataset->position)
    {
      QString errMsg(QObject::tr("Invalid location for DCP."));
      throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
    }

    int srid  = dcpDataset->position->getSRID();
    if(srid == 0)
    {
      if(dcpDataset->format.find("srid") != dcpDataset->format.end())
      {
        srid = std::stoi(dcpDataset->format.at("srid"));
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
    key.analysisHashCode_ = analysisHashCode;
    key.dateFilter_ = dateFilter;
    datasetMap_[key] = dataSeriesContext;
  }
}

bool terrama2::services::analysis::core::Context::exists(const AnalysisHashCode analysisHashCode, const DataSetId datasetId, const std::string& dateFilter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.analysisHashCode_ = analysisHashCode;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  return it != datasetMap_.end();
}

terrama2::services::analysis::core::AnalysisPtr terrama2::services::analysis::core::Context::getAnalysis(AnalysisHashCode analysisHashCode) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  auto it = analysisMap_.find(analysisHashCode);
  if(it != analysisMap_.end())
    return it->second;

  QString msg(QObject::tr("Could not find the analysis in the Context."));
  throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);

}

void terrama2::services::analysis::core::Context::addDataSeries(const AnalysisHashCode analysisHashCode,
                                                                terrama2::core::DataSeriesPtr dataSeries,
                                                                std::shared_ptr<te::gm::Geometry> envelope,
                                                                const std::string& dateFilter, bool createSpatialIndex)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(auto dataset : dataSeries->datasetList)
  {
    if(!exists(analysisHashCode, dataset->id, dateFilter))
    {
      needToAdd = true;
      break;
    }
  }

  auto analysis = Context::getInstance().getAnalysis(analysisHashCode);

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
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto dataProvider = dataManagerPtr->findDataProvider(dataSeries->dataProviderId);


  terrama2::core::Filter filter;

  filter.discardAfter = analysisStartTime_[analysisHashCode];

  if(!dateFilter.empty())
  {
    double seconds = terrama2::core::TimeUtils::convertTimeString(dateFilter, "SECOND", "h");

    ldt -= boost::posix_time::seconds(seconds);

    std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
    filter.discardBefore = std::move(titz);
  }

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries, filter);
  std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter);


  if(seriesMap.empty())
  {
    QString errMsg(QObject::tr("The data series %1 does not contain data").arg(dataSeries->id));
    throw EmptyDataSeriesException() << terrama2::ErrorDescription(errMsg);
  }

  for(auto mapItem : seriesMap)
  {
    auto series = mapItem.second;


    std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

    std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

    dataSeriesContext->series = series;
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
    key.analysisHashCode_ = analysisHashCode;
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

std::set<std::string> terrama2::services::analysis::core::Context::getAttributes(AnalysisHashCode analysisHashCode) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return attributes_.at(analysisHashCode);
}
void terrama2::services::analysis::core::Context::addAttribute(AnalysisHashCode analysisHashCode, const std::string& attribute)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  attributes_[analysisHashCode].insert(attribute);
}

void terrama2::services::analysis::core::Context::clearAnalysisContext(AnalysisHashCode analysisHashCode)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  attributes_.erase(analysisHashCode);
  analysisResult_.erase(analysisHashCode);
  analysisMap_.erase(analysisHashCode);
  analysisErrorsMap_.erase(analysisHashCode);

  // Remove all datasets from context
  auto it = datasetMap_.begin();
  while(it != datasetMap_.end())
  {
    if(it->first.analysisHashCode_ ==  analysisHashCode)
    {
      datasetMap_.erase(it++);
    }
    else
    {
      ++it;
    }
  }
}

void terrama2::services::analysis::core::Context::addAnalysis(AnalysisHashCode analysisHashCode, AnalysisPtr analysis)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  analysisMap_[analysisHashCode] = analysis;
}

void terrama2::services::analysis::core::Context::setMainThreadState(PyThreadState* state)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  mainThreadState_ = state;
}

PyThreadState* terrama2::services::analysis::core::Context::getMainThreadState() const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return mainThreadState_;
}

void terrama2::services::analysis::core::Context::addError(AnalysisHashCode analysisHashCode, const std::string& errorMessage)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  analysisErrorsMap_[analysisHashCode].insert(errorMessage);
}

std::set<std::string> terrama2::services::analysis::core::Context::getErrors(AnalysisHashCode analysisHashCode)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return analysisErrorsMap_[analysisHashCode];
}

void terrama2::services::analysis::core::Context::setStartTime(AnalysisHashCode analysisHashCode, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  analysisStartTime_[analysisHashCode] = startTime;
}

std::shared_ptr<te::dt::TimeInstantTZ> terrama2::services::analysis::core::Context::getStartTime(AnalysisHashCode analysisHashCode)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  return analysisStartTime_[analysisHashCode];
}












