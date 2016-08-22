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
#include "../../ContextManager.hpp"
#include "../../../../../core/data-model/DataSetGrid.hpp"
#include "../../Utils.hpp"

// TerraLib
#include <terralib/raster/Grid.h>
#include <terralib/raster/Reprojection.h>

// STD
#include <numeric>

std::vector<double> terrama2::services::analysis::core::grid::history::sample(const OperatorCache& cache, const std::string& dataSeriesName, const std::string& dateFilterBegin,
const std::string& dateFilterEnd)
{
  auto context = ContextManager::getInstance().getGridContext(cache.analysisHashCode);

  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return {};
    }

    auto dataSeries = context->findDataSeries(dataSeriesName);
    if(!dataSeries)
    {
      QString errMsg(QObject::tr("Could not find a data series with the given name: %1"));
      errMsg = errMsg.arg(QString::fromStdString(dataSeriesName));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto outputRaster = context->getOutputRaster();
    if(!outputRaster)
    {
      QString errMsg(QObject::tr("Invalid output raster"));
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    auto grid = outputRaster->getGrid();
    auto coord = grid->gridToGeo(cache.column, cache.row);

    auto datasets = dataSeries->datasetList;
    for(auto dataset : datasets)
    {

      auto rasterList = context->getRasterList(dataSeries, dataset->id, dateFilterBegin, dateFilterEnd);
      if(rasterList.empty())
      {
        QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      std::vector<double> samples;
      for(const auto& raster : rasterList)
      {
        auto dsGrid = raster->getGrid();
        if(!dsGrid)
        {
          QString errMsg(QObject::tr("Invalid grid for dataset: %1").arg(dataset->id));
          throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
        }

        // Tranform the coordinate from the output srid to the  source srid
        // so we can get the row and column of the source data.
        auto point = context->convertoTo(coord, dsGrid->getSRID());

        double column, row;
        dsGrid->geoToGrid(point.x, point.y, column, row);

        if(!grid->isPointInGrid(column, row))
          break;

        double value;
        raster->getValue(column, row, value);

        samples.push_back(value);
      }

      if(!samples.empty())
        return samples;
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

double terrama2::services::analysis::core::grid::history::operatorImpl(
  terrama2::services::analysis::core::StatisticOperation statisticOperation,
  const std::string& dataSeriesName, const std::string& dateFilterBegin,
  const std::string& dateFilterEnd)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // Inside Py_BEGIN_ALLOW_THREADS it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;
  auto context = ContextManager::getInstance().getGridContext(cache.analysisHashCode);

  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return NAN;
    }

    bool hasData = false;

    // Save thread state before entering multi-thread zone
    Py_BEGIN_ALLOW_THREADS

    try
    {
      auto samples = sample(cache, dataSeriesName, dateFilterBegin, dateFilterEnd);

      hasData = !samples.empty();

      if(hasData)
        terrama2::services::analysis::core::calculateStatistics(samples, cache);
      else if(statisticOperation == StatisticOperation::COUNT)
        return 0.;
    }
    catch(...)
    {
      exceptionOccurred = true;
    }

    // All operations are done, acquires the GIL and set the return value
    Py_END_ALLOW_THREADS

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

double terrama2::services::analysis::core::grid::history::min(const std::string& dataSeriesName, const std::string& dateFilter)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, dateFilter);
}

double terrama2::services::analysis::core::grid::history::max(const std::string& dataSeriesName, const std::string& dateFilter)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, dateFilter);
}

double terrama2::services::analysis::core::grid::history::mean(const std::string& dataSeriesName, const std::string& dateFilter)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, dateFilter);
}

double terrama2::services::analysis::core::grid::history::median(const std::string& dataSeriesName, const std::string& dateFilter)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, dateFilter);
}

double terrama2::services::analysis::core::grid::history::standardDeviation(const std::string& dataSeriesName, const std::string& dateFilter)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, dateFilter);
}

double terrama2::services::analysis::core::grid::history::variance(const std::string& dataSeriesName, const std::string& dateFilter)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, dateFilter);
}

double terrama2::services::analysis::core::grid::history::sum(const std::string& dataSeriesName, const std::string& dateFilter)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, dateFilter);
}
