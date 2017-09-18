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
  \file terrama2/services/analysis/core/grid/zonal/Utils.hpp

  \brief Zonal utils

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_UTILS_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_UTILS_HPP__

// STL
#include <string>
#include <vector>
#include <memory>
#include <map>

#include <QObject>

#include <terralib/raster/PositionIterator.h>

#include "../../Exception.hpp"
#include "../../../../../core/utility/BitsetIntersection.hpp"
#include "../../../../../core/utility/GeoUtils.hpp"

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace grid
        {
          namespace zonal
          {
            namespace utils
            {
              /*!
                \brief Get a map of <row, column> of values of the raster intersection with the geometry
              */
              template<class T>
              void getRasterValues(std::shared_ptr<te::gm::Geometry> geom,
                                   const std::shared_ptr<te::rst::Raster>& raster,
                                   const size_t band,
                                   std::map<std::pair<int, int>, T>& valuesMap);

              /*!
                \brief Get a map of <row, column> of the <accumulated values, count of values> of the raster intersection with the geometry
              */
              template<class T>
              std::unordered_map<std::pair<int, int>, std::pair<T, int>, boost::hash<std::pair<int, int> > >
              getAccumulatedMap(const std::string &dataSeriesName, const std::string &dateDiscardBefore,
                                const std::string &dateDiscardAfter, const size_t band,
                                terrama2::services::analysis::core::Buffer buffer,
                                terrama2::services::analysis::core::MonitoredObjectContextPtr context,
                                terrama2::services::analysis::core::OperatorCache cache);

            } /* utils */
          } /* zonal */
        } /* grid */
      } /* core */
    } /* analysis */
  } /* services */
} /* terrama2 */

template<class T>
void terrama2::services::analysis::core::grid::zonal::utils::getRasterValues(std::shared_ptr<te::gm::Geometry> geom,
                      const std::shared_ptr<te::rst::Raster>& raster,
                      const size_t band,
                      std::map<std::pair<int, int>, T>& valuesMap)
{
  terrama2::core::BitsetIntersection intersection = terrama2::core::BitsetIntersection::bitsetIntersection(geom, raster);

  auto grid = raster->getGrid();
  uint32_t lowerLeftCol, lowerLeftRow;
  std::tie(lowerLeftCol, lowerLeftRow) = terrama2::core::geoToGrid(intersection.lowerLeft(), grid);
  if(lowerLeftCol == std::numeric_limits<uint32_t>::max()) lowerLeftCol = 0;
  if(lowerLeftRow == std::numeric_limits<uint32_t>::max()) lowerLeftRow = grid->getNumberOfRows()-1;

  uint32_t upperRightCol, upperRightRow;
  std::tie(upperRightCol, upperRightRow) = terrama2::core::geoToGrid(intersection.upperRight(), grid);
  if(upperRightRow == std::numeric_limits<uint32_t>::max()) upperRightRow = 0;
  if(upperRightCol == std::numeric_limits<uint32_t>::max()) upperRightCol = grid->getNumberOfColumns()-1;

  auto rasterBand = raster->getBand(band);
  double noData = rasterBand->getProperty()->m_noDataValue;
  auto bitset = intersection.bitset();
  uint32_t i = 0;
  for(uint32_t row = upperRightRow; row <= lowerLeftRow; ++row)
  {
    for(uint32_t column = lowerLeftCol; column <= upperRightCol; ++column)
    {
      if(bitset[i])
      {
        auto key = std::make_pair(row, column);
        double value = noData;
        rasterBand->getValue(column, row, value);
        if(value != noData)
          valuesMap[key] = value;
      }
      ++i;
    }
  }
}

template<class T>
std::unordered_map<std::pair<int, int>, std::pair<T, int>, boost::hash<std::pair<int, int> > >
terrama2::services::analysis::core::grid::zonal::utils::getAccumulatedMap(
                  const std::string &dataSeriesName, const std::string &dateDiscardBefore,
                  const std::string &dateDiscardAfter, const size_t band,
                  terrama2::services::analysis::core::Buffer buffer,
                  terrama2::services::analysis::core::MonitoredObjectContextPtr context,
                  terrama2::services::analysis::core::OperatorCache cache)
{
  auto dataManagerPtr = context->getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  std::shared_ptr<ContextDataSeries> moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
  if(!moDsContext || moDsContext->series.syncDataSet->size() == 0)
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

  std::unordered_map<std::pair<int, int>, std::pair<T, int>, boost::hash<std::pair<int, int> > > valuesMap;

  //if it's an invalid geometry, return nan but continue the analysis
  if(!moGeom->isValid())
    return valuesMap;

  auto geomResult = createBuffer(buffer, moGeom);

  auto dataSeries = context->findDataSeries(dataSeriesName);

  /////////////////////////////////////////////////////////////////
  //map of sum of values for each pixel

  terrama2::core::Filter filter;
  filter.discardBefore = context->getTimeFromString(dateDiscardBefore);
  filter.discardAfter = context->getTimeFromString(dateDiscardAfter);

  auto datasets = dataSeries->datasetList;
  for(const auto& dataset : datasets)
  {
    auto rasterList = context->getRasterList(dataSeries, dataset->id, filter);
    //sanity check, if no date range only the last raster should be returned
    if(!filter.discardBefore && rasterList.size() > 1)
    {
      QString errMsg(QObject::tr("Invalid list of raster for dataset: %1").arg(dataset->id));
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    if(rasterList.empty())
    {
      QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(dataset->id));
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    auto firstRaster = rasterList.front();

    //no intersection between the raster and the object geometry
    if(!firstRaster->getExtent()->intersects(*geomResult->getMBR()))
      continue;

    geomResult->transform(firstRaster->getSRID());
    for(const auto& raster : rasterList)
    {
      std::map<std::pair<int, int>, T> tempValuesMap;
      utils::getRasterValues<T>(geomResult, raster, band, tempValuesMap);

      for_each(tempValuesMap.cbegin(), tempValuesMap.cend(), [&valuesMap](const std::pair<std::pair<int, int>, T>& val)
      {
        try
        {
          auto& it = valuesMap.at(val.first);
          it.first += val.second;
          it.second++;
        }
        catch (...)
        {
          valuesMap[val.first] = std::make_pair(val.second, 1);
        }
      });
    }

    if(!valuesMap.empty())
      break;
  }

  return valuesMap;

}

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_UTILS_HPP__
