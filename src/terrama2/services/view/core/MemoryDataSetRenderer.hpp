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
  \file terrama2/services/view/core/MemoryDataSetRenderer.hpp

  \brief DataSet Renderer to deal with an dataSet in memory(without a dataSource).

  \author Vinicius Campanha
*/


#ifndef __TERRAMA2_SERVICES_VIEW_CORE_MEMORYDATASETRENDERER_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_MEMORYDATASETRENDERER_HPP__

// TerraLib
#include <terralib/se/FeatureTypeStyle.h>
#include <terralib/se/Rule.h>
#include <terralib/se/Utils.h>
#include <terralib/se/CoverageStyle.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/maptools/AbstractLayerRenderer.h>
#include <terralib/maptools/Exception.h>
#include <terralib/maptools/Grouping.h>
#include <terralib/maptools/CanvasConfigurer.h>
#include <terralib/common/progress/TaskProgress.h>

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace te
{
  namespace map
  {
    class MemoryDataSetRenderer : public AbstractLayerRenderer
    {

    public:
      MemoryDataSetRenderer(std::shared_ptr< te::da::DataSet > dataSet, std::shared_ptr< LayerSchema > memoryLayerSchema)
        : dataSet_(dataSet), memoryLayerSchema_(memoryLayerSchema)
      { }

      virtual ~MemoryDataSetRenderer() = default;


      void drawLayerGeometries(AbstractLayer* layer,
                               const std::string& geomPropertyName,
                               te::se::FeatureTypeStyle* style,
                               Canvas* canvas,
                               const te::gm::Envelope& bbox,
                               int srid,
                               const double& scale, bool* cancel) override
      {
        assert(!geomPropertyName.empty());

        // Creates a canvas configurer
        CanvasConfigurer cc(canvas);

        // Number of rules defined on feature type style
        std::size_t nRules = style->getRules().size();

        for(std::size_t i = 0; i < nRules; ++i) // for each <Rule>
        {
          // The current rule
          te::se::Rule* rule = style->getRule(i);
          assert(rule);

          // TODO: Should be verified the MinScaleDenominator and MaxScaleDenominator. Where will we put the current scale information? Method parameter?

          if (!(scale >= rule->getMinScaleDenominator() && scale < rule->getMaxScaleDenominator()))
          {
            continue;
          }

          if(dataSet_.get() == 0)
            throw Exception((boost::format(TE_TR("Could not retrieve the data set from the layer %1%.")) % layer->getTitle()).str());

          if(dataSet_->moveFirst() == false)
            continue;

          // Gets the set of symbolizers defined on current rule
          const std::vector<te::se::Symbolizer*>& symbolizers = rule->getSymbolizers();

          // Builds task message; e.g. ("Drawing the layer Countries. Rule 1 of 3.")
          std::string message = TE_TR("Drawing the layer");
          message += " " + layer->getTitle() + ". ";
          message += TE_TR("Rule");
          message += " " + boost::lexical_cast<std::string>(i + 1) + " " + TE_TR("of") + " ";
          message += boost::lexical_cast<std::string>(nRules) + ".";

          // Creates a draw task
          te::common::TaskProgress task(message, te::common::TaskProgress::DRAW);
          //task.setTotalSteps(symbolizers.size() * dataset->size()); // Removed! The te::da::DataSet size() method would be too costly to compute.

          // For while, first geometry property. TODO: get which geometry property the symbolizer references
          std::size_t gpos = te::da::GetPropertyPos(dataSet_.get(), geomPropertyName);

          if(symbolizers.empty())
          {
            // The current rule do not have a symbolizer. Try creates a default based on first geometry of dataset.
            std::shared_ptr<te::gm::Geometry> g(dataSet_->getGeometry(gpos));
            assert(g.get());

            te::se::Symbolizer* symbolizer = te::se::CreateSymbolizer(g->getGeomTypeId());
            assert(symbolizer);

            rule->push_back(symbolizer);

            dataSet_->moveFirst();
          }

          std::size_t nSymbolizers = symbolizers.size();

          for(std::size_t j = 0; j < nSymbolizers; ++j) // for each <Symbolizer>
          {
            // The current symbolizer
            te::se::Symbolizer* symb = symbolizers[j];

            // Let's config the canvas based on the current symbolizer
            cc.config(symb);

            // Let's draw! for each data set geometry...
            if(j != nSymbolizers - 1)
              drawDatSetGeometries(dataSet_.get(), gpos, canvas, layer->getSRID(), srid, 0, cancel, &task);
            else
              drawDatSetGeometries(dataSet_.get(), gpos, canvas, layer->getSRID(), srid, layer->getChart(), cancel, &task); // Here, produces the chart if exists

            // Prepares to draw the other symbolizer
            dataSet_->moveFirst();

          } // end for each <Symbolizer>

        }   // end for each <Rule>
      }


      void draw(AbstractLayer* layer,
                Canvas* canvas,
                const te::gm::Envelope& bbox,
                int srid,
                const double& scale, bool* cancel) override
      {
        if(!bbox.isValid())
          throw Exception(TE_TR("The requested box is invalid!"));

        assert(layer);
        assert(canvas);

        // Check if layer extent intersects the drawing area and so compute bounding box intersection
        te::gm::Envelope reprojectedBBOX(bbox);

        if((layer->getSRID() != TE_UNKNOWN_SRS) && (srid != TE_UNKNOWN_SRS))
        {
          reprojectedBBOX.transform(srid, layer->getSRID());

          if(!reprojectedBBOX.isValid())
            throw Exception(TE_TR("The reprojected box is invalid!"));
        }
        else if(layer->getSRID() != srid)
        {
          throw Exception(TE_TR("The layer or view don't have a valid SRID!"));
        }

        if(!reprojectedBBOX.intersects(layer->getExtent()))
          return;

        // Adjust internal renderer transformer
        m_transformer.setTransformationParameters(bbox.m_llx, bbox.m_lly, bbox.m_urx, bbox.m_ury, canvas->getWidth(), canvas->getHeight());

        canvas->setWindow(bbox.m_llx, bbox.m_lly, bbox.m_urx, bbox.m_ury);

        // Resets internal renderer state
        reset();

        te::gm::Envelope ibbox = reprojectedBBOX.intersection(layer->getExtent());

        assert(ibbox.isValid());

        assert(memoryLayerSchema_.get());

        // Gets the name of the referenced spatial property
        std::string spatialPropertyName = layer->getGeomPropertyName();

        if(memoryLayerSchema_->hasGeom())
        {
          std::unique_ptr< te::gm::GeometryProperty > geometryProperty;

          if(spatialPropertyName.empty())
            geometryProperty.reset(dynamic_cast<te::gm::GeometryProperty*>(te::da::GetFirstGeomProperty(memoryLayerSchema_.get())->clone()));
          else
            geometryProperty.reset(dynamic_cast<te::gm::GeometryProperty*>(memoryLayerSchema_->getProperty(spatialPropertyName)->clone()));

          assert(geometryProperty);

          // If the AbstractLayer has a grouping, do not consider the style.
          Grouping* grouping = layer->getGrouping();
          if(grouping && grouping->isVisible())
          {
            drawLayerGroupingMem(layer, geometryProperty->getName(), canvas, ibbox, srid, scale, cancel);
            return;
          }

          // Gets the layer style
          te::se::Style* style = layer->getStyle();
          if(style == 0)
          {
            // Try create an appropriate style. i.e. a FeatureTypeStyle
            style = te::se::CreateFeatureTypeStyle(geometryProperty->getGeometryType());

            if(style == 0)
              throw Exception((boost::format(TE_TR("Could not create a default feature type style to the layer %1%.")) % layer->getTitle()).str());

            layer->setStyle(style);
          }

          // Should I render this style?
          te::se::FeatureTypeStyle* fts = dynamic_cast<te::se::FeatureTypeStyle*>(style);
          if(fts == 0)
            throw Exception(TE_TR("The layer style is not a Feature Type Style!"));

          drawLayerGeometries(layer, geometryProperty->getName(), fts, canvas, ibbox, srid, scale, cancel);
        }
        else if(memoryLayerSchema_->hasRaster())
        {
          te::rst::RasterProperty* rasterProperty = 0;

          if(spatialPropertyName.empty())
            rasterProperty = te::da::GetFirstRasterProperty(memoryLayerSchema_.get());
          else
            rasterProperty = dynamic_cast<te::rst::RasterProperty*>(memoryLayerSchema_->getProperty(spatialPropertyName));

          assert(rasterProperty);

          // Get the layer style
          te::se::Style* style = layer->getStyle();
          if(style == 0)
          {
            // Try create an appropriate style. i.e. a CoverageStyle
            style = te::se::CreateCoverageStyle(rasterProperty->getBandProperties());

            if(style == 0)
              throw Exception((boost::format(TE_TR("Could not create a default coverage style to the layer %1%.")) % layer->getTitle()).str());

            layer->setStyle(style);
          }

          // Should I render this style?
          te::se::CoverageStyle* cs = dynamic_cast<te::se::CoverageStyle*>(style);
          if(cs == 0)
            throw Exception(TE_TR("The layer style is not a Coverage Style!"));

          // Retrieves the data
          if(dataSet_.get() == 0)
            throw Exception((boost::format(TE_TR("Could not retrieve the data set from the layer %1%.")) % layer->getTitle()).str());

          // Retrieves the raster
          std::shared_ptr<te::rst::Raster> raster(dataSet_->getRaster(rasterProperty->getName()));
          if(dataSet_.get() == 0)
            throw Exception((boost::format(TE_TR("Could not retrieve the raster from the layer %1%.")) % layer->getTitle()).str());

          // Let's draw!
          DrawRaster(raster.get(), canvas, ibbox, layer->getSRID(), bbox, srid, cs, scale);
        }
        else
        {
          throw Exception(TE_TR("The layer don't have a geometry or raster property!"));
        }
      }

    private:
      std::shared_ptr< te::da::DataSet > dataSet_;
      std::shared_ptr< LayerSchema > memoryLayerSchema_;     //!< The dataset schema.
    };
  }
}

#endif // __TERRAMA2_SERVICES_VIEW_CORE_MEMORYDATASETRENDERER_HPP__
