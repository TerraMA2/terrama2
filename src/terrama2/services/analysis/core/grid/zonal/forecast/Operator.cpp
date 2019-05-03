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
  \file terrama2/services/analysis/core/grid/zonal/forecast/Operator.cpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/


#include "Operator.hpp"
#include "../Operator.hpp"
#include "../../../ContextManager.hpp"
#include "../../../../../../core/utility/Logger.hpp"
#include "../../../../../../core/utility/Utils.hpp"
#include "../../../../../../core/utility/TimeUtils.hpp"
#include "../../../utility/Verify.hpp"
#include "../history/Operator.hpp"
#include "../Utils.hpp"

//TerraLib
#include <terralib/dataaccess/utils/Utils.h>

int terrama2::services::analysis::core::grid::zonal::forecast::num( const std::string& dataSeriesName,
                                                                    const std::string& dateDiscardAfter,
                                                                    terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::numImpl(dataSeriesName, "0s", dateDiscardAfter, buffer);
}


boost::python::list terrama2::services::analysis::core::grid::zonal::forecast::list(const std::string& dataSeriesName,
                                                                                    const std::string& dateDiscardAfter,
                                                                                    terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::history::listImpl(dataSeriesName, "0s", dateDiscardAfter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl( terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                                                const std::string& dataSeriesName,
                                                                                const std::string& dateDiscardBefore,
                                                                                const std::string& dateDiscardAfter,
                                                                                terrama2::services::analysis::core::Buffer buffer)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;

  try
  {
    terrama2::services::analysis::core::verify::analysisMonitoredObject(analysis);
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
    // In case an error has already occurred, there is nothing to be done
    if(!contextManager.getMessages(cache.analysisHashCode, BaseContext::MessageType::ERROR_MESSAGE).empty())
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

    std::shared_ptr<ContextDataSeries> moDsContext = context->getMonitoredObjectContextDataSeries();
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

    //if it's an invalid geometry, return nan but continue the analysis
    if(!moGeom->isValid())
      return std::nan("");

    auto geomResult = createBuffer(buffer, moGeom);

    auto dataSeries = context->findDataSeries(dataSeriesName);

    terrama2::core::Filter filter;
    filter.discardAfter = context->getStartTime();
    filter.lastValues = std::make_shared<size_t>(1);

    auto seriesMap = context->getSeriesMap(dataSeries->id, filter);

    auto datasets = dataSeries->datasetList;
    for(const auto& dataset : datasets)
    {
      auto syncDataset = seriesMap.at(dataset).syncDataSet;
      if(syncDataset->size() > 1)
      {
        QString errMsg(QObject::tr("Invalid list of raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      auto rasterPos = te::da::GetFirstPropertyPos(syncDataset->dataset().get(), te::dt::RASTER_TYPE);
      auto raster = syncDataset->getRaster(0, rasterPos);

      auto datePos = te::da::GetFirstPropertyPos(syncDataset->dataset().get(), te::dt::DATETIME_TYPE);
      auto rasterDate = syncDataset->getDateTime(0, datePos);
      auto rasterTimestamp = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(rasterDate)->getTimeInstantTZ();

      auto currentTimestamp = context->getStartTime()->getTimeInstantTZ();

      std::vector<double> values;

      {
        geomResult->transform(raster->getSRID());
        //no intersection between the raster and the object geometry
        if(!raster->getExtent()->intersects(*geomResult->getMBR()))
          continue;

        auto timePassed = currentTimestamp.utc_time() - rasterTimestamp.utc_time();
        double secondsPassed = timePassed.total_seconds();

        size_t bandBegin, bandEnd;
        std::tie(bandBegin, bandEnd) = terrama2::services::analysis::core::getBandInterval(dataset, secondsPassed, dateDiscardBefore, dateDiscardAfter);

        // - the band 0 is always blank
        // - The beginning should be before the end
        if(bandBegin <= 0
          || bandBegin > bandEnd
          || bandBegin > raster->getNumberOfBands()
          || bandEnd > raster->getNumberOfBands())
        {
          QString errMsg{QObject::tr("Invalid value of band index.")};
          throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
        }

        std::map<std::pair<int, int>, double> tempValuesMap;
        for(size_t band = bandBegin; band <= bandEnd; ++ band)
        {
          terrama2::core::getRasterValues<double>(geomResult, raster.get(), band, tempValuesMap);
          transform(tempValuesMap.cbegin(), tempValuesMap.cend(), back_inserter(values), [](const std::pair<std::pair<int, int>, double>& val){ return val.second;} );
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
      return std::nan("");

    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return std::nan("");
    }

    return terrama2::services::analysis::core::getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    contextManager.addError(cache.analysisHashCode, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return std::nan("");
  }
  catch(const std::exception& e)
  {
    contextManager.addError(cache.analysisHashCode, e.what());
    return std::nan("");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    contextManager.addError(cache.analysisHashCode, errMsg.toStdString());
    return std::nan("");
  }
}

double terrama2::services::analysis::core::grid::zonal::forecast::min(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MIN, dataSeriesName, "0s", dateDiscardAfter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::max(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MAX, dataSeriesName, "0s", dateDiscardAfter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::mean(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MEAN, dataSeriesName, "0s", dateDiscardAfter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::median(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, "0s", dateDiscardAfter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::standardDeviation(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, "0s", dateDiscardAfter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::variance(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, "0s", dateDiscardAfter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::sum(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::SUM, dataSeriesName, "0s", dateDiscardAfter, buffer);
}
