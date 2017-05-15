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
  \file terrama2/services/view/core/MemoryDataSetRenderer.cpp

  \brief DataSet Renderer to deal with an dataSet in memory(without a dataSource).

  \author Vinicius Campanha
*/

// TerraLib
#include <terralib/core/translator/Translator.h>
#include <terralib/maptools/Canvas.h>
#include <terralib/maptools/Chart.h>
#include <terralib/maptools/Utils.h>
#include <terralib/maptools/GroupingItem.h>
#include <terralib/maptools/QueryEncoder.h>
#include <terralib/common/Globals.h>
#include <terralib/common/STLUtils.h>
#include <terralib/dataaccess/query_h.h>


// TerraMA2
#include "MemoryDataSetRenderer.hpp"

te::map::MemoryDataSetRenderer::MemoryDataSetRenderer(std::shared_ptr< te::da::DataSet > dataSet,
                                                      std::shared_ptr< LayerSchema > memoryLayerSchema)
  : dataSet_(dataSet), memoryLayerSchema_(memoryLayerSchema)
{

}

void te::map::MemoryDataSetRenderer::drawLayerGroupingMem(AbstractLayer* layer,
                                                          const std::string& geomPropertyName,
                                                          Canvas* canvas,
                                                          const te::gm::Envelope& bbox,
                                                          int srid,
                                                          const double& scale, bool* cancel)
{
  assert(!geomPropertyName.empty());

  //check scale
  if (layer->getStyle())
  {
    if (!layer->getStyle()->getRules().empty())
    {
      // The current rule
      te::se::Rule* rule = layer->getStyle()->getRules()[0];

      if (!(scale >= rule->getMinScaleDenominator() && scale < rule->getMaxScaleDenominator()))
      {
        return;
      }
    }
  }

  // Creates a canvas configurer
  te::map::CanvasConfigurer cc(canvas);

  // The layer grouping
  Grouping* grouping = layer->getGrouping();

  // The referenced property name
  std::string propertyName = grouping->getPropertyName();
  assert(!propertyName.empty());

  // The grouping type
  GroupingType type = grouping->getType();

  // The grouping precision
  const std::size_t& precision = grouping->getPrecision();

  // The grouping items
  const std::vector<GroupingItem*>& items = grouping->getGroupingItems();

  std::size_t nGroupItems = items.size();

  // case UNIQUE_VALUE: for each GroupingItem, builds a map [item value] -> [symbolizers]
  std::map<std::string, std::vector<te::se::Symbolizer*> > uniqueGroupsMap;

  // case (NOT) UNIQUE_VALUE: for each GroupingItem, builds a map [item upper limit] -> [symbolizers]
  std::map<std::pair< double, double>, std::vector<te::se::Symbolizer*> > othersGroupsMap;

  for(std::size_t i = 0; i < nGroupItems; ++i)
  {
    // The current group item
    GroupingItem* item = items[i];
    assert(item);

    if(type == UNIQUE_VALUE)
    {
      uniqueGroupsMap[item->getValue()] = item->getSymbolizers();
    }
    else
    {
      double lowerLimit = atof(item->getLowerLimit().c_str());
      double upperLimit = atof(item->getUpperLimit().c_str());
      std::pair<double, double> range(lowerLimit, upperLimit);

      othersGroupsMap[range] = item->getSymbolizers();
    }
  }

  // Builds the task message; e.g. ("Drawing the grouping of layer Countries.")
  std::string message = TE_TR("Drawing the grouping of layer");
  message += " " + layer->getTitle() + ".";

  // Creates the draw task
  te::common::TaskProgress task(message, te::common::TaskProgress::DRAW);

  if(dataSet_.get() == 0)
    throw Exception((boost::format(TE_TR("Could not retrieve the data set from the layer %1%.")) % layer->getTitle()).str());

  if(dataSet_->moveNext() == false)
    return;

  // Gets the first geometry property
  std::size_t gpos = te::da::GetPropertyPos(dataSet_.get(), geomPropertyName);

  // Gets the property position
  std::auto_ptr<te::map::LayerSchema> dt(layer->getSchema());
  std::size_t propertyPos = te::da::GetPropertyPos(dt.get(), propertyName);

  // Verifies if is necessary convert the data set geometries to the given srid
  bool needRemap = false;
  if((layer->getSRID() != TE_UNKNOWN_SRS) && (srid != TE_UNKNOWN_SRS) && (layer->getSRID() != srid))
    needRemap = true;

  // The layer chart
  Chart* chart = layer->getChart();

  dataSet_->moveFirst();
  do
  {
    std::vector<te::se::Symbolizer*> symbolizers;

    // Finds the current data set item on group map
    std::string value;

    if(dataSet_->isNull(propertyPos))
      value = te::common::Globals::sm_nanStr;
    else
      value = dataSet_->getAsString(propertyPos, precision);

    if(type == UNIQUE_VALUE)
    {
      std::map<std::string, std::vector<te::se::Symbolizer*> >::const_iterator it = uniqueGroupsMap.find(value);
      if(it == uniqueGroupsMap.end())
        continue;
      symbolizers = it->second;
    }
    else
    {
      double dvalue = atof(value.c_str());
      std::map<std::pair< double, double>, std::vector<te::se::Symbolizer*> >::const_iterator it;
      for(it = othersGroupsMap.begin(); it != othersGroupsMap.end(); ++it)
      {
        if(dvalue >= it->first.first && dvalue <= it->first.second)
          break;
      }

      if(it == othersGroupsMap.end())
      {
        te::se::Style* style = layer->getStyle();
        if(style)
        {
          if(!style->getRules().empty())
          {
            te::se::Rule* rule = style->getRule(0);

            symbolizers = rule->getSymbolizers();
          }
        }
      }
      else
      {
        symbolizers = it->second;
      }

      if(symbolizers.empty())
        continue;
    }

    std::auto_ptr<te::gm::Geometry> geom;
    try
    {
      geom = dataSet_->getGeometry(gpos);
      if(geom.get() == 0)
        continue;
    }
    catch(const std::exception& /*e*/)
    {
      continue;
    }

    // Gets the set of symbolizers defined on group item
    std::size_t nSymbolizers = symbolizers.size();

    for(std::size_t j = 0; j < nSymbolizers; ++j) // for each <Symbolizer>
    {
      // The current symbolizer
      te::se::Symbolizer* symb = symbolizers[j];

      // Let's config the canvas based on the current symbolizer
      cc.config(symb);

      // If necessary, geometry remap
      if(needRemap)
      {
        geom->setSRID(layer->getSRID());
        geom->transform(srid);
      }

      canvas->draw(geom.get());

      if(chart && j == nSymbolizers - 1)
        buildChart(chart, dataSet_.get(), geom.get());
    }

    if(cancel != 0 && (*cancel))
      return;

  } while(dataSet_->moveNext());

  // Let's draw the generated charts
  for(std::size_t i = 0; i < m_chartCoordinates.size(); ++i)
  {
    canvas->drawImage(static_cast<int>(m_chartCoordinates[i].x),
                      static_cast<int>(m_chartCoordinates[i].y),
                      m_chartImages[i],
                      chart->getWidth(),
                      chart->getHeight());

    te::common::Free(m_chartImages[i], chart->getHeight());
  }
}

void te::map::MemoryDataSetRenderer::drawLayerGeometries(AbstractLayer* layer,
                                                         const std::string& geomPropertyName,
                                                         te::se::FeatureTypeStyle* style,
                                                         Canvas* canvas,
                                                         const te::gm::Envelope& bbox,
                                                         int srid,
                                                         const double& scale, bool* cancel)
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

    if (!(scale >= rule->getMinScaleDenominator() && scale < rule->getMaxScaleDenominator()))
    {
      continue;
    }

    // Gets the rule filter
    const te::fe::Filter* filter = rule->getFilter();


    // TODO: draw dataset like it does in grouping
/*
 * // Let's retrieve the correct dataset
    std::auto_ptr<te::da::DataSet> dataSet_
    if(!filter)
    {
      try
      {
        // There isn't a Filter expression. Gets the data using only extent spatial restriction...
        dataset = layer->getData(geomPropertyName, &bbox, te::gm::INTERSECTS);
      }
      catch(Exception& e)
      {
        throw e;
      }
    }
    else
    {
      try
      {
        // Gets an enconder
        te::map::QueryEncoder filter2Query;

        // Converts the Filter expression to a TerraLib Expression!
        te::da::Expression* exp = filter2Query.getExpression(filter);
        if(exp == 0)
          throw Exception(TE_TR("Could not convert the OGC Filter expression to TerraLib expression!"));

        // 1) Creating the final restriction. i.e. Filter expression + extent spatial restriction

        // The extent spatial restriction
        te::da::LiteralEnvelope* lenv = new te::da::LiteralEnvelope(bbox, layer->getSRID());
        te::da::PropertyName* geometryPropertyName = new te::da::PropertyName(geomPropertyName);
        te::da::ST_Intersects* intersects = new te::da::ST_Intersects(geometryPropertyName, lenv);

        // Combining the expressions (Filter expression + extent spatial restriction)
        te::da::And* restriction = new te::da::And(exp, intersects);

        // 2) Calling the layer query method to get the correct restricted data.
        dataset = layer->getData(restriction);
      }
      catch(const std::exception& e)
      {
        throw Exception((boost::format(TE_TR("Could not retrieve the data set from the layer %1%.")) % layer->getTitle()).str());
      }
    }
*/
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
    //task.setTotalSteps(symbolizers.size() * dataSet_->size()); // Removed! The te::da::DataSet size() method would be too costly to compute.

    // For while, first geometry property.
    std::size_t gpos = te::da::GetPropertyPos(dataSet_.get(), geomPropertyName);

    if(symbolizers.empty())
    {
      // The current rule do not have a symbolizer. Try creates a default based on first geometry of dataSet_.
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


void te::map::MemoryDataSetRenderer::draw(AbstractLayer* layer,
                                          Canvas* canvas,
                                          const te::gm::Envelope& bbox,
                                          int srid,
                                          const double& scale, bool* cancel)
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

