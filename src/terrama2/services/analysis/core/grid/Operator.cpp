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
#include "../../../../core/data-model/DataSetGrid.hpp"
#include "../Utils.hpp"

#include <terralib/raster/Grid.h>
#include <terralib/raster/Reprojection.h>

double terrama2::services::analysis::core::grid::sample(const std::string& dataSeriesName)
{
  OperatorCache cache;

  try
  {
    readInfoFromDict(cache);

    // In case an error has already occurred, there is nothing to be done
    if(!Context::getInstance().getErrors(cache.analysisHashCode).empty())
    {
      return NAN;
    }

    auto dataManagerPtr = Context::getInstance().getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    AnalysisPtr analysis = Context::getInstance().getAnalysis(cache.analysisHashCode);

    //FIXME: PAULO: This is beeing called to many times, can this be avoided?
    // ps: O(n) processing time to get the name, if the id would be used O(c) can be achieved
    auto dataSeries = dataManagerPtr->findDataSeries(analysis->id, dataSeriesName);

    if(!dataSeries)
    {
      QString errMsg(QObject::tr("Could not find a data series with the given name: %1"));
      errMsg = errMsg.arg(QString::fromStdString(dataSeriesName));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto outputRaster = Context::getInstance().getOutputRaster(cache.analysisHashCode);
    if(!outputRaster)
    {
      QString errMsg(QObject::tr("Invalid output raster"));
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    auto outputGridConfig = Context::getInstance().getOutputRasterInfo(dataManagerPtr, cache.analysisHashCode);

    auto grid = outputRaster->getGrid();
    auto coord = grid->gridToGeo(cache.column, cache.row);


    auto datasets = dataSeries->datasetList;

    for(auto dataset : datasets)
    {

      auto raster = Context::getInstance().getRaster(cache.analysisHashCode, dataset->id);

      if(!raster)
      {
        // First call, need to call sample for each dataset raster and store the result in the context.
        auto gridMap = Context::getInstance().getGridMap(dataManagerPtr, dataSeries->id, cache.analysisHashCode);

        if(gridMap.empty())
        {
          continue;
        }

        auto it = gridMap.begin();
        while(it != gridMap.end())
        {
          auto datasetGrid = it->first;
          auto dsRaster = it->second;

          if(!dsRaster)
          {
            QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(datasetGrid->id));
            throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
          }

          auto reprojectedRaster = reprojectRaster(dsRaster, outputGridConfig, analysis->outputGridPtr->interpolationMethod);

          Context::getInstance().addRaster(cache.analysisHashCode, dataset->id, reprojectedRaster);
          it++;
        }
      }

      raster = Context::getInstance().getRaster(cache.analysisHashCode, dataset->id);

      if(!raster)
      {
        QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      auto dsGrid = raster->getGrid();

      if(!dsGrid)
      {
        QString errMsg(QObject::tr("Invalid grid for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      // Tranform the coordinate from the output srid to the  source srid
      // so we can get the row and column of the source data.
      te::gm::Point point(te::gm::PointType, grid->getSRID());
      point.setX(coord.getX());
      point.setY(coord.getY());
      //FIXME: PAULO: This a VERY cost operation, every raster's grid should be in the same srid before this, so it's never called

      // ps: instantiate a map of transformations from output raster to source raster?
      // this woud cut the cost of creating a transformation matrix every time
      point.transform(dsGrid->getSRID());

      double column, row;
      dsGrid->geoToGrid(point.getX(), point.getY(), column, row);

      if(!grid->isPointInGrid(column, row))
      {
        continue;
      }

      double value;
      raster->getValue(column, row, value);

      return value;
    }

    return NAN;
  }
  catch(terrama2::Exception e)
  {
    Context::getInstance().addError(cache.analysisHashCode,  boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return NAN;
  }
  catch(std::exception e)
  {
    Context::getInstance().addError(cache.analysisHashCode, e.what());
    return NAN;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    Context::getInstance().addError(cache.analysisHashCode, errMsg.toStdString());
    return NAN;
  }
}
