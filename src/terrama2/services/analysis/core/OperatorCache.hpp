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
  \file terrama2/services/analysis/core/OperatorCache.hpp

  \brief Cache to store the context of execution of an analysis.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_OPERATOR_CACHE_HPP__
#define __TERRAMA2_ANALYSIS_CORE_OPERATOR_CACHE_HPP__


// TerraLib
#include "Typedef.hpp"
#include "Shared.hpp"
#include "../../../core/Typedef.hpp"

#include <limits>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \struct OperatorCache
          \brief Model object to the analysis result.
        */
        struct OperatorCache
        {
          int32_t index = -1; //!< Geometry index of the monitored object.
          AnalysisHashCode analysisHashCode = 0; //!< Hashcode of current analysis.
          terrama2::services::analysis::core::AnalysisPtr analysisPtr;
          int32_t row = -1; //!< Output raster row.
          int32_t column = -1; //!< Output raster column.
          double sum = 0; //!< Result of the sum.
          double max = 0; //!< Maximum value.
          double min = 0; //!< Minimum value.
          double median = 0; //!< Median value.
          double mean = 0; //!< Mean value.
          double standardDeviation = 0; //!< Standard deviation value.
          double variance = 0; //!< Standard deviation value.
          uint32_t count = 0; //!< Count value.
        };
      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_CONTEXT_HPP__
