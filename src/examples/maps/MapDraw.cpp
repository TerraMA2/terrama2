// TerraLib
#include <terralib/common.h>
#include <terralib/geometry.h>
#include <terralib/dataaccess.h>
#include <terralib/raster/RasterProperty.h>
//#include <terralib/maptools.h>
//#include <terralib/maptools/RasterLayer.h>
#include <terralib/qt/widgets/canvas/MapDisplay.h>
#include <terralib/qt/widgets/canvas/MultiThreadMapDisplay.h>
#include <terralib/se.h>
#include <terralib/qt/widgets/canvas/Canvas.h>
#include <terralib/memory/DataSource.h>


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
#include <terrama2/services/maps/core/MemoryDataSetLayer.hpp>

te::se::Style* SimpleRasterStyle(int nBands)
{
  te::se::RasterSymbolizer* symbolizer = te::se::CreateRasterSymbolizer(nBands);

  te::se::Rule* rule = new te::se::Rule;
  rule->push_back(symbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  return style;
}

te::se::Style* SimplePolygonStyle()
{
  te::se::Fill* fill = te::se::CreateFill("#FF8C00", "1.0");

  te::se::PolygonSymbolizer* symbolizer = new te::se::PolygonSymbolizer;
  symbolizer->setFill(fill);

  te::se::Rule* rule = new te::se::Rule;
  rule->push_back(symbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  return style;
}

te::se::Style* SimpleLineStyle()
{
  te::se::Stroke* stroke = te::se::CreateStroke("#000000", "3.0");

  te::se::LineSymbolizer* symbolizer = te::se::CreateLineSymbolizer(stroke);

  te::se::Rule* rule = new te::se::Rule;
  rule->push_back(symbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  return style;
}

te::se::Style* MarkPointStyle(const std::string& markName)
{
  te::se::Fill* markFill = te::se::CreateFill("#009900", "1.0");
  te::se::Stroke* markStroke = te::se::CreateStroke("#000000", "1");
  te::se::Mark* mark = te::se::CreateMark(markName, markStroke, markFill);

  te::se::Graphic* graphic = te::se::CreateGraphic(mark, "16", "", "");

  te::se::PointSymbolizer* symbolizer = te::se::CreatePointSymbolizer(graphic);

  te::se::Rule* rule = new te::se::Rule;
  rule->push_back(symbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  return style;
}

void MapDisplay(std::shared_ptr<te::da::DataSet> dataSet, std::shared_ptr<te::da::DataSetType> teDataSetType)
{
  try
  {
    if(!teDataSetType->hasGeom())
      return;

    auto geomProperty = te::da::GetFirstGeomProperty(teDataSetType.get());
    assert(geomProperty);

    std::shared_ptr< te::gm::Envelope > extent(dataSet->getExtent(teDataSetType->getPropertyPosition(geomProperty)));

    // A map of GeomType -> Style
    std::map<te::gm::GeomType, te::se::Style*> styles;
    styles[te::gm::PolygonType] = SimplePolygonStyle();
    styles[te::gm::LineStringType] = SimpleLineStyle();
    styles[te::gm::PointType] = MarkPointStyle("circle");

    // Creates a Layer
    std::shared_ptr< te::map::MemoryDataSetLayer > layer(new te::map::MemoryDataSetLayer(te::common::Convert2String(1), teDataSetType->getName(), dataSet, teDataSetType));
    layer->setDataSetName(teDataSetType->getName());
    layer->setVisibility(te::map::VISIBLE);
    layer->setExtent(*extent);
    layer->setStyle(styles[geomProperty->getGeometryType()]);
    layer->setRendererType("ABSTRACT_LAYER_RENDERER");
    layer->setSRID(geomProperty->getSRID());

    // Creates a canvas
    double llx = extent->m_llx;
    double lly = extent->m_lly;
    double urx = extent->m_urx;
    double ury = extent->m_ury;

    std::shared_ptr<te::qt::widgets::Canvas> canvas(new te::qt::widgets::Canvas(800, 600));
    canvas->calcAspectRatio(llx, lly, urx, ury);
    canvas->setWindow(llx, lly, urx, ury);
    canvas->setBackgroundColor(te::color::RGBAColor(255, 255, 255, TE_OPAQUE));

    bool cancel = false;

    layer->draw(canvas.get(), *extent.get(), geomProperty->getSRID(), 0, &cancel);

    canvas->save("GeneretadeGeomImage", te::map::PNG);

    canvas->clear();

  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in DrawLayer example: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in DrawLayer example!" << std::endl;
  }
}


void RGB_012_RGB_Contrast_Style(std::shared_ptr<te::da::DataSet> dataSet, std::shared_ptr<te::da::DataSetType> teDataSetType, std::shared_ptr<te::qt::widgets::Canvas> c, te::map::MemoryDataSetLayer* l, te::gm::Envelope* e, int srid)
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

  l->setStyle(s);

  bool cancel = false;

  l->draw(c.get(), *e, srid, 0, &cancel);

  c->save("GeneretadeImage", te::map::PNG);

  c->clear();
}

void DrawRasterStyledLayersFromDataSet(std::shared_ptr<te::da::DataSet> dataSet, std::shared_ptr<te::da::DataSetType> teDataSetType)
{
  try
  {
    dataSet->moveFirst();

    std::size_t rpos = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::RASTER_TYPE);
//    std::shared_ptr<te::rst::Raster> raster(dataSet->getRaster(rpos));

    auto raster(dataSet->getRaster(rpos));

    te::gm::Envelope* extent = raster->getExtent();

    // Creates a DataSetLayer of raster
    std::shared_ptr<te::map::MemoryDataSetLayer> rasterLayer(new te::map::MemoryDataSetLayer(te::common::Convert2String(1), raster->getName(), dataSet, teDataSetType));
    rasterLayer->setDataSetName(teDataSetType->getDatasetName());
    rasterLayer->setExtent(*extent);
    rasterLayer->setRendererType("ABSTRACT_LAYER_RENDERER");
    rasterLayer->setSRID(raster->getSRID());

    // Creates a canvas
    double llx = extent->m_llx;
    double lly = extent->m_lly;
    double urx = extent->m_urx;
    double ury = extent->m_ury;

    std::shared_ptr<te::qt::widgets::Canvas> canvas(new te::qt::widgets::Canvas(800, 600));
    canvas->calcAspectRatio(llx, lly, urx, ury);
    canvas->setWindow(llx, lly, urx, ury);
    canvas->setBackgroundColor(te::color::RGBAColor(255, 255, 255, TE_OPAQUE));

    // RGB 012 with contrast in RGB bands Style
    RGB_012_RGB_Contrast_Style(dataSet, teDataSetType, canvas, rasterLayer.get(), extent, raster->getSRID());

  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in Styling example: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in Styling example!" << std::endl;
  }
}

int main(int argc, char** argv)
{
  terrama2::core::initializeTerraMA();

  QApplication app(argc, 0);

  try
  {
    {
      //DataProvider information
      terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
      terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
      dataProvider->uri = "file://";
      dataProvider->uri += TERRAMA2_DATA_DIR;
      dataProvider->uri += "/geotiff";

      dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
      dataProvider->dataProviderType = "FILE";
      dataProvider->active = true;

      //DataSeries information
      terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
      dataSeries->semantics.code = "GRID-geotiff";

      terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
      dataSet->active = true;
      dataSet->format.emplace("mask", "cbers2b_rgb342_crop.tif");
      dataSet->format.emplace("timezone", "-03");

      dataSeries->datasetList.emplace_back(dataSet);

      //empty filter
      terrama2::core::Filter filter;
      //accessing data
      terrama2::core::DataAccessorGeoTiff accessor(dataProviderPtr, dataSeriesPtr);

      std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > series = accessor.getSeries(filter);

      std::shared_ptr<te::da::DataSet> ds = series.begin()->second.syncDataSet->dataset();
      std::shared_ptr<te::da::DataSetType> teDataSetType = series.begin()->second.teDataSetType;

      DrawRasterStyledLayersFromDataSet(ds, teDataSetType);
    }

    {
      //DataProvider information
      terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
      terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
      dataProvider->uri = "file://";
      dataProvider->uri += TERRAMA2_DATA_DIR;
      dataProvider->uri += "/shapefile";

      dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
      dataProvider->dataProviderType = "FILE";
      dataProvider->active = true;

      //DataSeries information
      terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
      dataSeries->semantics.code = "STATIC_DATA-ogr";

      terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
      dataSet->active = true;
      dataSet->format.emplace("mask", "munic_2001.shp");
      dataSet->format.emplace("timezone", "-03");

      dataSeries->datasetList.emplace_back(dataSet);

      //empty filter
      terrama2::core::Filter filter;
      //accessing data
      terrama2::core::DataAccessorStaticDataOGR accessor(dataProviderPtr, dataSeriesPtr);

      std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > series = accessor.getSeries(filter);

      std::shared_ptr<te::da::DataSet> ds = series.begin()->second.syncDataSet->dataset();
      std::shared_ptr<te::da::DataSetType> teDataSetType = series.begin()->second.teDataSetType;

      MapDisplay(ds, teDataSetType);
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
