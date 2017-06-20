
#include "BitsetIntersection.hpp"
#include "GeoUtils.hpp"

#include <terralib/rp/Functions.h>
#include <terralib/geometry/Utils.h>

#include <limits>

terrama2::core::BitsetIntersection::BitsetIntersection(const te::gm::Coord2D& lowerLeftPixel,
                                                       const te::gm::Coord2D& upperRightPixel,
                                                       boost::dynamic_bitset<>&& bitset,
                                                       const double& xResolution,
                                                       const double& yResolution)
  : lowerLeftPixel_(lowerLeftPixel),
    upperRightPixel_(upperRightPixel),
    bitset_(std::move(bitset)),
    xResolution_(xResolution),
    yResolution_(yResolution)
{

}

terrama2::core::BitsetIntersection::~BitsetIntersection()
{
  bitset_.reset();
}

terrama2::core::BitsetIntersection terrama2::core::BitsetIntersection::bitsetIntersection(std::shared_ptr<te::gm::Geometry> geometry, std::shared_ptr<te::rst::Raster> raster)
{
  geometry->transform(raster->getSRID());
  auto extent = geometry->getMBR();

  auto lowerLeftCoord = extent->getLowerLeft();
  auto upperRightCoord = extent->getUpperRight();

  auto grid = raster->getGrid();

  uint32_t lowerLeftCol, lowerLeftRow;
  std::tie(lowerLeftCol, lowerLeftRow) = terrama2::core::geoToGrid(lowerLeftCoord, grid);
  if(lowerLeftCol == std::numeric_limits<uint32_t>::max()) lowerLeftCol = 0;
  if(lowerLeftRow == std::numeric_limits<uint32_t>::max()) lowerLeftRow = grid->getNumberOfRows();

  uint32_t upperRightCol, upperRightRow;
  std::tie(upperRightCol, upperRightRow) = terrama2::core::geoToGrid(upperRightCoord, grid);
  if(upperRightRow == std::numeric_limits<uint32_t>::max()) upperRightRow = 0;
  if(upperRightCol == std::numeric_limits<uint32_t>::max()) upperRightCol = grid->getNumberOfColumns();

  auto xResolution = grid->getResolutionX();
  auto yResolution = grid->getResolutionY();

  boost::dynamic_bitset<> bitset((lowerLeftRow-upperRightRow+1)*(upperRightCol-lowerLeftCol+1));
  size_t i = 0;
  for(uint32_t row = upperRightRow; row <= lowerLeftRow; ++row)
  {
    for(uint32_t col = lowerLeftCol; col <= upperRightCol; ++col)
    {
      auto pixelCenter = grid->gridToGeo(col, row);
      std::shared_ptr<te::gm::Envelope> envelope = std::make_shared<te::gm::Envelope>(pixelCenter.getX()-(xResolution/2.),
                                                                                      pixelCenter.getY()-(yResolution/2.),
                                                                                      pixelCenter.getX()+(xResolution/2.),
                                                                                      pixelCenter.getY()+(yResolution/2.));
      std::shared_ptr<te::gm::Geometry> envelopeGeom(te::gm::GetGeomFromEnvelope(envelope.get(), raster->getSRID()));

      if(envelopeGeom->intersects(geometry.get()))
        bitset[i] = 1;
      ++i;
    }
  }

  terrama2::core::BitsetIntersection intersection(grid->gridToGeo(lowerLeftCol, lowerLeftRow),
                                                  grid->gridToGeo(upperRightCol, upperRightRow),
                                                  std::move(bitset),
                                                  xResolution,
                                                  yResolution);
  return intersection;
}
