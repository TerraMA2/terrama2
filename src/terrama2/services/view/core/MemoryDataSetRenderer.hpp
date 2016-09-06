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
#include <terralib/maptools/AbstractLayer.h>
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
      MemoryDataSetRenderer(std::shared_ptr< te::da::DataSet > dataSet,
                            std::shared_ptr< LayerSchema > memoryLayerSchema);

      virtual ~MemoryDataSetRenderer() = default;

      void drawLayerGroupingMem(AbstractLayer* layer,
                                const std::string& geomPropertyName,
                                Canvas* canvas,
                                const te::gm::Envelope& bbox,
                                int srid,
                                const double& scale, bool* cancel) override;

      void drawLayerGeometries(AbstractLayer* layer,
                               const std::string& geomPropertyName,
                               te::se::FeatureTypeStyle* style,
                               Canvas* canvas,
                               const te::gm::Envelope& bbox,
                               int srid,
                               const double& scale, bool* cancel) override;


      void draw(AbstractLayer* layer,
                Canvas* canvas,
                const te::gm::Envelope& bbox,
                int srid,
                const double& scale, bool* cancel) override;

    private:
      std::shared_ptr< te::da::DataSet > dataSet_;
      std::shared_ptr< LayerSchema > memoryLayerSchema_;     //!< The dataset schema.
    };
  }
}

#endif // __TERRAMA2_SERVICES_VIEW_CORE_MEMORYDATASETRENDERER_HPP__
