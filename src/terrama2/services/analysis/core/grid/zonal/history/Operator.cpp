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
  \file terrama2/services/analysis/core/grid/zonal/history/Operator.cpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/


#include "Operator.hpp"
#include "../Operator.hpp"
#include "../../../ContextManager.hpp"
#include "../../../../../../core/utility/Logger.hpp"
#include "../../../utility/Verify.hpp"

//TerraLib
#include <terralib/dataaccess/utils/Utils.h>

int terrama2::services::analysis::core::grid::zonal::history::num(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.

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


  try
  {
    // In case an error has already occurred, there is nothing to do.
    if(context->hasError())
    {
      return std::nan("");
    }

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

    terrama2::core::Filter filter;
    filter.discardBefore = context->getTimeFromString(dateDiscardBefore);

    int count = 0;
    auto datasets = dataSeries->datasetList;
    for(auto dataset : datasets)
    {
      auto rasterList = context->getRasterList(dataSeries, dataset->id, filter);

      for (auto raster : rasterList)
      {
        auto extent = raster->getExtent();
        if(!extent->intersects(*geomResult->getMBR()))
          continue;

        ++count;
      }
    }

    return count;
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

boost::python::list terrama2::services::analysis::core::grid::zonal::history::list(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;

  try
  {
    terrama2::core::verify::analysisMonitoredObject(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return {};
  }

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return {};
  }

  try
  {
    // In case an error has already occurred, there is nothing to do.
    if(context->hasError())
    {
      return {};
    }

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

    terrama2::core::Filter filter;
    filter.discardBefore = context->getTimeFromString(dateDiscardBefore);
    filter.discardAfter = context->getTimeFromString("0s");

    auto seriesList = context->getSeriesMap(dataSeries->id, filter);
    for(auto pair : seriesList)
    {
      auto series = pair.second;
      auto teDataset = series.syncDataSet->dataset();
      if(!teDataset->isEmpty())
      {
        std::size_t rasterColumn = te::da::GetFirstPropertyPos(teDataset.get(), te::dt::RASTER_TYPE);
        if(rasterColumn == std::numeric_limits<size_t>::max())
          continue;

        std::size_t timestampColumn = te::da::GetFirstPropertyPos(teDataset.get(), te::dt::DATETIME_TYPE);
        //no date column found
        if(timestampColumn == std::numeric_limits<size_t>::max())
          continue;

        boost::python::list timeList;
        teDataset->moveBeforeFirst();
        while(teDataset->moveNext())
        {
          if(teDataset->isNull(rasterColumn) || teDataset->isNull(timestampColumn))
            continue;

          auto raster = teDataset->getRaster(rasterColumn);
          auto extent = raster->getExtent();
          if(!extent->intersects(*geomResult->getMBR()))
            continue;

          timeList.append(boost::python::object(teDataset->getDateTime(timestampColumn)->toString()));
        }

        return timeList;
      }
    }

    return {};
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return {};
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    return {};
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return {};
  }
}

double terrama2::services::analysis::core::grid::zonal::history::operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                                              const std::string& dataSeriesName,
                                                                              const std::string& dateDiscardBefore,
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
  filter.discardBefore = context->getTimeFromString(dateDiscardBefore);

  return terrama2::services::analysis::core::grid::zonal::operatorImpl(statisticOperation, dataSeriesName, filter, band, buffer, context, cache);
}

double terrama2::services::analysis::core::grid::zonal::history::min(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::operatorImpl(StatisticOperation::MIN, dataSeriesName, dateDiscardBefore, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::max(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::operatorImpl(StatisticOperation::MAX, dataSeriesName, dateDiscardBefore, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::mean(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::operatorImpl(StatisticOperation::MEAN, dataSeriesName, dateDiscardBefore, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::median(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, dateDiscardBefore, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::standardDeviation(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, dateDiscardBefore, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::variance(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, dateDiscardBefore, band, buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::sum(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::operatorImpl(StatisticOperation::SUM, dataSeriesName, dateDiscardBefore, band, buffer);
}
