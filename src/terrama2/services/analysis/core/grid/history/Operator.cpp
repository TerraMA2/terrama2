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
#include "../../../../../core/data-model/DataSetGrid.hpp"
#include "../../../../../core/utility/Logger.hpp"
#include "../../utility/Utils.hpp"
#include "../../utility/Verify.hpp"

// TerraLib
#include <terralib/raster/Grid.h>
#include <terralib/raster/Reprojection.h>

// STD
#include <numeric>


std::vector<double> terrama2::services::analysis::core::grid::history::sample(const OperatorCache& cache,
                                                                              const std::string& dataSeriesName,
                                                                              const std::string& dateFilterBegin,
                                                                              const std::string& dateFilterEnd,
                                                                              const size_t band)
{
  auto& contextManager = ContextManager::getInstance();
  auto analysis = contextManager.getAnalysis(cache.analysisHashCode);
  try
  {
    terrama2::core::verify::analysisGrid(analysis);
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

    terrama2::core::Filter filter;
    filter.discardBefore = context->getTimeFromString(dateFilterBegin);
    filter.discardAfter = context->getTimeFromString(dateFilterEnd);

    auto datasets = dataSeries->datasetList;
    for(auto dataset : datasets)
    {

      auto rasterList = context->getRasterList(dataSeries, dataset->id, filter);
      if(rasterList.empty())
      {
        QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      std::vector<double> samples;
      samples.reserve(rasterList.size());
      for(const auto& raster : rasterList)
      {
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

        double value = getValue(raster, interpolator, column, row, band);

        samples.push_back(value);
      }

      if(!samples.empty())
        return samples;
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

double terrama2::services::analysis::core::grid::history::operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                                       const std::string& dataSeriesName,
                                                                       const std::string& dateFilterBegin,
                                                                       const std::string& dateFilterEnd,
                                                                       const size_t band)
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
    terrama2::core::verify::analysisGrid(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return std::nan("");
  }

  terrama2::services::analysis::core::GridContextPtr context;
  try
  {
    context = ContextManager::getInstance().getGridContext(cache.analysisHashCode);
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

    bool hasData = false;

    // Frees the GIL, from now on it's not allowed to return any value because it doesn't have the interpreter lock.
    // In case an exception is thrown, we need to catch it and set a flag.
    // Once the code left the lock is acquired we should return NAN.

    {
      terrama2::services::analysis::core::python::OperatorLock operatorLock;


      try
      {
        std::vector<double> samples = sample(cache, dataSeriesName, dateFilterBegin, dateFilterEnd, band);

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

double terrama2::services::analysis::core::grid::history::min(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, dateFilter, "", band);
}

double terrama2::services::analysis::core::grid::history::max(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, dateFilter, "", band);
}

double terrama2::services::analysis::core::grid::history::mean(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, dateFilter, "", band);
}

double terrama2::services::analysis::core::grid::history::median(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, dateFilter, "", band);
}

double terrama2::services::analysis::core::grid::history::standardDeviation(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, dateFilter, "", band);
}

double terrama2::services::analysis::core::grid::history::variance(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, dateFilter, "", band);
}

double terrama2::services::analysis::core::grid::history::sum(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, dateFilter, "", band);
}
