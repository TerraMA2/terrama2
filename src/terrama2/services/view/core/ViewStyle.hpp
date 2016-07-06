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
  \file terrama2/services/view/core/ViewStyle.hpp

  \brief View style.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_VIEWTYLE_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_VIEWTYLE_HPP__

// TerraLib
#include <terralib/se.h>
#include <terralib/color/ColorTransform.h>

// TerraMA2
#include "MemoryDataSetLayer.hpp"

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {

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

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
} // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_CORE_VIEWTYLE_HPP__
