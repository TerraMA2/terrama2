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
  \file terrama2/services/analysis/core/Shared.hpp

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_SHARED_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_SHARED_HPP__

#include <memory>

class ThreadPool;
//! Shared smart pointer for ThreadPool
typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        class DataManager;
        //! Shared smart pointer for DataManager
        typedef std::shared_ptr<terrama2::services::analysis::core::DataManager> DataManagerPtr;

        struct Analysis;
        //! Shared smart pointer for Analysis
        typedef std::shared_ptr<const terrama2::services::analysis::core::Analysis> AnalysisPtr;

        struct AnalysisOutputGrid;
        //! Shared smart pointer for OutputGrid
        typedef std::shared_ptr<const terrama2::services::analysis::core::AnalysisOutputGrid> AnalysisOutputGridPtr;

        class BaseContext;
        typedef std::shared_ptr<terrama2::services::analysis::core::BaseContext> BaseContextPtr;

        class GridContext;
        typedef std::shared_ptr<terrama2::services::analysis::core::GridContext> GridContextPtr;

        class MonitoredObjectContext;
        typedef std::shared_ptr<terrama2::services::analysis::core::MonitoredObjectContext> MonitoredObjectContextPtr;
      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_SHARED_HPP__
