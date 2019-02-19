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
  \file terrama2/services/analysis/core/GridContext.hpp

  \brief Contains grid analysis context.

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_CONTEXT_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_CONTEXT_HPP__

// TerraMa2
#include "Config.hpp"
#include "Analysis.hpp"
#include "BaseContext.hpp"
#include "../../../core/utility/Utils.hpp"

// TerraLib
#include <terralib/geometry/Coord2D.h>

// Forward declaration
namespace te
{
  namespace rst
  {
    class Raster;
  }
}

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        class TMANALYSISEXPORT GridContext : public BaseContext
        {
          public:
            GridContext(DataManagerPtr dataManager,  AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime);

            ~GridContext() = default;
            GridContext(const GridContext& other) = default;
            GridContext(GridContext&& other) = default;
            GridContext& operator=(const GridContext& other) = default;
            GridContext& operator=(GridContext&& other) = default;

            /*!
              \brief Returns the output raster of the analysis.
            */
            std::shared_ptr<te::rst::Raster> getOutputRaster();
            /*!
              \brief Reads output grid configuration and adds the output grid to context.
              \param analysisHashCode Hash code of the analysis.
            */
            void createOutputRaster();

            /*!
              \brief Convert a coordinate from output srid to another srid

              Use an stored converter to make the conversion, if none instantiate and store a new one.

              \param point Coordinate to be converted.
              \param srid New srid of the coordinate.
              \return Coordinate in the new srid
            */
            te::gm::Coord2D convertoTo(const te::gm::Coord2D& point, const int srid);

          protected:

            std::map<std::string, double> getOutputRasterInfo();
            void addInterestAreaToRasterInfo(std::map<std::string, double>& outputRasterInfo, const terrama2::core::Filter& filter);
            void addResolutionToRasterInfo(std::map<std::string, double>& outputRasterInfo, const terrama2::core::Filter& filter);

            std::shared_ptr<te::rst::Raster> outputRaster_;
            std::map<std::string, double> outputRasterInfo_;
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_CONTEXT_HPP__
