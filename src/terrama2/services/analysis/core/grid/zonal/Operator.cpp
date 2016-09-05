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
#include "../../Utils.hpp"
#include "../../PythonInterpreter.hpp"
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

void terrama2::services::analysis::core::grid::zonal::appendValues(te::rst::Raster* raster, te::gm::Polygon* polygon, std::vector<double>& values)
{
  //raster values can always be read as double
  auto it = te::rst::PolygonIterator<double>::begin(raster, polygon);
  auto end = te::rst::PolygonIterator<double>::end(raster, polygon);

  //there are pixels inside the object
  if(it != end)
  {
    for(; it != end; ++it)
    {
      //FIXME: getting from first band
      values.push_back(it[0]);
    }
  }
}

double terrama2::services::analysis::core::grid::zonal::operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
    const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // Inside Py_BEGIN_ALLOW_THREADS it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    auto context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return NAN;
  }


  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return NAN;
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

    std::string geomId = moDsContext->series.syncDataSet->getString(cache.index, moDsContext->identifier);

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
    for(auto dataset : datasets)
    {
      auto rasterList = context->getRasterList(dataSeries, dataset->id, dateDiscardBefore, dateDiscardAfter);

      //sanity check, if no date range only the last raster should be returned
      if(dateDiscardBefore.empty() && rasterList.size() > 1)
      {
        //FIXME: should not happen, throw?
        assert(0);
      }

      if(rasterList.empty())
      {
        QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      std::vector<double> values;
      for(auto raster : rasterList)
      {
        geomResult->transform(raster->getSRID());
        //no intersection between the raster and the object geometry
        if(!raster->getExtent()->intersects(*geomResult->getMBR()))
          continue;


        //TODO: check for other valid types
        auto type = geomResult->getGeomTypeId();
        if(type == te::gm::PolygonType)
        {
          auto polygon = std::static_pointer_cast<te::gm::Polygon>(geomResult);
          appendValues(raster.get(), polygon.get(), values);
        }
        else if(type == te::gm::MultiPolygonType)
        {
          auto multiPolygon = std::static_pointer_cast<te::gm::MultiPolygon>(geomResult);
          for(auto geom : multiPolygon->getGeometries())
          {
            auto polygon = static_cast<te::gm::Polygon*>(geom);
            appendValues(raster.get(), polygon, values);
          }
        }
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
      return NAN;

    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return NAN;
    }

    return terrama2::services::analysis::core::getOperationResult(cache, statisticOperation);
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

double terrama2::services::analysis::core::grid::zonal::count(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::COUNT, dataSeriesName, "", "", buffer);
}


double terrama2::services::analysis::core::grid::zonal::min(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, "", "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::max(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, "", "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::mean(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, "", "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::median(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, "", "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::standardDeviation(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, "", "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::variance(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, "", "", buffer);
}

double terrama2::services::analysis::core::grid::zonal::sum(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, "", "", buffer);
}
