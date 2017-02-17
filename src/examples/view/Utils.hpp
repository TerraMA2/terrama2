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
  \file examples/view/Utils.hpp

  \brief Utils for ViewDraw examples

  \author Vinicius Campanha
 */

#ifndef __TERRAMA2_EXAMPLES_VIEW_UTILS_HPP__
#define __TERRAMA2_EXAMPLES_VIEW_UTILS_HPP__

// TerraLib
#include <terralib/se.h>
#include <terralib/fe.h>

// TerraMA2
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/services/view/core/DataManager.hpp>
#include <terrama2_config.hpp>

void prepareExample(std::shared_ptr<terrama2::services::view::core::DataManager> dataManager)
{
  //DataProvider information
  terrama2::core::DataProvider* dataProviderRaster = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderRasterPtr(dataProviderRaster);
  dataProviderRaster->id = 1;
  dataProviderRaster->name = "dataProviderRaster";
  dataProviderRaster->uri = "file://";
  dataProviderRaster->uri += TERRAMA2_DATA_DIR;
  dataProviderRaster->uri += "/geotiff";

  dataProviderRaster->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProviderRaster->dataProviderType = "FILE";
  dataProviderRaster->active = true;

  dataManager->add(dataProviderRasterPtr);


  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  //DataSeries information
  terrama2::core::DataSeries* dataSeriesRaster = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesRasterPtr(dataSeriesRaster);
  dataSeriesRaster->id = 1;
  dataSeriesRaster->name = "dataSeriesRaster";
  dataSeriesRaster->dataProviderId = 1;
  dataSeriesRaster->semantics = semanticsManager.getSemantics("GRID-gdal");

  terrama2::core::DataSetGrid* dataSetRaster = new terrama2::core::DataSetGrid();
  dataSetRaster->active = true;
  dataSetRaster->format.emplace("mask", "Spot_Vegetacao_Jul2001_SP.tif");
  dataSetRaster->format.emplace("timezone", "-03");

  dataSeriesRaster->datasetList.emplace_back(dataSetRaster);

  dataManager->add(dataSeriesRasterPtr);

  //DataProvider information
  terrama2::core::DataProvider* dataProviderGeometry = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderGeometryPtr(dataProviderGeometry);
  dataProviderGeometry->id = 2;
  dataProviderGeometry->name = "dataProviderGeometry";
  dataProviderGeometry->uri = "file://";
  dataProviderGeometry->uri += TERRAMA2_DATA_DIR;
  dataProviderGeometry->uri += "/shapefile";

  dataProviderGeometry->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProviderGeometry->dataProviderType = "FILE";
  dataProviderGeometry->active = true;

  dataManager->add(dataProviderGeometryPtr);

  //DataSeries information
  terrama2::core::DataSeries* dataSeriesGeometry = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesGeometryPtr(dataSeriesGeometry);
  dataSeriesGeometry->id = 2;
  dataSeriesGeometry->name = "dataSeriesGeometry";
  dataSeriesGeometry->dataProviderId = 2;
  dataSeriesGeometry->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");

  terrama2::core::DataSetGrid* dataSetGeometry = new terrama2::core::DataSetGrid();
  dataSetGeometry->active = true;
  dataSetGeometry->format.emplace("mask", "35MUE250GC_SIR.shp");
  dataSetGeometry->format.emplace("timezone", "-03");

  dataSeriesGeometry->datasetList.emplace_back(dataSetGeometry);

  dataManager->add(dataSeriesGeometryPtr);

  //DataSeries information
  terrama2::core::DataSeries* dataSeriesGeometry2 = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesGeometry2Ptr(dataSeriesGeometry2);
  dataSeriesGeometry2->id = 3;
  dataSeriesGeometry2->name = "dataSeriesGeometry2";
  dataSeriesGeometry2->dataProviderId = 2;
  dataSeriesGeometry2->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");

  terrama2::core::DataSetGrid* dataSetGeometry2 = new terrama2::core::DataSetGrid();
  dataSetGeometry2->active = true;
  dataSetGeometry2->format.emplace("mask", "Rod_Principais_SP_lin.shp");
  dataSetGeometry2->format.emplace("timezone", "-03");
  dataSetGeometry2->format.emplace("srid", "4326");

  dataSeriesGeometry2->datasetList.emplace_back(dataSetGeometry2);

  dataManager->add(dataSeriesGeometry2Ptr);
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

te::se::Symbolizer* getSymbolizer(const te::gm::GeomType& geomType, std::string color)
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
      te::se::Fill* fill = CreateFill(color, "0.8");
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
      te::se::PointSymbolizer* symbolizer = new te::se::PointSymbolizer;
      symbolizer->setGraphic(graphic);

      return symbolizer;
    }

    default:
      return nullptr;
  }
}

te::se::Style* CreateFeatureTypeStyle(const te::gm::GeomType& geomType, std::string color)
{
  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;

  {
    te::se::Rule* rule = new te::se::Rule;

    te::se::Symbolizer* symbolizer = getSymbolizer(geomType, color);

    if(symbolizer != 0)
      rule->push_back(symbolizer);

    style->push_back(rule);
  }

  {
    te::se::Rule* rule = new te::se::Rule;

    te::se::Symbolizer* symbolizer = getSymbolizer(geomType, "#4935CB");

    if(symbolizer != 0)
      rule->push_back(symbolizer);

    te::fe::PropertyName* result = new te::fe::PropertyName("result");
    te::fe::Literal* value = new te::fe::Literal("1");
    te::fe::BinaryComparisonOp* stateEqual = new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsEqualTo, result, value);

    te::fe::Filter* filter = new te::fe::Filter;
    filter->setOp(stateEqual);

    rule->setFilter(filter);

    style->push_back(rule);
  }

  {
    te::se::Rule* rule = new te::se::Rule;

    te::se::Symbolizer* symbolizer = getSymbolizer(geomType, "#fdcc00");

    if(symbolizer != 0)
      rule->push_back(symbolizer);

    te::fe::PropertyName* result = new te::fe::PropertyName("result");
    te::fe::Literal* value = new te::fe::Literal("2");
    te::fe::BinaryComparisonOp* stateEqual = new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsEqualTo, result, value);

    te::fe::Filter* filter = new te::fe::Filter;
    filter->setOp(stateEqual);

    rule->setFilter(filter);

    style->push_back(rule);
  }

  {
    te::se::Rule* rule = new te::se::Rule;

    te::se::Symbolizer* symbolizer = getSymbolizer(geomType, "#ff7400");

    if(symbolizer != 0)
      rule->push_back(symbolizer);

    te::fe::PropertyName* result = new te::fe::PropertyName("result");
    te::fe::Literal* value = new te::fe::Literal("3");
    te::fe::BinaryComparisonOp* stateEqual = new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsEqualTo, result, value);

    te::fe::Filter* filter = new te::fe::Filter;
    filter->setOp(stateEqual);

    rule->setFilter(filter);

    style->push_back(rule);
  }

  {
    te::se::Rule* rule = new te::se::Rule;

    te::se::Symbolizer* symbolizer = getSymbolizer(geomType, "#FF0000");

    if(symbolizer != 0)
      rule->push_back(symbolizer);

    te::fe::PropertyName* result = new te::fe::PropertyName("result");
    te::fe::Literal* value = new te::fe::Literal("4");
    te::fe::BinaryComparisonOp* stateEqual = new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsEqualTo, result, value);

    te::fe::Filter* filter = new te::fe::Filter;
    filter->setOp(stateEqual);

    rule->setFilter(filter);

    style->push_back(rule);
  }

  style->setName(new std::string("Style 1"));

  return style;
}

te::se::Style* RGB_012_RGB_Contrast_Style()
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

  te::se::Style* style = new te::se::CoverageStyle();
  style->push_back(r);

  return style;
}

te::se::Style* MONO_0_Style()
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

  te::se::Style* style = new te::se::CoverageStyle();
  style->push_back(r);

  return style;
}

#endif //__TERRAMA2_EXAMPLES_VIEW_UTILS_HPP__
