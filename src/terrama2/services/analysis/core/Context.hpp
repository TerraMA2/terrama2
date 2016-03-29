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
  \file terrama2/services/analysis/core/Context.hpp

  \brief Class to store the context of execution of an python script.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_CONTEXT_HPP__
#define __TERRAMA2_ANALYSIS_CORE_CONTEXT_HPP__


// TerraLib
#include <terralib/common/Singleton.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/memory/DataSet.h>

#include "Analysis.hpp"
#include "SyncronizedDataSet.hpp"
#include "../../../core/data-model/DataSetDcp.hpp"

// STL
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>



namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        struct ContextDataset
        {
          std::shared_ptr<SyncronizedDataSet> dataset;
          std::shared_ptr<te::da::DataSetType> datasetType;
          std::string identifier;
          int64_t geometryPos;
          te::sam::rtree::Index<uint64_t, 8> rtree;

        };

        struct ContextKey
        {
          DataSetId datasetId_;
          uint64_t analysisId_;
          std::string dateFilter_;
        };

        struct ContextKeyComparer
        {
            bool operator()( const ContextKey& lhs , const ContextKey& rhs) const
            {
              if(lhs.analysisId_ < rhs.analysisId_)
              {
                return true;
              }
              else if(lhs.analysisId_ > rhs.analysisId_)
              {
                return false;
              }
              else if(lhs.datasetId_ < rhs.datasetId_)
              {
                return true;
              }
              else if(lhs.datasetId_ > rhs.datasetId_)
              {
                return true;
              }
              else
              {
                return lhs.dateFilter_.compare(rhs.dateFilter_) < 0;
              }
            }
        };

        bool operator<(const ContextKey & lhs, const ContextKey & rhs);

        class Context : public te::common::Singleton<Context>
        {
          public:
            std::map<std::string, double> analysisResult(uint64_t analysisId);
            void setAnalysisResult(uint64_t analysisId, std::string geomId, double result);

            Analysis getAnalysis(const uint64_t id);
            void addAnalysis(const Analysis& analysis);

            std::shared_ptr<ContextDataset> getContextDataset(const uint64_t analysisId, const DataSetId datasetId, const std::string& dateFilter = "") const;
            std::shared_ptr<ContextDataset> addDataset(const uint64_t analysisId, const DataSetId datasetId, const std::string& dateFilter, std::shared_ptr<te::mem::DataSet>& dataset, std::string identifier, bool createSpatialIndex = true);
            std::shared_ptr<ContextDataset> addDCP(const uint64_t analysisId, terrama2::core::DataSetDcpPtr dcp, const std::string& dateFilter, std::shared_ptr<te::mem::DataSet>& dataset);
            bool exists(const uint64_t analysisId, const DataSetId datasetId, const std::string& dateFilter = "") const;


          private:
            std::map<uint64_t, Analysis> analysis_;
            std::map<uint64_t, std::map<std::string, double> > analysisResult_;
            std::map<ContextKey, std::shared_ptr<ContextDataset>, ContextKeyComparer> datasetMap_;
            mutable std::mutex mutex_;
  			};
      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_CONTEXT_HPP__
