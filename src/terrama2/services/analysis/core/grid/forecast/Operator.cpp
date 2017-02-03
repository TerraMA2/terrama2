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
  \file terrama2/services/analysis/core/grid/Operator.cpp

  \brief Contains grid analysis operators.

  \author Paulo R. M. Oliveira
  \author Jano Simas
*/

// TerraMA2
#include "Operator.hpp"
#include "../Operator.hpp"
#include "../../ContextManager.hpp"
#include "../../utility/Verify.hpp"
#include "../../../../../core/utility/Logger.hpp"

//TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/raster/Grid.h>

double terrama2::services::analysis::core::grid::forecast::operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                                        const std::string& dataSeriesName,
                                                                        const std::string& dateFilterBegin,
                                                                        const std::string& dateFilterEnd)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  auto& contextManager = ContextManager::getInstance();
  auto analysis = contextManager.getAnalysis(cache.analysisHashCode);
  try
  {
    verify::analysisGrid(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return {};
  }

  terrama2::services::analysis::core::GridContextPtr context;
  try
  {
    context = ContextManager::getInstance().getGridContext(cache.analysisHashCode);
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

    auto dataSeries = context->findDataSeries(dataSeriesName);

    auto outputRaster = context->getOutputRaster();
    if(!outputRaster)
    {
      QString errMsg(QObject::tr("Invalid output raster"));
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    auto grid = outputRaster->getGrid();
    auto coord = grid->gridToGeo(cache.column, cache.row);

    terrama2::core::Filter filter;
    filter.lastValue = true;
    filter.discardAfter = context->getStartTime();

    auto seriesMap = context->getSeriesMap(dataSeries->id, filter);

    auto datasets = dataSeries->datasetList;
    assert(datasets.size() == 1);
    auto dataset = datasets.front();

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
    auto timePassed = currentTimestamp.utc_time() - rasterTimestamp.utc_time();
    double secondsPassed = timePassed.total_seconds();

    int bandBegin, bandEnd;
    std::tie(bandBegin, bandEnd) = terrama2::services::analysis::core::getBandInterval(dataset, secondsPassed, dateFilterBegin, dateFilterEnd);

    // - the band 0 is always blank
    // - The begining should be before the end
    if(bandBegin == 0 || bandBegin > bandEnd)
    {
      QString errMsg{QObject::tr("Invalid value of band index.")};
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    std::vector<double> samples;
    samples.reserve(bandEnd-bandBegin+1);

    auto dsGrid = raster->getGrid();
    if(!dsGrid)
    {
      QString errMsg(QObject::tr("Invalid grid for dataset: %1").arg(dataset->id));
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    // Transform the coordinate from the output srid to the  source srid
    // so we can get the row and column of the source data.
    auto point = context->convertoTo(coord, dsGrid->getSRID());

    double column, row;
    dsGrid->geoToGrid(point.x, point.y, column, row);

    auto interpolator = context->getInterpolator(raster);

    for(int band = bandBegin; band <= bandEnd; ++band)
    {
      double value = terrama2::services::analysis::core::grid::getValue(raster, interpolator, column, row, band);
      samples.push_back(value);
    }


    if(samples.empty() && statisticOperation != StatisticOperation::COUNT)
    {
      return std::nan("");
    }

    return terrama2::services::analysis::core::getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return std::nan("");
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    return std::nan("");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return std::nan("");
  }
}

double terrama2::services::analysis::core::grid::forecast::min(const std::string& dataSeriesName, const std::string& dateFilterEnd)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, "0s", dateFilterEnd);
}

double terrama2::services::analysis::core::grid::forecast::max(const std::string& dataSeriesName, const std::string& dateFilterEnd)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, "0s", dateFilterEnd);
}

double terrama2::services::analysis::core::grid::forecast::mean(const std::string& dataSeriesName, const std::string& dateFilterEnd)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, "0s", dateFilterEnd);
}

double terrama2::services::analysis::core::grid::forecast::median(const std::string& dataSeriesName, const std::string& dateFilterEnd)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, "0s", dateFilterEnd);
}

double terrama2::services::analysis::core::grid::forecast::standardDeviation(const std::string& dataSeriesName, const std::string& dateFilterEnd)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, "0s", dateFilterEnd);
}

double terrama2::services::analysis::core::grid::forecast::variance(const std::string& dataSeriesName, const std::string& dateFilterEnd)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, "0s", dateFilterEnd);
}

double terrama2::services::analysis::core::grid::forecast::sum(const std::string& dataSeriesName, const std::string& dateFilterEnd)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, "0s", dateFilterEnd);
}
