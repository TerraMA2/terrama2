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
  \file terrama2/services/view/core/ViewStyle.cpp

  \brief View style.

  \author Vinicius Campanha
*/


// TerraLib
#include <terralib/se/ChannelSelection.h>
#include <terralib/se/ContrastEnhancement.h>
#include <terralib/se/ParameterValue.h>
#include <terralib/se/RasterSymbolizer.h>
#include <terralib/se/SelectedChannel.h>

#include <terralib/color/ColorTransform.h>

// TerraMA2
#include "ViewStyle.hpp"


terrama2::services::view::core::ViewStyle::ViewStyle(const ViewStyle& rhs)
{
  polygonSymbolizer_.reset(rhs.getSymbolizer(te::gm::PolygonType));
  lineSymbolizer_.reset(rhs.getSymbolizer(te::gm::LineStringType));
  pointSymbolizer_.reset(rhs.getSymbolizer(te::gm::PointType));
}

void terrama2::services::view::core::ViewStyle::setPolygonSymbolizer(te::se::Symbolizer* symbolizer) noexcept
{
  polygonSymbolizer_.reset(symbolizer);
}

void terrama2::services::view::core::ViewStyle::setLineSymbolizer(te::se::Symbolizer* symbolizer) noexcept
{
  lineSymbolizer_.reset(symbolizer);
}

void terrama2::services::view::core::ViewStyle::setPointSymbolizer(te::se::Symbolizer* symbolizer) noexcept
{
  pointSymbolizer_.reset(symbolizer);
}

te::se::Style* terrama2::services::view::core::ViewStyle::CreateFeatureTypeStyle(const te::gm::GeomType& geomType) const noexcept
{
  te::se::Symbolizer* symbolizer = getSymbolizer(geomType);

  te::se::Rule* rule = new te::se::Rule;

  if(symbolizer != 0)
    rule->push_back(symbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  return style;
}

te::se::Symbolizer* terrama2::services::view::core::ViewStyle::getSymbolizer(const te::gm::GeomType& geomType) const noexcept
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
      if(polygonSymbolizer_.get())
        return polygonSymbolizer_->clone();

      return nullptr;
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
      if(lineSymbolizer_.get())
        return lineSymbolizer_->clone();

      return nullptr;
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
      if(pointSymbolizer_.get())
        return pointSymbolizer_->clone();

      return nullptr;
    }

    default:
      return nullptr;
  }
}


void terrama2::services::view::core::MONO_0_Style(std::shared_ptr<te::map::MemoryDataSetLayer> layer)
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

void terrama2::services::view::core::RGB_012_RGB_Contrast_Style(std::shared_ptr<te::map::MemoryDataSetLayer> layer)
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
