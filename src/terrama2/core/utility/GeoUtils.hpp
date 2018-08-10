
#ifndef __TERRAMA2_CORE_UTILITY_GEOUTILS_HPP__
#define __TERRAMA2_CORE_UTILITY_GEOUTILS_HPP__

#include <terralib/geometry/Coord2D.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/sam/rtree/Index.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/Raster.h>
#include <memory>
#include <string>
#include <utility>

#include "../Config.hpp"
#include "../data-access/DataSetSeries.hpp"

namespace te {
namespace gm {
class Geometry;
struct Coord2D;
}  // namespace gm
namespace rst {
class Grid;
class Raster;
}  // namespace rst
}  // namespace te

namespace terrama2 {
  namespace core {
    struct DataSetSeries;

    /*!
      \brief Returns the SRID of a UTM projection based on the zone of given coordinate.
    */
    TMCOREEXPORT int getUTMSrid(te::gm::Geometry* geom);

    /*!
      \brief Converts the distance from the given unit to the target unit.
    */
    TMCOREEXPORT double convertDistanceUnit(double distance, const std::string& fromUnit, const std::string& targetUnit);

    /*!
      \brief Constructs a te::gm::Geometry from a EWKT (Postgis WKT with projection information).
    */
    TMCOREEXPORT std::shared_ptr<te::gm::Geometry> ewktToGeom(const std::string& ewkt);

    /*!
      \brief Create an expansible raster from another raster.

      The content will be copied by block.
    */
    TMCOREEXPORT std::unique_ptr<te::rst::Raster> cloneRaster(const te::rst::Raster& raster);


    /*!
     * \brief Create an expansible raster from another raster.
              Each value of raster will be multiplied by multiplier parameter.
     * \param raster The raster to copy
     * \param multiplier The value to multiply the raster values
     * \return An expansible raster
     */
    TMCOREEXPORT std::unique_ptr<te::rst::Raster> multiplyRaster(const te::rst::Raster& raster, const double& multiplier);

    /*
      \brief Convert the coordinate to the grid col/row postion of the pixel .

      If the coordinate is outside the grid extent the function will
      return std::numeric_limits<uint32_t>::max for col and/or row the is outside.
      The other value is valid.

      \return Pair of <col, row> of the pixel at the coord.
    */
    TMCOREEXPORT std::pair<uint32_t, uint32_t> geoToGrid(const te::gm::Coord2D& coord, te::rst::Grid* grid);

    /*
        \brief creates an RTree from the geometrict property of the DataSetSeries

        \warning The RTree return is in EPSG:4326
    */
    TMCOREEXPORT std::unique_ptr<te::sam::rtree::Index<size_t, 8> > createRTreeFromSeries(const terrama2::core::DataSetSeries& dataSetSeries);
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_UTILITY_GEOUTILS_HPP__
