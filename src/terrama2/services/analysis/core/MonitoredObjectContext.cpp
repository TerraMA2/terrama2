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
  \file terrama2/services/analysis/occurrence/GridContext.cpp

  \brief Contains occurrence analysis contextDataSeries.

  \author Jano Simas
*/

#include "MonitoredObjectContext.hpp"
#include "DataManager.hpp"
#include "utility/Utils.hpp"
#include "python/PythonInterpreter.hpp"

#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Raii.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>

//terralib
#include <terralib/dataaccess/datasource/DataSourceCapabilities.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/utils/Utils.h>

// Boost Python
#include <boost/python/call.hpp>
#include <boost/python/object.hpp>
#include <boost/python/import.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/make_function.hpp>
#include <boost/bind.hpp>

#include <utility>

terrama2::services::analysis::core::MonitoredObjectContext::MonitoredObjectContext(terrama2::services::analysis::core::DataManagerPtr dataManager, terrama2::services::analysis::core::AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
  : BaseContext(dataManager, analysis, startTime)
{
}

void terrama2::services::analysis::core::MonitoredObjectContext::loadMonitoredObject()
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto analysis = getAnalysis();

  for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
    auto datasets = dataSeriesPtr->datasetList;
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      if(analysis->type == AnalysisType::MONITORED_OBJECT_TYPE)
      {
        assert(datasets.size() == 1);
        auto dataset = datasets[0];

        auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
        terrama2::core::Filter filter;

        //accessing data
        terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
        auto seriesMap = accessor->getSeries(filter, remover_);
        auto series = seriesMap[dataset];

        std::string identifier;
        try
        {
          identifier = analysisDataSeries.metadata.at("identifier");
        }
        catch (...)
        {
          /* code */
        }

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

        ObjectKey key(dataset->id);
        datasetMap_[key] = dataSeriesContext;
      }
      else if(analysis->type == AnalysisType::DCP_TYPE)
      {
        auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
        te::core::URI uri(dataProvider->uri);
        std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("POSTGIS", uri));

        terrama2::core::OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);

        std::string destinationDataSetName = getDCPPositionsTableName(dataSeriesPtr);

        std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
        te::da::ScopedTransaction scopedTransaction(*transactorDestination);

        std::shared_ptr<te::da::DataSet> teDataset = transactorDestination->getDataSet(destinationDataSetName);
        std::shared_ptr<te::da::DataSetType> teDataSetType = transactorDestination->getDataSetType(destinationDataSetName);

        terrama2::core::DataSetSeries series;
        series.dataSet = nullptr;
        series.syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(teDataset);
        series.teDataSetType = teDataSetType;

        std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);
        dataSeriesContext->series = series;
        dataSeriesContext->identifier = "table_name";
        dataSeriesContext->geometryPos = teDataSetType->getPropertyPosition("geom");

        ObjectKey key(DCP_ANALYSIS_DATASET);
        datasetMap_[key] = dataSeriesContext;
      }
    }
    else if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_PCD_TYPE)
    {
      for(const auto& dataset : dataSeriesPtr->datasetList)
      {
        auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
        terrama2::core::Filter filter;

        //accessing data
        terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
        auto seriesMap = accessor->getSeries(filter, remover_);
        auto series = seriesMap[dataset];

        std::string identifier;
        try
        {
          identifier = analysisDataSeries.metadata.at("identifier");
        }
        catch (...)
        {
          /* code */
        }


        std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

        std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

        dataSeriesContext->series = series;
        dataSeriesContext->identifier = identifier;
        dataSeriesContext->geometryPos = geomPropertyPosition;

        ObjectKey key(dataset->id);
        datasetMap_[key] = dataSeriesContext;
      }
    }
  }
}

void terrama2::services::analysis::core::MonitoredObjectContext::addDCPDataSeries(terrama2::core::DataSeriesPtr dataSeries,
                                                                                  const terrama2::core::Filter& filter)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(const auto& dataset : dataSeries->datasetList)
  {
    if(!exists(dataset->id, filter))
    {
      needToAdd = true;
      break;
    }
  }

  auto analysis = getAnalysis();

  if(!needToAdd)
    return;

  time_t ts = 0;
  struct tm t;
  char buf[16];
  ::localtime_r(&ts, &t);
  ::strftime(buf, sizeof(buf), "%Z", &t);



  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto dataProvider = dataManagerPtr->findDataProvider(dataSeries->dataProviderId);

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries);
  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter, remover_);

  if(seriesMap.empty())
  {
    QString errMsg(QObject::tr("The data series %1 does not contain data").arg(dataSeries->id));
    throw EmptyDataSeriesException() << terrama2::ErrorDescription(errMsg);
  }

  for(const auto& mapItem : seriesMap)
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

    dataSeriesContext->rtree.insert(*dcpDataset->position->getMBR(), dcpDataset->id);

    ObjectKey key(series.dataSet->id, filter);
    datasetMap_[key] = dataSeriesContext;
  }
}

std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries>
terrama2::services::analysis::core::MonitoredObjectContext::getContextDataset(const DataSetId datasetId, const terrama2::core::Filter& filter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ObjectKey key(datasetId, filter);

  try
  {
    return datasetMap_.at(key);
  }
  catch (const std::out_of_range&)
  {
    return std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries>();
  }
}

bool terrama2::services::analysis::core::MonitoredObjectContext::exists(const DataSetId datasetId, const terrama2::core::Filter& filter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ObjectKey key(datasetId, filter);

  auto it = datasetMap_.find(key);
  return it != datasetMap_.end();
}

void terrama2::services::analysis::core::MonitoredObjectContext::addDataSeries(terrama2::core::DataSeriesPtr dataSeries,
                                                                               const terrama2::core::Filter& filter,
                                                                               bool createSpatialIndex)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(const auto& dataset : dataSeries->datasetList)
  {
    if(!exists(dataset->id, filter))
    {
      needToAdd = true;
      break;
    }
  }

  auto analysis = getAnalysis();

  if(!needToAdd)
    return;

  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto dataProvider = dataManagerPtr->findDataProvider(dataSeries->dataProviderId);


  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries);
  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter, remover_);


  if(seriesMap.empty())
  {
    QString errMsg(QObject::tr("The data series %1 does not contain data").arg(dataSeries->id));
    throw EmptyDataSeriesException() << terrama2::ErrorDescription(errMsg);
  }

  for(const auto& mapItem : seriesMap)
  {
    auto series = mapItem.second;


    std::shared_ptr<ContextDataSeries> dataSeriesContext(new ContextDataSeries);

    std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

    dataSeriesContext->series = series;
    dataSeriesContext->geometryPos = geomPropertyPosition;

    if(createSpatialIndex)
    {
      for(std::size_t i = 0; i < series.syncDataSet->size(); ++i)
      {

        auto geom = series.syncDataSet->getGeometry(i, geomPropertyPosition);
        dataSeriesContext->rtree.insert(*geom->getMBR(), i);
      }
    }

    ObjectKey key(series.dataSet->id, filter);
    datasetMap_[key] = dataSeriesContext;
  }
}

void terrama2::services::analysis::core::MonitoredObjectContext::addAttribute(const std::string& attribute, int dataType)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  attributes_.insert(std::make_pair(attribute, dataType));
}

void terrama2::services::analysis::core::MonitoredObjectContext::setAnalysisResult(const int index, const std::string& attribute, boost::any result)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  auto& attributeMap = analysisResult_[index];
  attributeMap[attribute] = result;
}

std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries>
terrama2::services::analysis::core::MonitoredObjectContext::getMonitoredObjectContextDataSeries(std::shared_ptr<DataManager>& dataManagerPtr)
{

  std::lock_guard<std::recursive_mutex> lock(mutex_);

  auto analysis = getAnalysis();
  for(const AnalysisDataSeries& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

    if(analysis->type == AnalysisType::MONITORED_OBJECT_TYPE && analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(dataSeries->datasetList.size() == 1);
      auto datasetMO = dataSeries->datasetList[0];

      terrama2::core::Filter filter;
      if(!exists(datasetMO->id, filter))
      {
        QString errMsg(QObject::tr("Could not recover monitored object dataset."));

        addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
        return nullptr;
      }

      return getContextDataset(datasetMO->id, filter);
    }
    else if(analysis->type == AnalysisType::DCP_TYPE && analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      terrama2::core::Filter filter;
      if(!exists(DCP_ANALYSIS_DATASET, filter))
      {
        QString errMsg(QObject::tr("Could not recover monitored object dataset."));

        addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
        return nullptr;
      }

      return getContextDataset(DCP_ANALYSIS_DATASET, filter);
    }
  }

  return nullptr;
}

std::shared_ptr<te::gm::Geometry> terrama2::services::analysis::core::MonitoredObjectContext::getDCPBuffer(const DataSetId datasetId)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  terrama2::core::Filter filter;
  ObjectKey key(datasetId, filter);

  try
  {
    return bufferDcpMap_.at(key);
  }
  catch (const std::out_of_range&)
  {
    return nullptr;
  }
}

void terrama2::services::analysis::core::MonitoredObjectContext::addDCPBuffer(const DataSetId datasetId, std::shared_ptr<te::gm::Geometry> buffer)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  terrama2::core::Filter filter;
  ObjectKey key(datasetId, filter);
  bufferDcpMap_[key] = buffer;
}
