
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/Raii.hpp>
#include <terrama2/core/utility/BitsetIntersection.hpp>
#include <terrama2/core/utility/GeoUtils.hpp>

#include <terralib/geometry/Polygon.h>
#include <terralib/geometry/Envelope.h>
#include <terralib/geometry/Utils.h>
#include <terralib/geometry/LinearRing.h>

#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/memory/Raster.h>

#include <terralib/raster/Raster.h>
#include <terralib/raster/Grid.h>
#include <terralib/rp/Functions.h>

#include <boost/dynamic_bitset.hpp>

#include <stdexcept>
#include <memory>
#include <limits>


std::shared_ptr<te::gm::Polygon> createPolygon()
{
  double urx = -43.35449;
  double ury = -24.59051;
  double llx = -48.92983;
  double lly = -24.97189;
  auto e = new te::gm::Envelope(llx, lly, urx, ury);
  // create an outer ring with the same envelope as our envelope
  te::gm::LinearRing* r = new te::gm::LinearRing(5, te::gm::LineStringType, 4326, new te::gm::Envelope(*e));

  r->setPoint(0, e->m_llx, e->m_lly);
  r->setPoint(1, e->m_urx, e->m_lly);
  r->setPoint(2, e->m_urx, e->m_ury);
  r->setPoint(3, e->m_llx, e->m_ury);
  r->setPoint(4, e->m_llx, e->m_lly);

  // create the polygon
  std::shared_ptr<te::gm::Polygon> p = std::make_shared<te::gm::Polygon>(1, te::gm::PolygonType, 4326, new te::gm::Envelope(*e));
  p->setRingN(0, r);

  return p;
}

void highlightRaster(std::shared_ptr<te::rst::Raster> raster, const terrama2::core::BitsetIntersection& intersection)
{
  auto grid = raster->getGrid();
  uint32_t lowerLeftCol, lowerLeftRow;
  std::tie(lowerLeftCol, lowerLeftRow) = terrama2::core::geoToGrid(intersection.lowerLeft(), grid);
  if(lowerLeftCol == std::numeric_limits<uint32_t>::max()) lowerLeftCol = 0;
  if(lowerLeftRow == std::numeric_limits<uint32_t>::max()) lowerLeftRow = grid->getNumberOfRows();

  uint32_t upperRightCol, upperRightRow;
  std::tie(upperRightCol, upperRightRow) = terrama2::core::geoToGrid(intersection.upperRight(), grid);
  if(upperRightRow == std::numeric_limits<uint32_t>::max()) upperRightRow = 0;
  if(upperRightCol == std::numeric_limits<uint32_t>::max()) upperRightCol = grid->getNumberOfColumns();

  auto bitset = intersection.bitset();
  auto band = raster->getBand(0);
  uint32_t i = 0;
  for(uint32_t r = upperRightRow; r <= lowerLeftRow; ++r)
  {
    for(uint32_t c = lowerLeftCol; c <= upperRightCol; ++c)
    {
      if(bitset[i])
        band->setValue(c, r, 1);
      ++i;
    }
  }
}

std::shared_ptr<te::rst::Raster> createRaster()
{
  uint rows=5, cols=5;

  std::vector<te::rst::BandProperty*> bands;
  te::rst::BandProperty* bandProp = new te::rst::BandProperty(0, te::dt::DOUBLE_TYPE);
  bandProp->m_blkh = 1;
  bandProp->m_blkw = cols;
  bandProp->m_nblocksx = 1;
  bandProp->m_nblocksy = rows;
  bandProp->m_noDataValue = std::numeric_limits<double>::max();
  bands.push_back(bandProp);

  auto grid = new te::rst::Grid(cols, rows, new te::gm::Envelope(-53.108193, -25.310889, -39.666462, -14.232851), 4326);
  std::shared_ptr<te::rst::Raster> raster(te::rst::RasterFactory::make("EXPANSIBLE", grid, bands, {}));

  auto band = raster->getBand(0);

  for(size_t r = 0; r < rows; ++r)
  {
    for(size_t c = 0; c < cols; ++c)
    {
      band->setValue(c, r, 0);
    }
  }

  return raster;
}

std::shared_ptr<te::da::DataSet> getDataSet(const std::string& fileUri, const std::string& dataSetName)
{
    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("OGR", fileUri));

    //RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
    std::shared_ptr<te::da::DataSet> dataset(transactor->getDataSet(dataSetName));
    assert(dataset);

    return dataset;
}

template<class T, size_t N>
constexpr size_t size(T (&)[N]) { return N; }

std::unique_ptr<te::rst::Raster> cloneRaster(const te::rst::Raster& raster)
{
  std::vector<te::rst::BandProperty*> bands;
  for(size_t i = 0; i < raster.getNumberOfBands(); ++i)
  {
    bands.push_back(new te::rst::BandProperty(*raster.getBand(i)->getProperty()));
  }
  auto grid = new te::rst::Grid(raster.getNumberOfColumns(), raster.getNumberOfRows(), new te::gm::Envelope(*raster.getExtent()), raster.getSRID());
  std::unique_ptr<te::rst::Raster> expansible(te::rst::RasterFactory::make("EXPANSIBLE", grid, bands, {}));

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
        for(size_t i = 0; i < rasterBand->getBlockSize(); ++i)
        {
          //buffer with 0 valeu
          buffer[i] = static_cast<char>(0);
        }

        expansibleBand->write( blkXIdx, blkYIdx, buffer.get());
      }
    }
  }

  return expansible;
}

std::shared_ptr<te::rst::Raster> getRaster(const std::string& fileUri, const std::string& dataSetName)
{
    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("GDAL", fileUri));

    //RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
    std::shared_ptr<te::da::DataSet> dataset(transactor->getDataSet(dataSetName));
    assert(dataset);

    dataset->moveFirst();
    std::shared_ptr<te::rst::Raster> raster(dataset->getRaster(0).release());

    return std::shared_ptr<te::rst::Raster>(cloneRaster(*raster).release());
}

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terrama2Init("", 0);

  // std::shared_ptr<te::rst::Raster> raster = createRaster();
  std::shared_ptr<te::rst::Raster> raster = getRaster("file:///home/jsimas/MyDevel/dpi/terrama2-build/data/bitsetRaster/eymar/hidro_igual_tif", "S10238225_201703131700.tif");
  te::rp::Copy2DiskRaster(*raster, "/home/jsimas/MyDevel/dpi/terrama2-build/data/bitsetRaster/blank.tif");

  // auto dataset = getDataSet("file:///home/jsimas/MyDevel/dpi/terrama2-build/data/bitsetRaster", "polygon");
  auto dataset = getDataSet("file:///home/jsimas/MyDevel/dpi/terrama2-build/data/bitsetRaster/eymar", "Teste_SJC_pto");
  dataset->moveBeforeFirst();

  int i = 0;
  while(dataset->moveNext())
  {
    std::shared_ptr<te::rst::Raster> raster = getRaster("file:///home/jsimas/MyDevel/dpi/terrama2-build/data/bitsetRaster/eymar/hidro_igual_tif", "S10238225_201703131700.tif");
    auto geom = dataset->getGeometry(0);
    std::shared_ptr<te::gm::Polygon> polygon(static_cast<te::gm::Polygon*>(geom.release()));
    terrama2::core::BitsetIntersection intersection = terrama2::core::BitsetIntersection::bitsetIntersection(polygon, raster);
    highlightRaster(raster, intersection);

    te::rp::Copy2DiskRaster(*raster, "/home/jsimas/MyDevel/dpi/terrama2-build/data/bitsetRaster/raster_"+std::to_string(i)+".tif");
    ++i;
  }

  return 0;
}
