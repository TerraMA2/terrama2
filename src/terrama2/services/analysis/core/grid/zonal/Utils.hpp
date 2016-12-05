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
              struct mapValueIsLesser
              {
                  template<class U, class T>
                  bool operator()(const std::pair<U, T>& left, const std::pair<U, T>& right) const
                  {
                      return left.second < right.second;
                  }
              };

              template<class T>
              void getRasterValues( te::gm::Geometry* geom,
                                    const std::shared_ptr<te::rst::Raster>& raster,
                                    const size_t band,
                                    std::map<std::pair<int, int>, T>& valuesMap);

              template<class T>
              void getRasterValues( te::gm::Polygon* polygon,
                                    const std::shared_ptr<te::rst::Raster>& raster,
                                    const size_t band,
                                    std::map<std::pair<int, int>, T>& valuesMap);

              template<class T>
              void getRasterValues( std::vector<te::gm::Point*> pointSet,
                                    const std::shared_ptr<te::rst::Raster>& raster,
                                    const size_t band,
                                    std::map<std::pair<int, int>, T>& valuesMap);

              template<class T>
              void getRasterValues( te::gm::Line* Line,
                                    const std::shared_ptr<te::rst::Raster>& raster,
                                    const size_t band,
                                    std::map<std::pair<int, int>, T>& valuesMap);

              template<template<class> class U, class T>
              void getRasterValues(U<T>& rasterIt,
                                   U<T>& end,
                                   const size_t band,
                                   std::map<std::pair<int, int>, T>& valuesMap);

            } /* utils */
          }
        }
      }
    }
  }
}

template<class T>
void terrama2::services::analysis::core::grid::zonal::utils::getRasterValues( te::gm::Geometry* geom,
                      const std::shared_ptr<te::rst::Raster>& raster,
                      const size_t band,
                      std::map<std::pair<int, int>, T>& valuesMap)
{
  //TODO: check for other valid types
  auto type = geom->getGeomTypeId();

  switch(type)
  {
    case te::gm::PolygonType:
    {
      auto polygon = static_cast<te::gm::Polygon*>(geom);
      getRasterValues(polygon, raster, band, valuesMap);
      break;
    }
    case te::gm::MultiPolygonType:
    {
      auto multiPolygon = static_cast<te::gm::MultiPolygon*>(geom);
      for(auto geom : multiPolygon->getGeometries())
      {
        auto polygon = static_cast<te::gm::Polygon*>(geom);
        getRasterValues(polygon, raster, band, valuesMap);
      }
      break;
    }
    case te::gm::LineStringType:
    {
      auto line = static_cast<te::gm::Line*>(geom);
      getRasterValues(line, raster, band, valuesMap);
      break;
    }
    case te::gm::MultiLineStringType:
    {
      auto multiLine = static_cast<te::gm::MultiLineString*>(geom);
      for(auto geom : multiLine->getGeometries())
      {
        auto line = static_cast<te::gm::Line*>(geom);
        getRasterValues(line, raster, band, valuesMap);
      }
      break;
    }
    case te::gm::PointType:
    {
      auto point = static_cast<te::gm::Point*>(geom);
      getRasterValues({point}, raster, band, valuesMap);
      break;
    }
    case te::gm::MultiPointType:
    {
      auto multiPoint = static_cast<te::gm::MultiPoint*>(geom);
      std::vector<te::gm::Point*> pointSet;
      for(auto geom : multiPoint->getGeometries())
      {
        auto point = static_cast<te::gm::Point*>(geom);
        pointSet.push_back(point);
      }

      getRasterValues(pointSet, raster, band, valuesMap);
      break;
    }
    default:
      QString errMsg = QObject::tr("Unrecognized geometry type: %1").arg(type);
      throw InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }
}

template<class T>
void terrama2::services::analysis::core::grid::zonal::utils::getRasterValues( te::gm::Polygon* polygon,
                      const std::shared_ptr<te::rst::Raster>& raster,
                      const size_t band,
                      std::map<std::pair<int, int>, T>& valuesMap)
{
  //raster values can always be read as double
  auto rasterIt = te::rst::PolygonIterator<T>::begin(raster.get(), polygon);
  auto end = te::rst::PolygonIterator<T>::end(raster.get(), polygon);

  getRasterValues(rasterIt, end, band, valuesMap);
}

template<class T>
void terrama2::services::analysis::core::grid::zonal::utils::getRasterValues( std::vector<te::gm::Point*> pointSet,
                      const std::shared_ptr<te::rst::Raster>& raster,
                      const size_t band,
                      std::map<std::pair<int, int>, T>& valuesMap)
{
  //raster values can always be read as double
  auto rasterIt = te::rst::PointSetIterator<T>::begin(raster.get(), pointSet);
  auto end = te::rst::PointSetIterator<T>::end(raster.get(), pointSet);

  getRasterValues(rasterIt, end, band, valuesMap);
}

template<class T>
void terrama2::services::analysis::core::grid::zonal::utils::getRasterValues( te::gm::Line* line,
                      const std::shared_ptr<te::rst::Raster>& raster,
                      const size_t band,
                      std::map<std::pair<int, int>, T>& valuesMap)
{
  //raster values can always be read as double
  auto rasterIt = te::rst::LineIterator<T>::begin(raster.get(), line);
  auto end = te::rst::LineIterator<T>::end(raster.get(), line);

  getRasterValues(rasterIt, end, band, valuesMap);
}

template<template<class> class U, class T>
void terrama2::services::analysis::core::grid::zonal::utils::getRasterValues(
                     U<T>& rasterIt,
                     U<T>& end,
                     const size_t band,
                     std::map<std::pair<int, int>, T>& valuesMap)
{
  for(; rasterIt != end; ++rasterIt)
  {
    auto column = rasterIt.getColumn();
    auto row = rasterIt.getRow();

    auto key = std::make_pair(row, column);
    valuesMap[key] = rasterIt[band];
  }
}


#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_UTILS_HPP__
