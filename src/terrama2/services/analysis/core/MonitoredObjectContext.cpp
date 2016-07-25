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
#include "dcp/Operator.hpp"
#include "dcp/history/Operator.hpp"
#include "occurrence/Operator.hpp"
#include "occurrence/aggregation/Operator.hpp"
#include "DataManager.hpp"
#include "Utils.hpp"
#include "PythonInterpreter.hpp"

#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/TimeUtils.hpp"

#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>

// Boost Python
#include <boost/python/call.hpp>
#include <boost/python/object.hpp>
#include <boost/python/import.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/make_function.hpp>
#include <boost/bind.hpp>


// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceCount_overloads, terrama2::services::analysis::core::occurrence::count, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMin_overloads, terrama2::services::analysis::core::occurrence::min, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMax_overloads, terrama2::services::analysis::core::occurrence::max, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMean_overloads, terrama2::services::analysis::core::occurrence::mean, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMedian_overloads, terrama2::services::analysis::core::occurrence::median, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceSum_overloads, terrama2::services::analysis::core::occurrence::sum, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::standardDeviation, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationCount_overloads, terrama2::services::analysis::core::occurrence::aggregation::count, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMin_overloads, terrama2::services::analysis::core::occurrence::aggregation::min, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMax_overloads, terrama2::services::analysis::core::occurrence::aggregation::max, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMean_overloads, terrama2::services::analysis::core::occurrence::aggregation::mean, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMedian_overloads, terrama2::services::analysis::core::occurrence::aggregation::median, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationSum_overloads, terrama2::services::analysis::core::occurrence::aggregation::sum, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::aggregation::standardDeviation, 6, 7);


terrama2::services::analysis::core::MonitoredObjectContext::MonitoredObjectContext(terrama2::services::analysis::core::DataManagerPtr dataManager, terrama2::services::analysis::core::AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
  : BaseContext(dataManager, analysis, startTime)
{
}

void terrama2::services::analysis::core::MonitoredObjectContext::registerFunctions()
{
  GILLock lock;

  auto oldState = PyThreadState_Swap(mainThreadState_);
  populateNamespace();

  registerOccurrenceFunctions();
  registerDCPFunctions();

  PyThreadState_Swap(oldState);
}

void terrama2::services::analysis::core::MonitoredObjectContext::registerOccurrenceFunctions()
{
  using namespace boost::python;

  // map the occurence namespace to a sub-module
  // make "from terrama2.occurence import <function>" work
  boost::python::object occurrenceModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence"))));
  // make "from terrama2 import occurence" work
  import("terrama2").attr("occurrence") = occurrenceModule;
  // set the current scope to the new sub-module
  scope occurrenceScope = occurrenceModule;

  auto context = std::static_pointer_cast<MonitoredObjectContext>(shared_from_this());

  // export functions inside occurrence namespace
  def("count", terrama2::services::analysis::core::occurrence::count,
      occurrenceCount_overloads(args("dataSeriesName", "buffer", "dateFilter", "restriction"),
                                "Count operator for occurrence"));
  def("min", terrama2::services::analysis::core::occurrence::min,
      occurrenceMin_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Minimum operator for occurrence"));
  def("max", terrama2::services::analysis::core::occurrence::max,
      occurrenceMax_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Maximum operator for occurrence"));
  def("mean", terrama2::services::analysis::core::occurrence::mean,
      occurrenceMean_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Mean operator for occurrence"));
  def("median", terrama2::services::analysis::core::occurrence::median,
      occurrenceMedian_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Median operator for occurrence"));
  def("sum", terrama2::services::analysis::core::occurrence::sum,
      occurrenceSum_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Sum operator for occurrence"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::standardDeviation,
      occurrenceStandardDeviation_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"),
          "Standard deviation operator for occurrence"));

  // Register operations for occurrence.aggregation
  object occurrenceAggregationModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence.aggregation"))));
  // make "from terrama2.occurrence import aggregation" work
  scope().attr("aggregation") = occurrenceAggregationModule;
  // set the current scope to the new sub-module
  scope occurrenceAggregationScope = occurrenceAggregationModule;

  def("count", terrama2::services::analysis::core::occurrence::aggregation::count,
      occurrenceAggregationCount_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"),
          "Count operator for occurrence aggregation"));
  def("min", terrama2::services::analysis::core::occurrence::aggregation::min,
      occurrenceAggregationMin_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Minimum operator for occurrence aggregation"));
  def("max", terrama2::services::analysis::core::occurrence::aggregation::max,
      occurrenceAggregationMax_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Maximum operator for occurrence aggregation"));
  def("mean", terrama2::services::analysis::core::occurrence::aggregation::mean,
      occurrenceAggregationMean_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Mean operator for occurrence aggregation"));
  def("median", terrama2::services::analysis::core::occurrence::aggregation::median,
      occurrenceAggregationMedian_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Median operator for occurrence aggregation"));
  def("sum", terrama2::services::analysis::core::occurrence::aggregation::sum,
      occurrenceAggregationSum_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Sum operator for occurrence aggregation"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::aggregation::standardDeviation,
      occurrenceAggregationStandardDeviation_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"),
          "Standard deviation operator for occurrence aggregation"));
}

void terrama2::services::analysis::core::MonitoredObjectContext::loadMonitoredObject()
{
  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto analysis = getAnalysis();

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
        datasetMap_[key] = dataSeriesContext;
      }
    }
  }
}

void terrama2::services::analysis::core::MonitoredObjectContext::addDCPDataSeries(terrama2::core::DataSeriesPtr dataSeries,
    const std::string& dateFilter, const bool lastValue)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(auto dataset : dataSeries->datasetList)
  {
    if(!exists(dataset->id, dateFilter))
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
  filter.discardAfter = startTime_;

  if(!dateFilter.empty())
  {
    double seconds = terrama2::core::TimeUtils::convertTimeString(dateFilter, "SECOND", "h");

    ldt -= boost::posix_time::seconds(seconds);

    std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
    filter.discardBefore = std::move(titz);
  }

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries, filter);
  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter);

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
    key.dateFilter_ = dateFilter;
    datasetMap_[key] = dataSeriesContext;
  }
}

std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> terrama2::services::analysis::core::MonitoredObjectContext::getContextDataset(const DataSetId datasetId, const std::string& dateFilter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  if(it == datasetMap_.end())
  {
    return std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries>();
  }

  return it->second;
}

bool terrama2::services::analysis::core::MonitoredObjectContext::exists(const DataSetId datasetId, const std::string& dateFilter) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  return it != datasetMap_.end();
}

void terrama2::services::analysis::core::MonitoredObjectContext::addDataSeries(terrama2::core::DataSeriesPtr dataSeries,
    std::shared_ptr<te::gm::Geometry> envelope,
    const std::string& dateFilter, bool createSpatialIndex)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  bool needToAdd = false;
  for(auto dataset : dataSeries->datasetList)
  {
    if(!exists(dataset->id, dateFilter))
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

  filter.discardAfter = startTime_;

  if(!dateFilter.empty())
  {
    double seconds = terrama2::core::TimeUtils::convertTimeString(dateFilter, "SECOND", "h");

    ldt -= boost::posix_time::seconds(seconds);

    std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
    filter.discardBefore = std::move(titz);
  }

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries, filter);
  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesMap = accessor->getSeries(filter);


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
    key.dateFilter_ = dateFilter;
    datasetMap_[key] = dataSeriesContext;
  }
}

void terrama2::services::analysis::core::MonitoredObjectContext::addAttribute(const std::string& attribute)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  attributes_.insert(attribute);
}

void terrama2::services::analysis::core::MonitoredObjectContext::setAnalysisResult(const std::string& geomId, const std::string& attribute, double result)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  auto& attributeMap = analysisResult_[geomId];
  attributeMap[attribute] = result;
}


// Declaration needed for default parameter ids
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMin_overloads, terrama2::services::analysis::core::dcp::min, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMax_overloads, terrama2::services::analysis::core::dcp::max, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMean_overloads, terrama2::services::analysis::core::dcp::mean, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMedian_overloads, terrama2::services::analysis::core::dcp::median, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpSum_overloads, terrama2::services::analysis::core::dcp::sum, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpStandardDeviation_overloads,
                                terrama2::services::analysis::core::dcp::standardDeviation, 3, 4);


void terrama2::services::analysis::core::MonitoredObjectContext::registerDCPFunctions()
{
  using namespace boost::python;

  // map the dcp namespace to a sub-module
  // make "from terrama2.dcp import <function>" work
  boost::python::object dcpModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp"))));
  // make "from terrama2 import dcp" work
  import("terrama2").attr("dcp") = dcpModule;
  // set the current scope to the new sub-module
  scope scpScope = dcpModule;

  // export functions inside dcp namespace
  def("min", terrama2::services::analysis::core::dcp::min,
      dcpMin_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Minimum operator for DCP"));
  def("max", terrama2::services::analysis::core::dcp::max,
      dcpMax_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Maximum operator for DCP"));
  def("mean", terrama2::services::analysis::core::dcp::mean,
      dcpMean_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Mean operator for DCP"));
  def("median", terrama2::services::analysis::core::dcp::median,
      dcpMedian_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Median operator for DCP"));
  def("sum", terrama2::services::analysis::core::dcp::sum,
      dcpSum_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Sum operator for DCP"));
  def("standard_deviation", terrama2::services::analysis::core::dcp::standardDeviation,
      dcpStandardDeviation_overloads(args("dataSeriesName", "attribute", "buffer", "ids"),
                                     "Standard deviation operator for DCP"));
  def("count", terrama2::services::analysis::core::dcp::count);

  // Register operations for dcp.history
  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.history"))));
  // make "from terrama2.dcp import history" work
  scope().attr("history") = dcpHistoryModule;
  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;

  // export functions inside history namespace
  def("min", terrama2::services::analysis::core::dcp::history::min);
  def("max", terrama2::services::analysis::core::dcp::history::max);
  def("mean", terrama2::services::analysis::core::dcp::history::mean);
  def("median", terrama2::services::analysis::core::dcp::history::median);
  def("sum", terrama2::services::analysis::core::dcp::history::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::history::standardDeviation);

}
