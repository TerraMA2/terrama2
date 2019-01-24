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
  \file terrama2/services/analysis/core/occurrence/ContextManager.hpp

  \brief Contains occurrence analysis context.

  \author Jano Simas
*/

#ifndef __TERRAMA2_ANALYSIS_CORE_CONTEXT_MANAGER_HPP__
#define __TERRAMA2_ANALYSIS_CORE_CONTEXT_MANAGER_HPP__

// TerraMa2
#include "../../Config.hpp"
#include "Shared.hpp"
#include "Analysis.hpp"
#include "BaseContext.hpp"

// TerraLib
#include <terralib/common/Singleton.h>

//STL
#include <unordered_map>
#include <set>
#include <mutex>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        class MonitoredObjectContext;
        class GridContext;

        class TMANALYSISEXPORT ContextManager : public te::common::Singleton<ContextManager>
        {
          public:
            void addMonitoredObjectContext(const AnalysisHashCode analysisHashCode, MonitoredObjectContextPtr context);
            void addGridContext(const AnalysisHashCode analysisHashCode, GridContextPtr context);
            void addGeometryContext(const AnalysisHashCode analysisHashCode, GeometryIntersectionContextPtr context);

            MonitoredObjectContextPtr getMonitoredObjectContext(const AnalysisHashCode analysisHashCode) const;
            GeometryIntersectionContextPtr getGeometryContext(const AnalysisHashCode analysisHashCode) const;
            GridContextPtr getGridContext(const AnalysisHashCode analysisHashCode) const;
            AnalysisPtr getAnalysis(const AnalysisHashCode analysisHashCode) const;

            void clearContext(const AnalysisHashCode analysisHashCode);

            void addError(const AnalysisHashCode analysisHashCode, const std::string& error);
            std::set<std::string> getMessages(const AnalysisHashCode analysisHashCode, terrama2::services::analysis::core::BaseContext::MessageType messageType) const;


          private:
            std::unordered_map<AnalysisHashCode, AnalysisPtr> analysisMap_;

            std::unordered_map<AnalysisHashCode, MonitoredObjectContextPtr> monitoredObjectContextMap_;
            std::unordered_map<AnalysisHashCode, GridContextPtr> gridContextMap_;
            std::unordered_map<AnalysisHashCode, GeometryIntersectionContextPtr> geometryContextMap_;

            std::unordered_map<AnalysisHashCode, std::set<std::string> > analysisErrorMap_;
            mutable std::recursive_mutex mutex_; //!< A mutex to synchronize all operations.
        };
      }
    }
  }
}

#endif //__TERRAMA2_ANALYSIS_CORE_CONTEXT_MANAGER_HPP__
