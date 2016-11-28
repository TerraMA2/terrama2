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
  \file terrama2/services/analysis/core/grid/zonal/Operator.cpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/



#include "Operator.hpp"
#include "Utils.hpp"
#include "../../utility/Utils.hpp"
#include "../../utility/Verify.hpp"
#include "../../python/PythonInterpreter.hpp"
#include "../../ContextManager.hpp"
#include "../../MonitoredObjectContext.hpp"

#include <QTextStream>

#include "../../../../../core/data-model/Filter.hpp"
#include "../../../../../core/utility/Logger.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/geometry/Utils.h>
#include <terralib/raster/PositionIterator.h>

double terrama2::services::analysis::core::grid::zonal::operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                                     const std::string& dataSeriesName,
                                                                     const size_t band,
                                                                     terrama2::services::analysis::core::Buffer buffer)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;

  try
  {
    terrama2::core::verify::analysisMonitoredObject(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return std::nan("");
  }

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return std::nan("");
  }

  terrama2::core::Filter filter;
  filter.lastValue = true;

  return operatorImpl(statisticOperation, dataSeriesName, filter, band, buffer, context, cache);
}

double terrama2::services::analysis::core::grid::zonal::operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                                    const std::string& dataSeriesName,
                                                                    const terrama2::core::Filter& filter,
                                                                    const size_t band,
                                                                    terrama2::services::analysis::core::Buffer buffer,
                                                                    terrama2::services::analysis::core::MonitoredObjectContextPtr context,
                                                                    OperatorCache cache)
{

  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;
  try
  {
    // In case an error has already occurred, there is nothing to do.
    if(context->hasError())
    {
      return std::nan("");
    }

    bool hasData = false;

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    std::shared_ptr<ContextDataSeries> moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    if(moDsContext->series.syncDataSet->size() == 0)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto moGeom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
    if(!moGeom.get())
    {
      QString errMsg(QObject::tr("Could not recover monitored object geometry."));
      throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
    }
    auto geomResult = createBuffer(buffer, moGeom);

    auto dataSeries = context->findDataSeries(dataSeriesName);
    if(!dataSeries)
    {
      QString errMsg(QObject::tr("Could not find a data series with the given name: %1"));
      errMsg = errMsg.arg(QString::fromStdString(dataSeriesName));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto datasets = dataSeries->datasetList;
    for(const auto& dataset : datasets)
    {
      auto rasterList = context->getRasterList(dataSeries, dataset->id, filter);

      //sanity check, if no date range only the last raster should be returned
      if(!filter.discardBefore && rasterList.size() > 1)
      {
        QString errMsg(QObject::tr("Invalid list of raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      if(rasterList.empty())
      {
        QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      std::vector<double> values;
      for(const auto& raster : rasterList)
      {
        geomResult->transform(raster->getSRID());
        //no intersection between the raster and the object geometry
        if(!raster->getExtent()->intersects(*geomResult->getMBR()))
          continue;

        std::map<std::pair<int, int>, double> tempValuesMap;
        utils::getRasterValues<double>(geomResult.get(), raster, band, tempValuesMap);

        transform(tempValuesMap.cbegin(), tempValuesMap.cend(), back_inserter(values), [](const std::pair<std::pair<int, int>, double>& val){ return val.second;} );
      }

      if(!values.empty())
      {
        terrama2::services::analysis::core::calculateStatistics(values, cache);
        hasData = true;
      }

      if(hasData)
        break;
    }

    if(exceptionOccurred)
      return std::nan("");

    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return std::nan("");
    }

    return terrama2::services::analysis::core::getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return std::nan("");
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    return std::nan("");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return std::nan("");
  }
}

double terrama2::services::analysis::core::grid::zonal::count(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::COUNT, dataSeriesName, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::min(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::max(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::mean(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::median(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::standardDeviation(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::variance(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::sum(const std::string& dataSeriesName, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, band, buffer);
}
