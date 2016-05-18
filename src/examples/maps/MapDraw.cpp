// TerraLib
#include <terralib/common.h>
#include <terralib/geometry.h>
#include <terralib/dataaccess.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/maptools.h>
#include <terralib/qt/widgets/canvas/MapDisplay.h>
#include <terralib/qt/widgets/canvas/MultiThreadMapDisplay.h>
#include <terralib/se.h>
#include <terralib/qt/widgets/canvas/Canvas.h>

// STL
#include <cassert>
#include <iostream>
#include <list>
#include <memory>

// Qt
#include <QApplication>
#include <QDialog>
#include <QLabel>


#include <terrama2/core/utility/Utils.hpp>


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

void MapDisplay(QApplication& app)
{
  try
  {
    std::map<std::string, std::string> connInfo;
    connInfo["URI"] = "/home/vinicius/data/munic_2001.shp";

    // Creates and connects data source
    te::da::DataSourcePtr datasource = te::da::DataSourceManager::getInstance().open(te::common::Convert2String(1), "OGR", connInfo);

    // Get the number of data set types that belongs to the data source
    std::vector<std::string> datasets = datasource->getDataSetNames();

    // Creates the MapDisplay
    std::shared_ptr<te::qt::widgets::MapDisplay> mapDisplay(new te::qt::widgets::MultiThreadMapDisplay(QSize(700, 500)));

    // MapDisplay box
    te::gm::Envelope env;

    // A map of GeomType -> Style
    std::map<te::gm::GeomType, te::se::Style*> styles;
    styles[te::gm::PolygonType] = SimplePolygonStyle();
    styles[te::gm::LineStringType] = SimpleLineStyle();
    styles[te::gm::PointType] = MarkPointStyle("circle");

    // Creates the Layer list
    int id = 0;
    std::list<te::map::AbstractLayerPtr> layerList;
    for(unsigned int i = 0; i < datasets.size(); ++i)
    {
      std::shared_ptr<te::da::DataSetType> dt(datasource->getDataSetType(datasets[i]));

      if(!dt->hasGeom())
        continue;

      te::gm::GeometryProperty* geomProperty = te::da::GetFirstGeomProperty(dt.get());
      assert(geomProperty);

      // To MapDisplay extent
      std::shared_ptr<te::gm::Envelope> e(datasource->getExtent(datasets[i], geomProperty->getName()));
      env.Union(*e);

      // Creates a Layer
      te::map::DataSetLayer* layer = new te::map::DataSetLayer(te::common::Convert2String(++id), datasets[i]);
      layer->setDataSourceId(datasource->getId());
      layer->setDataSetName(datasets[i]);
      layer->setVisibility(te::map::VISIBLE);
      layer->setExtent(*e);
      layer->setStyle(styles[geomProperty->getGeometryType()]);
      layer->setRendererType("ABSTRACT_LAYER_RENDERER");

      layerList.push_back(layer);
    }

    mapDisplay->setMinimumSize(QSize(60, 60));
    mapDisplay->setResizePolicy(te::qt::widgets::MapDisplay::Center);
    mapDisplay->setLayerList(layerList);
    mapDisplay->show();
    mapDisplay->setExtent(env);

    app.exec();

    layerList.clear();
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


void RGB_012_RGB_Contrast_Style(te::qt::widgets::Canvas* c, te::map::DataSetLayer* l, te::gm::Envelope* e, int srid)
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

  l->draw(c, *e, srid, 0, &cancel);

  c->save("GeneretadeImage", te::map::PNG);

  c->clear();
}


void DrawRasterStyledLayers(QApplication& app)
{
  try
  {
    // Connection string to a raster file
    std::map<std::string, std::string> connInfo;
    connInfo["URI"] = "/home/vinicius/data/cbers2b_rgb342_crop.tif";

    // Creates and connects data source
    te::da::DataSourcePtr datasource = te::da::DataSourceManager::getInstance().open(te::common::Convert2String(2), "GDAL", connInfo);

    // Get the number of data set types that belongs to the data source
    std::vector<std::string> datasets = datasource->getDataSetNames();
    assert(!datasets.empty());

    // Gets the first dataset
    std::string dataSetName(datasets[0]);
    std::shared_ptr<te::da::DataSet> ds(datasource->getDataSet(dataSetName));

    std::size_t rpos = te::da::GetFirstPropertyPos(ds.get(), te::dt::RASTER_TYPE);
    std::shared_ptr<te::rst::Raster> raster(ds->getRaster(rpos));

    std::shared_ptr<te::gm::Envelope> extent(raster->getExtent());

    // Creates a DataSetLayer of raster
    std::shared_ptr<te::map::DataSetLayer> rasterLayer(new te::map::DataSetLayer(te::common::Convert2String(1), dataSetName));
    rasterLayer->setDataSourceId(datasource->getId());
    rasterLayer->setDataSetName(dataSetName);
    rasterLayer->setExtent(*extent.get());
    rasterLayer->setRendererType("ABSTRACT_LAYER_RENDERER");
    rasterLayer->setSRID(raster->getSRID());

    // Get the projection used to be paint the raster
    int srid = raster->getSRID();
    //int srid = 4618; // LL SAD69

    //    if(srid != raster->getSRID())
    //      extent.transform(raster->getSRID(), srid);

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
    RGB_012_RGB_Contrast_Style(canvas.get(), rasterLayer.get(), extent.get(), srid);

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
    MapDisplay(app);

    DrawRasterStyledLayers(app);
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
