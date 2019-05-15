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
  \file terrama2/services/analysis/core/VectorProcessingContext.hpp

  \brief Contains Geometric Intersection analysis context object
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_VECTOR_PROCESSING_CONTEXT_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_VECTOR_PROCESSING_CONTEXT_HPP__

// TerraMa2
#include "../../Config.hpp"
#include "Analysis.hpp"
#include "MonitoredObjectContext.hpp"

// Boost
#include <boost/any.hpp>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        class TMANALYSISEXPORT VectorProcessingContext : public MonitoredObjectContext
        {
          public:
            VectorProcessingContext(DataManagerPtr dataManager, AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime);

            ~VectorProcessingContext() = default;
            VectorProcessingContext(const VectorProcessingContext& other) = default;
            VectorProcessingContext(VectorProcessingContext&& other) = default;
            VectorProcessingContext& operator=(const VectorProcessingContext& other) = default;
            VectorProcessingContext& operator=(VectorProcessingContext&& other) = default;

            /*!
            \brief Sets the analysis result item into stack of attributes

            \note It does not require index value since the primary key auto increment

            \param attribute Name of the attribute.
            \param result The result value.
            */
            void addAnalysisResultItem(const std::string& attribute, boost::any result);
          private:
            int lastIndex_; //!
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_VECTOR_PROCESSING_CONTEXT_HPP__
