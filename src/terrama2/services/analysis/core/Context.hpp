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
#include "DataManager.hpp"
#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-access/Series.hpp"

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
        /*!
          \brief Contains additional information about a DataSeries to be used in an analysis.
        */
        struct ContextDataSeries
        {
          terrama2::core::Series series;
          std::string identifier;
          int64_t geometryPos = -1;
          te::sam::rtree::Index<uint64_t, 8> rtree;

        };

        /*!
          \brief Composed key for accessing a ContextDataSeries.
        */
        struct ContextKey
        {
          DataSetId datasetId_;
          AnalysisId analysisId_;
          std::string dateFilter_;
        };

        /*!
          \brief Composed key for accessing the analysis result.
        */
        struct ResultKey
        {
          std::string geomId_;
          std::string attribute_;
        };

        /*!
          \brief Comparator the context key.
        */
        struct ContextKeyComparator
        {
          /*!
            \brief Operator less then.
          */
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
              return false;
            }
            else
            {
              return lhs.dateFilter_.compare(rhs.dateFilter_) < 0;
            }
          }
        };

        /*!
          \brief Comparator the result key.
        */
        struct ResultKeyComparator
        {
          /*!
            \brief Operator less then.
          */
          bool operator()( const ResultKey& lhs , const ResultKey& rhs) const
          {
            if(lhs.geomId_.compare(rhs.geomId_) >= 0)
            {
              return true;
            }
            else
            {
              return lhs.attribute_.compare(rhs.attribute_) < 0;
            }
          }
        };

        /*!
          \class Context

          \brief Context class for the analysis execution.

          Singleton class to keep an reference to the data that will be used in the analysis.
          It's also used to store the result of the analysis.

          Thread-safe class

         */
        class Context : public te::common::Singleton<Context>
        {
          public:

            /*!
              \brief Returns the map with the result for the given analysis.

              \param analysisId Identifier of the analysis.
              \return The map with the analysis result.
            */
            std::map<ResultKey, double, ResultKeyComparator> analysisResult(uint64_t analysisId);
            void setAnalysisResult(uint64_t analysisId, const std::string& geomId, const std::string& attribute, double result);

            std::weak_ptr<terrama2::services::analysis::core::DataManager> getDataManager();
            void setDataManager(std::weak_ptr<terrama2::services::analysis::core::DataManager> dataManager);
            Analysis getAnalysis(AnalysisId analysisId) const;
            std::shared_ptr<ContextDataSeries> getContextDataset(const AnalysisId analysisId, const DataSetId datasetId, const std::string& dateFilter = "") const;
            void loadMonitoredObject(const Analysis& analysis);

            bool exists(const AnalysisId analysisId, const DataSetId datasetId, const std::string& dateFilter = "") const;
            void addDataset(const AnalysisId analysisId, terrama2::core::DataSeriesPtr dataSeries, const std::string& dateFilter = "", bool createSpatialIndex = true);
            void addDCP(const AnalysisId analysisId, terrama2::core::DataSeriesPtr dataSeries, const std::string& dateFilter = "", const bool lastValue = false);

          private:
            std::weak_ptr<terrama2::services::analysis::core::DataManager> dataManager_;
            std::map<AnalysisId, std::map<ResultKey, double, ResultKeyComparator> > analysisResult_;
            std::map<ContextKey, std::shared_ptr<ContextDataSeries>, ContextKeyComparator> datasetMap_;
            mutable std::recursive_mutex mutex_;
  			};
      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_CONTEXT_HPP__
