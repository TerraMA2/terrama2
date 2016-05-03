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
#include <set>


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
          terrama2::core::Series series; //!< Dataset information.
          std::string identifier; //!< Identifier column.
          int64_t geometryPos = -1; //!< Geometry column position.
          te::sam::rtree::Index<uint64_t, 8> rtree; //!< Spatial index in memory

        };

        /*!
          \brief Composed key for accessing a ContextDataSeries.
        */
        struct ContextKey
        {
          DataSetId datasetId_; //!< DataSet identifier.
          AnalysisId analysisId_; //!< Analysis identifier.
          std::string dateFilter_; //!< Date restriction.
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
            std::map<std::string, std::map<std::string, double> > analysisResult(AnalysisId analysisId);

            /*!
              \brief Sets the analysis result for a geometry and a given attribute.

              \param analysisId Identifier of the analysis.
              \param geomId Geometry identifier.
              \param attribute Name of the attribute.
              \param result The result value.
            */
            void setAnalysisResult(uint64_t analysisId, const std::string& geomId, const std::string& attribute, double result);

            /*!
              \brief Returns a weak pointer to the data manager.

              \param result The weak pointer to the data manager.
            */
            std::weak_ptr<terrama2::services::analysis::core::DataManager> getDataManager();

            /*!
              \brief Returns a weak pointer to the data manager.

              \param dataManager A weak pointer to the data manager.
            */
            void setDataManager(std::weak_ptr<terrama2::services::analysis::core::DataManager> dataManager);

            /*!
              \brief Returns the analysis configuration.

              \param analysisId The analysis identifier.
            */
            Analysis getAnalysis(AnalysisId analysisId) const;

            /*!
              \brief Returns a smart pointer that contains the TerraLib DataSet for the given DataSetId.

              \param analysisId The analysis identifier.
              \param datasetId The DataSet identifier.
              \param dateFilter The date restriction to be used in the DataSet.
            */
            std::shared_ptr<ContextDataSeries> getContextDataset(const AnalysisId analysisId, const DataSetId datasetId, const std::string& dateFilter = "") const;

            /*!
              \brief Reads the analysis configuration and adds to the context the monitored object dataset.

              \param analysis The analysis configuration.
            */
            void loadMonitoredObject(const Analysis& analysis);

            /*!
              \brief Returns true if the given dataset has already been loaded into the context.

              \param analysisId The analysis identifier.
              \param datasetId The DataSet identifier.
              \param dateFilter The date restriction to be used in the DataSet.
              \return True if the given dataset has already been loaded into the context.
            */
            bool exists(const AnalysisId analysisId, const DataSetId datasetId, const std::string& dateFilter = "") const;

            /*!
              \brief Creates a TerraLib dataset to read the data with the date filter and adds it to the context.

              \param analysisId The analysis identifier.
              \param dataSeries A smart pointer to the DataSeries to be loaded.
              \param dateFilter The date restriction to be used in the DataSet.
              \param createSpatialIndex Defines if a spatial index should be created to optimize data access.
            */
            void addDataset(const AnalysisId analysisId, terrama2::core::DataSeriesPtr dataSeries, const std::string& dateFilter = "", bool createSpatialIndex = true);

            /*!
              \brief Creates a TerraLib dataset to read the DCP data with the date filter and adds it to the context.

              \param analysisId The analysis identifier.
              \param dataSeries A smart pointer to the DataSeries to be loaded.
              \param dateFilter The date restriction to be used in the DataSet.
              \param lastValue Defines if is an historic operator or if it should access only the latest data.
            */
            void addDCP(const AnalysisId analysisId, terrama2::core::DataSeriesPtr dataSeries, const std::string& dateFilter = "", const bool lastValue = false);

            /*!
              \brief Returns the set of attributes that compose the analysis result.

              \param analysisId The analysis identifier.
            */
            std::set<std::string> getAttributes(AnalysisId analysisId) const;

            /*!
              \brief Adds an attribute to the result list of the given analysis.

              \param analysisId The analysis identifier.
              \param attribute The name of the attribute.
            */
            void addAttribute(AnalysisId analysisId, const std::string& attribute);

          private:
            std::weak_ptr<terrama2::services::analysis::core::DataManager> dataManager_; //!< Weak pointer to the data manager.
            std::map<AnalysisId, std::set<std::string> > attributes_; //!< Set of attributes that compose the result of an analysis.
            std::map<AnalysisId, std::map<std::string, std::map<std::string, double> > > analysisResult_; //!< Map with analysis result AnalysisId -> GeomId -> Attribute -> Value.
            std::map<ContextKey, std::shared_ptr<ContextDataSeries>, ContextKeyComparator> datasetMap_; //!< Map containing all loaded datasets.
            mutable std::recursive_mutex mutex_; //!< A mutex to syncronize all operations.
  			};
      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_CONTEXT_HPP__
