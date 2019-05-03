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
  \file terrama2/core/BitsetIntersection.hpp

  \brief Class to hold the intersection of a geometry and a raster

  \author Jano Simas
*/

// Boost
#include <boost/dynamic_bitset.hpp>
#include <terralib/geometry/Coord2D.h>
// TerraLib
#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/Coord2D.h>
#include <terralib/raster/Raster.h>
// STL
#include <memory>

// TerraMa2
#include "../Config.hpp"

namespace te {
namespace gm {
class Geometry;
}  // namespace gm
namespace rst {
class Raster;
}  // namespace rst
}  // namespace te

#ifndef __TERRAMA2_CORE_UTILS_BITSET_INTERSECTION_HPP__
#define __TERRAMA2_CORE_UTILS_BITSET_INTERSECTION_HPP__

namespace terrama2 {
  namespace core {
    /*!
      \brief Class to hold the intersection of a geometry and a raster

      This class holds a bitset of the intersecting pixel of a geometry and a raster.
      Each value of the bitset is a bool representig if a pixel intersects the geometry.
      The values are ordinated from top-left to lower-right

      Verify the x and y resolution using applying at another raster.

      ## Usage:

      Get the col-row pair of the first and last fixel using the lowerLeftPixel
      and upperRightPixel.

      Iterating from top-left to lower-right pixel check if the bitset[i] is true.

      \warning Verify the x and y resolution using applying at another raster.

      \sa terrama2::core::geoToGrid
    */
    class TMCOREEXPORT BitsetIntersection
    {
      public:
        /*!
          \brief Constructor.

          \param lowerLeftPixel Coordinate of the lower left pixel of the intersection of the geometry envelope.
          \param upperRightPixel Coordinate of the upper roght pixel of the intersection of the geometry envelope.
          \param bitset Bitset of the intersection.
          \param xResolution X resolution of the raster.
          \param yResolution Y resolution of the raster.
        */
        BitsetIntersection(const te::gm::Coord2D& lowerLeftPixel,
                          const te::gm::Coord2D& upperRightPixel,
                          boost::dynamic_bitset<>&& bitset,
                          const double& xResolution,
                          const double& yResolution);

        /*!
          \brief Destructor.

          Resets the bitset.
        */
        ~BitsetIntersection();

        /*!
          \brief Creates a BitsetIntersection of the geometry and the raster
        */
        static BitsetIntersection bitsetIntersection(std::shared_ptr<te::gm::Geometry> geometry, te::rst::Raster* raster);

        //! Coordinate of the lower left pixel of the intersection of the geometry envelope.
        te::gm::Coord2D lowerLeft() const { return lowerLeftPixel_; }
        //! Coordinate of the upper roght pixel of the intersection of the geometry envelope.
        te::gm::Coord2D upperRight() const { return upperRightPixel_; }
        //! Bitset of the intersection.
        boost::dynamic_bitset<> bitset() const { return boost::dynamic_bitset<>(bitset_); }
        //! X resolution of the raster.
        double xResolution() const { return xResolution_; }
        //! Y resolution of the raster.
        double yResolution() const { return yResolution_; }

      private:
        te::gm::Coord2D lowerLeftPixel_; //!< Coordinate of the lower left pixel of the intersection of the geometry envelope.
        te::gm::Coord2D upperRightPixel_; //!< Coordinate of the upper roght pixel of the intersection of the geometry envelope.
        boost::dynamic_bitset<> bitset_; //!< Bitset of the intersection.

        double xResolution_; //!< X resolution of the raster.
        double yResolution_; //!< Y resolution of the raster.
    };
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_UTILS_BITSET_INTERSECTION_HPP__
