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
  \file terrama2/core/utility/JSonUtils.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_UTILS_GEO_UTILS_HPP__
#define __TERRAMA2_CORE_UTILS_GEO_UTILS_HPP__

#include <string>

#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/Coord2D.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/Grid.h>

#include "../data-access/DataSetSeries.hpp"

namespace terrama2 {
  namespace core {
    /*!
      \brief Returns the SRID of a UTM projection based on the zone of given coordinate.
    */
    int getUTMSrid(te::gm::Geometry* geom);

    /*!
      \brief Converts the distance from the given unit to the target unit.
    */
    double convertDistanceUnit(double distance, const std::string& fromUnit, const std::string& targetUnit);

    /*!
      \brief Constructs a te::gm::Geometry from a EWKT (Postgis WKT with projection information).
    */
    std::shared_ptr<te::gm::Geometry> ewktToGeom(const std::string& ewkt);

    /*!
      \brief Create an expansible raster from another raster.

      The content will be copied by block.
    */
    std::unique_ptr<te::rst::Raster> cloneRaster(const te::rst::Raster& raster);


    /*!
     * \brief Create an expansible raster from another raster.
              Each value of raster will be multiplied by multiplier parameter.
     * \param raster The raster to copy
     * \param multiplier The value to multiply the raster values
     * \return An expansible raster
     */
    std::unique_ptr<te::rst::Raster> multiplyRaster(const te::rst::Raster& raster, const double& multiplier);

    /*
      \brief Convert the coordinate to the grid col/row postion of the pixel .

      If the coordinate is outside the grid extent the function will
      return std::numeric_limits<uint32_t>::max for col and/or row the is outside.
      The other value is valid.

      \return Pair of <col, row> of the pixel at the coord.
    */
    std::pair<uint32_t, uint32_t> geoToGrid(const te::gm::Coord2D& coord, te::rst::Grid* grid);

    /*
        \brief creates an RTree from the geometrict property of the DataSetSeries

        \warning The RTree return is in EPSG:4326
    */
    std::unique_ptr<te::sam::rtree::Index<size_t, 8> > createRTreeFromSeries(const terrama2::core::DataSetSeries& dataSetSeries);
  } /* core */
} /* terrama2 */

#endif // __TERRAMA2_CORE_UTILS_GEO_UTILS_HPP__
