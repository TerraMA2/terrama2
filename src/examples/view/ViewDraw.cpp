// TerraLib
#include <terralib/common.h>
#include <terralib/geometry.h>
#include <terralib/dataaccess.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/qt/widgets/canvas/MapDisplay.h>
#include <terralib/qt/widgets/canvas/MultiThreadMapDisplay.h>
#include <terralib/se.h>
#include <terralib/qt/widgets/canvas/Canvas.h>
#include <terralib/memory/DataSource.h>
#include <terralib/color/ColorTransform.h>


// STL
#include <cassert>
#include <iostream>
#include <list>
#include <memory>

// Qt
#include <QApplication>
#include <QDialog>
#include <QLabel>

// TerraMA2
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/impl/DataAccessorGeoTiff.hpp>
#include <terrama2/impl/DataAccessorStaticDataOGR.hpp>
#include <terrama2/core/data-access/GridSeries.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/services/view/core/MemoryDataSetLayer.hpp>
#include <terrama2/services/view/core/View.hpp>


std::string GenerateRandomColor()
{
  te::color::ColorTransform t;
  t.setHsv(rand() % 360, 64 + (rand() % 192), 128 + (rand() % 128));

  te::color::RGBAColor color(t.getRgba());

  return color.getColor();
}

te::se::Stroke* CreateStroke(te::se::Graphic* graphicFill,
                                     const std::string& width, const std::string& opacity,
                                     const std::string& dasharray, const std::string& linecap, const std::string& linejoin)
{
  te::se::Stroke* stroke = new te::se::Stroke;

  if(graphicFill)
    stroke->setGraphicFill(graphicFill);

  if(!width.empty())
    stroke->setWidth(width);

  if(!opacity.empty())
    stroke->setOpacity(opacity);

  if(!dasharray.empty())
    stroke->setDashArray(dasharray);

  if(!linecap.empty())
    stroke->setLineCap(linecap);

  if(!linejoin.empty())
    stroke->setLineJoin(linecap);

  return stroke;
}

te::se::Stroke* CreateStroke(const std::string& color, const std::string& width,
                                     const std::string& opacity, const std::string& dasharray,
                                     const std::string& linecap, const std::string& linejoin)
{
  te::se::Stroke* stroke = CreateStroke(0, width, opacity, dasharray, linecap, linejoin);

  if(!color.empty())
    stroke->setColor(color);

  return stroke;
}

te::se::Fill* CreateFill(const std::string& color, const std::string& opacity)
{
  te::se::Fill* fill = new te::se::Fill;

  if(!color.empty())
    fill->setColor(color);

  if(!opacity.empty())
    fill->setOpacity(opacity);

  return fill;
}

te::se::Symbolizer* CreateSymbolizer(const te::gm::GeomType& geomType, const std::string& color)
{
  switch(geomType)
  {
    case te::gm::PolygonType:
    case te::gm::PolygonMType:
    case te::gm::PolygonZType:
    case te::gm::PolygonZMType:
    case te::gm::MultiPolygonType:
    case te::gm::MultiPolygonMType:
    case te::gm::MultiPolygonZType:
    case te::gm::MultiPolygonZMType:
    case te::gm::MultiSurfaceType:
    case te::gm::MultiSurfaceMType:
    case te::gm::MultiSurfaceZType:
    case te::gm::MultiSurfaceZMType:
    {
      te::se::Fill* fill = CreateFill(color, "100.0");
      te::se::Stroke* stroke = CreateStroke("#000000", "1", "", "", "", "");
      te::se::PolygonSymbolizer* symbolizer = new te::se::PolygonSymbolizer;
      symbolizer->setFill(fill);
      symbolizer->setStroke(stroke);
      return symbolizer;
    }

    case te::gm::LineStringType:
    case te::gm::LineStringMType:
    case te::gm::LineStringZType:
    case te::gm::LineStringZMType:
    case te::gm::MultiLineStringType:
    case te::gm::MultiLineStringMType:
    case te::gm::MultiLineStringZType:
    case te::gm::MultiLineStringZMType:
    {
      te::se::Stroke* stroke = CreateStroke(color, "1", "", "", "", "");
      te::se::LineSymbolizer* symbolizer = new te::se::LineSymbolizer;
      symbolizer->setStroke(stroke);
      return symbolizer;
    }

    case te::gm::PointType:
    case te::gm::PointMType:
    case te::gm::PointZType:
    case te::gm::PointZMType:
    case te::gm::MultiPointType:
    case te::gm::MultiPointMType:
    case te::gm::MultiPointZType:
    case te::gm::MultiPointZMType:
    {
      te::se::Fill* markFill = CreateFill(color, "1.0");
      te::se::Stroke* markStroke = CreateStroke("#000000", "1", "", "", "", "");
      te::se::Mark* mark = CreateMark("circle", markStroke, markFill);
      te::se::Graphic* graphic = CreateGraphic(mark, "12", "", "");
      return te::se::CreatePointSymbolizer(graphic);
    }

    default:
      return 0;
  }
}

te::se::Style* CreateFeatureTypeStyle(const te::gm::GeomType& geomType)
{
  std::string color = GenerateRandomColor();

  te::se::Symbolizer* symbolizer = CreateSymbolizer(geomType, color);

  te::se::Rule* rule = new te::se::Rule;

  if(symbolizer != 0)
    rule->push_back(symbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  return style;
}


void MONO_0_Style(std::shared_ptr<te::map::MemoryDataSetLayer> layer)
{
  //create default raster symbolizer
  te::se::RasterSymbolizer* rs = new te::se::RasterSymbolizer();

  //set transparency
  rs->setOpacity(new te::se::ParameterValue("1.0"));

  //set channel selection
  te::se::ChannelSelection* cs = new te::se::ChannelSelection();
  cs->setColorCompositionType(te::se::GRAY_COMPOSITION);

  //channel M
  te::se::SelectedChannel* scM = new te::se::SelectedChannel();
  scM->setSourceChannelName("0");
  cs->setGrayChannel(scM);

  rs->setChannelSelection(cs);

  //add symbolizer to a layer style
  te::se::Rule* r = new te::se::Rule();
  r->push_back(rs);

  te::se::Style* s = new te::se::CoverageStyle();
  s->push_back(r);

  layer->setStyle(s);
}

void RGB_012_RGB_Contrast_Style(std::shared_ptr<te::map::MemoryDataSetLayer> layer)
{
  //create default raster symbolizer
  te::se::RasterSymbolizer* rs = new te::se::RasterSymbolizer();

  //set transparency
  rs->setOpacity(new te::se::ParameterValue("1.0"));

  //set channel selection
  te::se::ChannelSelection* cs = new te::se::ChannelSelection();
  cs->setColorCompositionType(te::se::RGB_COMPOSITION);

  //channel R
  te::se::SelectedChannel* scR = new te::se::SelectedChannel();
  scR->setSourceChannelName("0");

  te::se::ContrastEnhancement* cR = new te::se::ContrastEnhancement();
  cR->setGammaValue(0.5);
  scR->setContrastEnhancement(cR);
  cs->setRedChannel(scR);

  //channel G
  te::se::SelectedChannel* scG = new te::se::SelectedChannel();
  scG->setSourceChannelName("1");

  te::se::ContrastEnhancement* cG = new te::se::ContrastEnhancement();
  cG->setGammaValue(0.5);
  scG->setContrastEnhancement(cG);
  cs->setGreenChannel(scG);

  //channel B
  te::se::SelectedChannel* scB = new te::se::SelectedChannel();
  scB->setSourceChannelName("2");

  te::se::ContrastEnhancement* cB = new te::se::ContrastEnhancement();
  cB->setGammaValue(0.5);
  scB->setContrastEnhancement(cB);
  cs->setBlueChannel(scB);

  rs->setChannelSelection(cs);

  //add symbolizer to a layer style
  te::se::Rule* r = new te::se::Rule();
  r->push_back(rs);

  te::se::Style* s = new te::se::CoverageStyle();
  s->push_back(r);

  layer->setStyle(s);
}


std::shared_ptr< te::map::MemoryDataSetLayer > CreateGeometryLayer(std::shared_ptr<te::da::DataSet> dataSet,
                         std::shared_ptr<te::da::DataSetType> teDataSetType)
{
  try
  {
    if(!teDataSetType->hasGeom())
      throw;

    auto geomProperty = te::da::GetFirstGeomProperty(teDataSetType.get());
    assert(geomProperty);

    std::shared_ptr< te::gm::Envelope > extent(dataSet->getExtent(teDataSetType->getPropertyPosition(geomProperty)));

    // Creates a Layer
    std::shared_ptr< te::map::MemoryDataSetLayer > layer(new te::map::MemoryDataSetLayer(te::common::Convert2String(1), teDataSetType->getName(), dataSet, teDataSetType));
    layer->setDataSetName(teDataSetType->getName());
    layer->setVisibility(te::map::VISIBLE);
    layer->setExtent(*extent);
    layer->setStyle(CreateFeatureTypeStyle(geomProperty->getGeometryType()));
    layer->setRendererType("ABSTRACT_LAYER_RENDERER");
    layer->setSRID(geomProperty->getSRID());

    return layer;

  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in DrawLayer example: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in DrawLayer example!" << std::endl;
  }
  return std::shared_ptr< te::map::MemoryDataSetLayer >();
}

std::shared_ptr<te::map::MemoryDataSetLayer> CreateRasterLayer(std::shared_ptr<te::da::DataSet> dataSet, std::shared_ptr<te::da::DataSetType> teDataSetType)
{
  try
  {
    dataSet->moveFirst();

    std::size_t rpos = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::RASTER_TYPE);

    auto raster(dataSet->getRaster(rpos));

    te::gm::Envelope* extent = raster->getExtent();

    // Creates a DataSetLayer of raster
    std::shared_ptr<te::map::MemoryDataSetLayer> rasterLayer(new te::map::MemoryDataSetLayer(te::common::Convert2String(1), raster->getName(), dataSet, teDataSetType));
    rasterLayer->setDataSetName(teDataSetType->getDatasetName());
    rasterLayer->setExtent(*extent);
    rasterLayer->setRendererType("ABSTRACT_LAYER_RENDERER");
    rasterLayer->setSRID(raster->getSRID());

    // set  Style
//    RGB_012_RGB_Contrast_Style(rasterLayer);

    MONO_0_Style(rasterLayer);

    return rasterLayer;
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in Styling example: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in Styling example!" << std::endl;
  }

  return std::shared_ptr<te::map::MemoryDataSetLayer>();
}

void Draw(std::shared_ptr<te::map::MemoryDataSetLayer> RasterLayer, std::shared_ptr<te::map::MemoryDataSetLayer> GeometryLayer)
{
 te::gm::Envelope extent(RasterLayer->getExtent());
 int srid = RasterLayer->getSRID();

 extent.Union(GeometryLayer->getExtent());

  // Creates a canvas
  double llx = extent.m_llx;
  double lly = extent.m_lly;
  double urx = extent.m_urx;
  double ury = extent.m_ury;

  std::unique_ptr<te::qt::widgets::Canvas> canvas(new te::qt::widgets::Canvas(800, 600));
  canvas->calcAspectRatio(llx, lly, urx, ury);
  canvas->setWindow(llx, lly, urx, ury);
  canvas->setBackgroundColor(te::color::RGBAColor(255, 255, 255, TE_OPAQUE));

  bool cancel = false;

  RasterLayer->draw(canvas.get(), extent, srid, 0, &cancel);
  GeometryLayer->draw(canvas.get(), extent, srid, 0, &cancel);

  canvas->save("GeneretadImage", te::map::PNG);

  canvas->clear();
}

int main(int argc, char** argv)
{
  terrama2::core::initializeTerraMA();

  QApplication app(argc, 0);

  try
  {
    {
      //DataProvider information
      terrama2::core::DataProvider* dataProviderRaster = new terrama2::core::DataProvider();
      terrama2::core::DataProviderPtr dataProviderRasterPtr(dataProviderRaster);
      dataProviderRaster->uri = "file://";
      dataProviderRaster->uri += TERRAMA2_DATA_DIR;
      dataProviderRaster->uri += "/geotiff";

      dataProviderRaster->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
      dataProviderRaster->dataProviderType = "FILE";
      dataProviderRaster->active = true;

      //DataSeries information
      terrama2::core::DataSeries* dataSeriesRaster = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr dataSeriesRasterPtr(dataSeriesRaster);
      dataSeriesRaster->semantics.code = "GRID-geotiff";

      terrama2::core::DataSetGrid* dataSetRaster = new terrama2::core::DataSetGrid();
      dataSetRaster->active = true;
      dataSetRaster->format.emplace("mask", "23S465RS.tif");
      dataSetRaster->format.emplace("timezone", "-03");

      dataSeriesRaster->datasetList.emplace_back(dataSetRaster);

      //empty filter
      terrama2::core::Filter filterRaster;
      //accessing data
      terrama2::core::DataAccessorGeoTiff accessorRaster(dataProviderRasterPtr, dataSeriesRasterPtr);

      std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesRaster = accessorRaster.getSeries(filterRaster);

      std::shared_ptr<te::da::DataSet> dsRaster = seriesRaster.begin()->second.syncDataSet->dataset();
      std::shared_ptr<te::da::DataSetType> teDataSetTypeRaster = seriesRaster.begin()->second.teDataSetType;

      //DataProvider information
      terrama2::core::DataProvider* dataProviderGeometry = new terrama2::core::DataProvider();
      terrama2::core::DataProviderPtr dataProviderGeometryPtr(dataProviderGeometry);
      dataProviderGeometry->uri = "file://";
      dataProviderGeometry->uri += TERRAMA2_DATA_DIR;
      dataProviderGeometry->uri += "/shapefile";

      dataProviderGeometry->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
      dataProviderGeometry->dataProviderType = "FILE";
      dataProviderGeometry->active = true;

      //DataSeries information
      terrama2::core::DataSeries* dataSeriesGeometry = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr dataSeriesGeometryPtr(dataSeriesGeometry);
      dataSeriesGeometry->semantics.code = "STATIC_DATA-ogr";

      terrama2::core::DataSetGrid* dataSetGeometry = new terrama2::core::DataSetGrid();
      dataSetGeometry->active = true;
      dataSetGeometry->format.emplace("mask", "35MUE250GC_SIR.shp");
      dataSetGeometry->format.emplace("timezone", "-03");

      dataSeriesGeometry->datasetList.emplace_back(dataSetGeometry);

      //empty filter
      terrama2::core::Filter filterGeometry;
      //accessing data
      terrama2::core::DataAccessorStaticDataOGR accessorGeometry(dataProviderGeometryPtr, dataSeriesGeometryPtr);

      std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesGeometry = accessorGeometry.getSeries(filterGeometry);

/*
      std::shared_ptr<te::da::DataSet> dsGeometry = seriesGeometry.begin()->second.syncDataSet->dataset();
      std::shared_ptr<te::da::DataSetType> teDataSetTypeGeometry = seriesGeometry.begin()->second.teDataSetType;

      std::shared_ptr<te::map::MemoryDataSetLayer> rasterLayer = CreateRasterLayer(dsRaster, teDataSetTypeRaster);
      std::shared_ptr<te::map::MemoryDataSetLayer> GeometryLayer =CreateGeometryLayer(dsGeometry, teDataSetTypeGeometry);

      Draw(rasterLayer,GeometryLayer);
*/

      std::vector<std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries>> seriesList;
      seriesList.push_back(seriesRaster);
      seriesList.push_back(seriesGeometry);

      terrama2::services::view::core::drawSeriesList(seriesList, 800, 600);

    }
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in DrawLayer example: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in DrawLayer example!" << std::endl;
    return EXIT_FAILURE;
  }

  terrama2::core::finalizeTerraMA();

  return EXIT_SUCCESS;
}
