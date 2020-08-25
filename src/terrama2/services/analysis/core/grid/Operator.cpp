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
*/

#include "Operator.hpp"
#include "../ContextManager.hpp"
#include "../../../../core/data-model/DataSetGrid.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/FilterUtils.hpp"
#include "../utility/Utils.hpp"
#include "../utility/Verify.hpp"
#include "../GridContext.hpp"
#include "../python/PythonInterpreter.hpp"

#include <terralib/raster/Band.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/Reprojection.h>

double terrama2::services::analysis::core::grid::getValue(std::shared_ptr<te::rst::Raster> raster,
                                                          std::shared_ptr<terrama2::core::SynchronizedInterpolator> interpolator,
                                                          double column,
                                                          double row,
                                                          size_t bandIdx)
{
  std::complex<double> val;
  interpolator->getValue(column, row, val, bandIdx);
  auto band = raster->getBand(bandIdx);

  double noData = band->getProperty()->m_noDataValue;
  double value =  val.real();
  if(value == noData)
    return std::nan("");
  else
    return value;
}

double terrama2::services::analysis::core::grid::sample(const std::string& dataSeriesName, size_t bandIdx, const std::string& date)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;  

  try
  {
    terrama2::services::analysis::core::verify::analysisGrid(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return std::nan("");
  }

  terrama2::services::analysis::core::GridContextPtr context;
  try
  {
    context = contextManager.getGridContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    contextManager.addError(cache.analysisHashCode, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return std::nan("");
  }

  try
  {
    // In case an error has already occurred, there is nothing to do.
    if(context->hasError())
    {
      return std::nan("");
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

    if (analysis->schedule.reprocessingHistoricalData != nullptr)
    {
      auto startTime = context->getStartTime();

      if(date.compare("") != 0)
      {
        filter.discardAfter = terrama2::core::gridSampleVerifyMask(date, context->getStartTime());
      }
      else
      {
        filter.discardAfter = startTime;
      }

      filter.lastValues = std::make_shared<std::size_t>(1);
      filter.isReprocessingHistoricalData = true;
    }
    else
    {
      if(date.compare("") != 0)
      {
        filter.discardAfter = terrama2::core::gridSampleVerifyMask(date, context->getStartTime());
      }
      filter.lastValues = std::make_shared<size_t>(1);
    }

    auto datasets = dataSeries->datasetList;
    for(const auto& dataset : datasets)
    {
      auto rasterList = context->getRasterList(dataSeries, dataset->id, filter);
      //sanity check, if no date range only the last raster should be returned
      if(rasterList.size() > 1)
      {
        QString errMsg(QObject::tr("Invalid list of raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      if(rasterList.empty())
      {
        QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      auto raster = rasterList.front();
      if(bandIdx >= raster->getNumberOfBands())
      {
        QString errMsg(QObject::tr("Invalid band index for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      auto interpolator = context->getInterpolator(raster);
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

      return getValue(raster, interpolator, column, row, bandIdx);
    }

    return std::nan("");
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
