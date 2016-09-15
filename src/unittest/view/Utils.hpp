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
  \file unittest/view/Utils.hpp

  \brief Utils for tests for View JsonUtils class

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_VIEW_UTILS_HPP__
#define __TERRAMA2_UNITTEST_VIEW_UTILS_HPP__

// TerraLib
#include <terralib/se.h>
#include <terralib/fe.h>
#include <terralib/maptools/GroupingAlgorithms.h>


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
      te::se::Fill* fill = CreateFill(color, "0.9");
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

te::se::Style* CreateFeatureTypeStyle(const te::gm::GeomType& geomType)
{
  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;

  {
    te::se::Rule* rule = new te::se::Rule;

    te::se::Symbolizer* symbolizer = getSymbolizer(geomType, "#00c290");

    if(symbolizer != 0)
      rule->push_back(symbolizer);

    style->push_back(rule);
  }

  {
    te::se::Rule* rule = new te::se::Rule;

    te::se::Symbolizer* symbolizer = getSymbolizer(geomType, "#4935CB");

    if(symbolizer != 0)
      rule->push_back(symbolizer);

    te::fe::PropertyName* result = new te::fe::PropertyName("RESULT");
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

    te::fe::PropertyName* result = new te::fe::PropertyName("RESULT");
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

    te::fe::PropertyName* result = new te::fe::PropertyName("RESULT");
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

    te::fe::PropertyName* result = new te::fe::PropertyName("RESULT");
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

#endif // __TERRAMA2_UNITTEST_VIEW_UTILS_HPP__
