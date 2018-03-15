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
  \file terrama2/services/analysis/core/grid/Operator.hpp

  \brief Contains grid analysis operators.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_OPERATOR_HPP__

// TerraMA2
#include "../BufferMemory.hpp"
#include "../Analysis.hpp"
#include "../../../../core/data-access/SynchronizedInterpolator.hpp"

// STL
#include <string>


namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace grid
        {

          /*!
            \brief Return current pixel value for the selected data series.
            \param dataSeriesName DataSeries name.
            \return The current pixel value for the selected data series.
          */
          TMANALYSISEXPORT double sample(const std::string& dataSeriesName, size_t bandIdx = 0);

          TMANALYSISEXPORT double getValue(std::shared_ptr<te::rst::Raster> raster, std::shared_ptr<terrama2::core::SynchronizedInterpolator> interpolator, double column, double row, size_t bandIdx);


        }   // end namespace grid
      }     // end namespace core
    }       // end namespace analysis
  }         // end namespace services
}           // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_OPERATOR_HPP__
