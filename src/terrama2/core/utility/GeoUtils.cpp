
#include "GeoUtils.hpp"
#include "../Exception.hpp"
#include "Logger.hpp"

#include <terralib/common/UnitsOfMeasureManager.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>
#include <terralib/geometry/WKTReader.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/Band.h>
#include <terralib/raster/BandIterator.h>

#include <QObject>
#include <QString>

int terrama2::core::getUTMSrid(te::gm::Geometry* geom)
{
  te::gm::Coord2D coord = geom->getCentroid();

  // Calculates the UTM zone for the given coordinate
  int zoneNumber = floor((coord.getX() + 180)/6) + 1;

  if(coord.getY() >= 56.0 && coord.getY() < 64.0 && coord.getX() >= 3.0 && coord.getX() < 12.0)
    zoneNumber = 32;

  // Special zones for Svalbard
  if(coord.getY() >= 72.0 && coord.getY() < 84.0)
  {
    if(coord.getX() >= 0.0  && coord.getX() <  9.0)
      zoneNumber = 31;
    else if(coord.getX() >= 9.0  && coord.getX() < 21.0)
      zoneNumber = 33;
    else if(coord.getX() >= 21.0 && coord.getX() < 33.0)
      zoneNumber = 35;
    else if(coord.getX() >= 33.0 && coord.getX() < 42.0)
      zoneNumber = 37;
  }

  // Creates a Proj4 description and returns the SRID.
  std::string p4txt = "+proj=utm +zone=" + std::to_string(zoneNumber) + " +datum=WGS84 +units=m +no_defs ";

  try
  {
    auto srsPair = te::srs::SpatialReferenceSystemManager::getInstance().getIdFromP4Txt(p4txt);
    return srsPair.second;
  }
  catch(const std::exception& e)
  {
    QString msg(QObject::tr("Could not determine the SRID for a UTM projection"));
    throw InvalidSRIDException() << terrama2::ErrorDescription(msg);
  }
}

double terrama2::core::convertDistanceUnit(double distance, const std::string& fromUnit, const std::string& targetUnit)
{
  auto it = te::common::UnitsOfMeasureManager::getInstance().begin();

  if(it == te::common::UnitsOfMeasureManager::getInstance().end())
  {
    QString msg(QObject::tr("There is no UnitOfMeasure registered."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InitializationException() << terrama2::ErrorDescription(msg);
  }

  return te::common::UnitsOfMeasureManager::getInstance().getConversion(fromUnit, targetUnit) * distance;

}

std::shared_ptr<te::gm::Geometry> terrama2::core::ewktToGeom(const std::string& ewkt)
{
  auto pos = ewkt.find(";")+1;
  auto wkt = ewkt.substr(pos);
  auto sridStr = ewkt.substr(0, pos-1);
  pos = sridStr.find("=")+1;
  int srid = std::stoi(sridStr.substr(pos));

  auto geom = std::shared_ptr<te::gm::Geometry>(te::gm::WKTReader::read(wkt.c_str()));
  geom->setSRID(srid);

  return geom;
}

std::unique_ptr<te::rst::Raster> terrama2::core::cloneRaster(const te::rst::Raster& raster)
{
  std::vector<te::rst::BandProperty*> bands;
  for(size_t i = 0; i < raster.getNumberOfBands(); ++i)
  {
    bands.push_back(new te::rst::BandProperty(*raster.getBand(i)->getProperty()));
  }
  std::unique_ptr<te::gm::Envelope> envelope(new te::gm::Envelope(*raster.getExtent()));
  std::unique_ptr<te::rst::Grid> grid(new te::rst::Grid(raster.getNumberOfColumns(), raster.getNumberOfRows(), envelope.release(), raster.getSRID()));
  std::unique_ptr<te::rst::Raster> expansible(te::rst::RasterFactory::make("EXPANSIBLE", grid.release(), bands, {}));

  for(uint bandIdx = 0; bandIdx < raster.getNumberOfBands(); ++bandIdx)
  {
    const te::rst::Band* rasterBand = raster.getBand(bandIdx);
    te::rst::Band* expansibleBand = expansible->getBand(bandIdx);

    const int nblocksX = rasterBand->getProperty()->m_nblocksx;
    const int nblocksY = rasterBand->getProperty()->m_nblocksy;
    int blkYIdx = 0;

    for( int blkXIdx = 0 ; blkXIdx < nblocksX ; ++blkXIdx )
    {
      for( blkYIdx = 0 ; blkYIdx < nblocksY ; ++blkYIdx )
      {
        std::unique_ptr<unsigned char[]> buffer(new unsigned char[rasterBand->getBlockSize()]);
        rasterBand->read( blkXIdx, blkYIdx, buffer.get());
        expansibleBand->write( blkXIdx, blkYIdx, buffer.get());
      }
    }
  }

  return expansible;
}

std::unique_ptr<te::rst::Raster> terrama2::core::multiplyRaster(const te::rst::Raster& raster, const double& multiplier)
{
  std::vector<te::rst::BandProperty*> bands;
  for(size_t i = 0; i < raster.getNumberOfBands(); ++i)
  {
    te::rst::BandProperty* bProp = new te::rst::BandProperty(*raster.getBand(i)->getProperty());

    bProp->m_type = te::dt::DOUBLE_TYPE;

    bands.push_back(bProp);
  }

  std::unique_ptr<te::gm::Envelope> envelope(new te::gm::Envelope(*raster.getExtent()));
  std::unique_ptr<te::rst::Grid> grid(new te::rst::Grid(raster.getNumberOfColumns(), raster.getNumberOfRows(), envelope.release(), raster.getSRID()));
  std::unique_ptr<te::rst::Raster> expansible(te::rst::RasterFactory::make("EXPANSIBLE", grid.release(), bands, {}));

  int columns = raster.getNumberOfColumns();
  int rows = raster.getNumberOfRows();

  for(uint bandIdx = 0; bandIdx < raster.getNumberOfBands(); ++bandIdx)
  {
    const te::rst::Band* rasterBand = raster.getBand(bandIdx);
    const auto bandProperty = rasterBand->getProperty();
    auto noData = bandProperty->m_noDataValue;
    te::rst::Band* expansibleBand = expansible->getBand(bandIdx);

    for(int col = 0 ; col < columns ; col++)
    {
      for(int row = 0 ; row < rows ; row++)
      {
        double value = 0.0;
        rasterBand->getValue(col, row, value);
        if(value != noData)
          expansibleBand->setValue(col, row, value * multiplier);
        else
          expansibleBand->setValue(col, row, noData);
      }
    }
  }

  return expansible;
}

std::pair<uint32_t, uint32_t> terrama2::core::geoToGrid(const te::gm::Coord2D& coord, te::rst::Grid* grid)
{
  double colD=-1, rowD=-1;
  grid->geoToGrid(coord.getX(), coord.getY(), colD, rowD);
  int col = std::round(colD);
  int row = std::round(rowD);

  if(col < 0 || col >= grid->getNumberOfColumns())
    col = std::numeric_limits<uint32_t>::max();
  if(row < 0 || row >= grid->getNumberOfRows())
    row = std::numeric_limits<uint32_t>::max();

  return std::make_pair(col, row);
}
