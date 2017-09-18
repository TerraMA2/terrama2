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
  \file terrama2/services/analysis/core/dcp/DCPOperator.hpp

  \brief Contains DCP analysis operators.

  \author Paulo R. M. Oliveira
*/



#include "Operator.hpp"
#include "../utility/Utils.hpp"
#include "../utility/Verify.hpp"
#include "../Exception.hpp"
#include "../ContextManager.hpp"
#include "../python/PythonUtils.hpp"
#include "../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../core/data-model/Filter.hpp"
#include "../../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../../core/Shared.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/GeoUtils.hpp"
#include "zonal/influence/Operator.hpp"
#include "zonal/Operator.hpp"
#include "../MonitoredObjectContext.hpp"

// QT
#include <QObject>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/UnitsOfMeasureManager.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>

#include <cmath>
#include <algorithm>
#include <boost/range/algorithm_ext/for_each.hpp>
#include <boost/range/join.hpp>

using namespace boost::python;

double terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation statisticOperation,
    const std::string& attribute,
    boost::python::list pcds,
    const std::string& dateFilterBegin,
    const std::string& dateFilterEnd)
{
  ///////////////////////////////////////////////////////////////
  // check analysis

  // A DCP attribute must be given
  if(attribute.empty())
    return std::nan("");

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::services::analysis::core::verify::analysisDCP(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return std::nan("");
  }

  auto monitoredObjectContext = contextManager.getMonitoredObjectContext(cache.analysisHashCode);
  auto dataManagerPtr = monitoredObjectContext->getDataManager().lock();
  auto moDsContext = monitoredObjectContext->getMonitoredObjectContextDataSeries(dataManagerPtr);

  for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      // found monitored dataseries

      auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
      if(pcds.is_none() || (boost::python::len(pcds) == 0))
      {
        // if no dcp was given as argument use current
        terrama2::core::SynchronizedDataSetPtr syncDs = moDsContext->series.syncDataSet;
        auto dataSetId = moDsContext->series.syncDataSet->getInt32(cache.index, "id");

        auto datasets = dataSeriesPtr->datasetList;
        auto dataset = std::find_if(datasets.begin(), datasets.end(), [&dataSetId](const terrama2::core::DataSetPtr& dataset){ return dataSetId == dataset->id; });
        auto dataSetDcp = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(*dataset);
        if(!dataSetDcp)
        {
          QString errMsg(QObject::tr("Invalid dataset for data series: %1").arg(QString::fromStdString(dataSeriesPtr->name)));
          throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
        }

        auto alias = dataSetDcp->alias();
        pcds.append(alias);
      }

      return zonal::operatorImpl(statisticOperation, dataSeriesPtr->name, attribute, pcds, dateFilterBegin, dateFilterEnd);
    }
  }

  contextManager.addError(cache.analysisHashCode, QObject::tr("Unable to find DCP monitored object for analysis %1.").arg(analysis->id).toStdString());
  return std::nan("");
}

double terrama2::services::analysis::core::dcp::value(const std::string& attribute)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::services::analysis::core::verify::analysisDCP(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return std::nan("");
  }

  auto monitoredObjectContext = contextManager.getMonitoredObjectContext(cache.analysisHashCode);
  auto dataManagerPtr = monitoredObjectContext->getDataManager().lock();
  auto moDsContext = monitoredObjectContext->getMonitoredObjectContextDataSeries(dataManagerPtr);
  if(!moDsContext || !moDsContext->series.teDataSetType)
  {
    QString errMsg(QObject::tr("Could not recover monitored object data series."));
    throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
  }

  // end recover operation data
  ////////////////////////////////

  terrama2::core::SynchronizedDataSetPtr syncDs = moDsContext->series.syncDataSet;
  auto dataSetId = moDsContext->series.syncDataSet->getInt32(cache.index, "id");

  for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      // found monitored dataseries
      auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

      auto datasets = dataSeriesPtr->datasetList;
      auto dataset = std::find_if(datasets.begin(), datasets.end(), [&dataSetId](const terrama2::core::DataSetPtr& dataset){ return dataSetId == dataset->id; });
      auto dataSetDcp = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(*dataset);
      if(!dataSetDcp)
      {
        QString errMsg(QObject::tr("Invalid dataset for data series: %1").arg(QString::fromStdString(dataSeriesPtr->name)));
        throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
      }

      auto alias = dataSetDcp->alias();
      boost::python::list pcds;
      pcds.append(alias);

      return zonal::operatorImpl(StatisticOperation::MAX, dataSeriesPtr->name, attribute, pcds);
    }
  }

  contextManager.addError(cache.analysisHashCode, QObject::tr("Unable to find DCP monitored object for analysis %1.").arg(analysis->id).toStdString());
  return std::nan("");
}

int terrama2::services::analysis::core::dcp::count(Buffer buffer)
{
  return static_cast<int>(influence::byRule(buffer).size());
}

double terrama2::services::analysis::core::dcp::min(const std::string& attribute, boost::python::list ids)
{
  if(ids.is_none())
  {
    auto all_dcps = influence::all(true);
    std::for_each(all_dcps.begin(), all_dcps.end(), [&ids](const std::string& dcp_name){ids.append(boost::python::object(dcp_name));});
  }
  return operatorImpl(StatisticOperation::MIN, attribute, ids);
}

double terrama2::services::analysis::core::dcp::max(const std::string& attribute,
    boost::python::list ids)
{
  if(ids.is_none())
  {
    auto all_dcps = influence::all(true);
    std::for_each(all_dcps.begin(), all_dcps.end(), [&ids](const std::string& dcp_name){ids.append(boost::python::object(dcp_name));});
  }
  return operatorImpl(StatisticOperation::MAX, attribute, ids);
}

double terrama2::services::analysis::core::dcp::mean(const std::string& attribute,
    boost::python::list ids)
{
  if(ids.is_none())
  {
    auto all_dcps = influence::all(true);
    std::for_each(all_dcps.begin(), all_dcps.end(), [&ids](const std::string& dcp_name){ids.append(boost::python::object(dcp_name));});
  }
  return operatorImpl(StatisticOperation::MEAN, attribute, ids);
}

double terrama2::services::analysis::core::dcp::median(const std::string& attribute,
    boost::python::list ids)
{
  if(ids.is_none())
  {
    auto all_dcps = influence::all(true);
    std::for_each(all_dcps.begin(), all_dcps.end(), [&ids](const std::string& dcp_name){ids.append(boost::python::object(dcp_name));});
  }
  return operatorImpl(StatisticOperation::MEDIAN, attribute, ids);
}

double terrama2::services::analysis::core::dcp::sum(const std::string& attribute,
    boost::python::list ids)
{
  if(ids.is_none())
  {
    auto all_dcps = influence::all(true);
    std::for_each(all_dcps.begin(), all_dcps.end(), [&ids](const std::string& dcp_name){ids.append(boost::python::object(dcp_name));});
  }
  return operatorImpl(StatisticOperation::SUM, attribute, ids);
}

double terrama2::services::analysis::core::dcp::standardDeviation(const std::string& attribute,
                                                                  boost::python::list ids)
{
  if(ids.is_none())
  {
    auto all_dcps = influence::all(true);
    std::for_each(all_dcps.begin(), all_dcps.end(), [&ids](const std::string& dcp_name){ids.append(boost::python::object(dcp_name));});
  }
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, attribute, ids);
}

double terrama2::services::analysis::core::dcp::variance(const std::string& attribute,
                                                         boost::python::list ids)
{
  if(ids.is_none())
  {
    auto all_dcps = influence::all(true);
    std::for_each(all_dcps.begin(), all_dcps.end(), [&ids](const std::string& dcp_name){ids.append(boost::python::object(dcp_name));});
  }
  return operatorImpl(StatisticOperation::VARIANCE, attribute, ids);
}

boost::python::list terrama2::services::analysis::core::dcp::influence::python::byRule(const terrama2::services::analysis::core::Buffer& buffer)
{
  auto vecDCP = influence::byRule(buffer);

  boost::python::list pyList;
  for(const auto& dcp : vecDCP)
  {
    pyList.append(boost::python::object(dcp));
  }
  return pyList;
}

std::vector< std::string > terrama2::services::analysis::core::dcp::influence::byRule(const terrama2::services::analysis::core::Buffer& buffer)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::services::analysis::core::verify::analysisDCP(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return {};
  }

  auto monitoredObjectContext = contextManager.getMonitoredObjectContext(cache.analysisHashCode);
  auto dataManagerPtr = monitoredObjectContext->getDataManager().lock();

  for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
      return zonal::influence::byRule(dataSeriesPtr->name, buffer);
    }
  }

  contextManager.addError(cache.analysisHashCode, QObject::tr("Unable to find DCP monitored object for analysis %1.").arg(analysis->id).toStdString());
  return {};
}

std::vector< std::string > terrama2::services::analysis::core::dcp::influence::all(bool isActive)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::services::analysis::core::verify::analysisDCP(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return {};
  }

  auto monitoredObjectContext = contextManager.getMonitoredObjectContext(cache.analysisHashCode);
  auto dataManagerPtr = monitoredObjectContext->getDataManager().lock();

  for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      std::vector< std::string > vecDCP;
      auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
      for(const auto& dataset : dataSeriesPtr->datasetList)
      {
        auto dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
        if(!dcpDataset)
        {
          QString errMsg(QObject::tr("Invalid dataset for data series: ").arg(QString::fromStdString(dataSeriesPtr->name)));
          throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
        }

        if(isActive && dcpDataset->active)//only add active DCP
          vecDCP.push_back(dcpDataset->alias());
        else
          vecDCP.push_back(dcpDataset->alias());
      }

      return vecDCP;
    }
  }

  contextManager.addError(cache.analysisHashCode, QObject::tr("Unable to find DCP monitored object for analysis %1.").arg(analysis->id).toStdString());
  return {};
}
