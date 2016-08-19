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
#include "../Utils.hpp"
#include "../GridContext.hpp"
#include "../PythonInterpreter.hpp"

#include <terralib/raster/Grid.h>
#include <terralib/raster/Reprojection.h>

double terrama2::services::analysis::core::grid::sample(const std::string& dataSeriesName)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  auto context = ContextManager::getInstance().getGridContext(cache.analysisHashCode);

  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return NAN;
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

      auto rasterList = context->getRasterList(dataSeries, dataset->id);
      if(rasterList.size() > 1)
      {
        //FIXME: should not happen, throw?
        assert(0);
      }

      if(rasterList.empty())
      {
        QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      auto raster = rasterList.front();
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

      int icol = static_cast<int>(std::round(column));
      int irow = static_cast<int>(std::round(row));
      if(!dsGrid->isPointInGrid(icol, irow))
        continue;

      double value;
      raster->getValue(icol, irow, value);

      return value;
    }

    return NAN;
  }
  catch(terrama2::Exception e)
  {
    context->addError(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return NAN;
  }
  catch(std::exception e)
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
