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
  \file terrama2/services/analysis/core/grid/zonal/history/ratio/Operator.cpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/


#include "Operator.hpp"
#include "../Operator.hpp"
#include "../../../ContextManager.hpp"

//TerraLib
#include <terralib/dataaccess/utils/Utils.h>

int terrama2::services::analysis::core::grid::zonal::history::num(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // Inside Py_BEGIN_ALLOW_THREADS it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  auto context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);

  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return NAN;
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

    int count = 0;
    auto datasets = dataSeries->datasetList;
    for(auto dataset : datasets)
    {
      auto rasterList = context->getRasterList(dataSeries, dataset->id, dateDiscardBefore, "");

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
    context->addError(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return NAN;
  }
  catch(const std::exception& e)
  {
    context->addError(e.what());
    return NAN;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addError(errMsg.toStdString());
    return NAN;
  }
}

boost::python::list terrama2::services::analysis::core::grid::zonal::history::list(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // Inside Py_BEGIN_ALLOW_THREADS it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  auto context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);

  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
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

    auto seriesList = context->getSeriesMap(dataSeries->id, dateDiscardBefore, "0s");
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
    context->addError(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return {};
  }
  catch(const std::exception& e)
  {
    context->addError(e.what());
    return {};
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addError(errMsg.toStdString());
    return {};
  }
}

double terrama2::services::analysis::core::grid::zonal::history::count(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::COUNT, dataSeriesName, dateDiscardBefore, "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::min(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::MIN, dataSeriesName, dateDiscardBefore, "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::max(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::MAX, dataSeriesName, dateDiscardBefore, "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::mean(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::MEAN, dataSeriesName, dateDiscardBefore, "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::median(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, dateDiscardBefore, "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::standardDeviation(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, dateDiscardBefore, "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::variance(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, dateDiscardBefore, "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::history::sum(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::operatorImpl(StatisticOperation::SUM, dataSeriesName, dateDiscardBefore, "", buffer);
}
