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
  \file terrama2/services/analysis/core/BaseContext.hpp

  \brief Base class for analysis context

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_BASE_CONTEXT_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_BASE_CONTEXT_HPP__

#include <set>
#include <memory>

#include "../../../core/data-model/Filter.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../core/data-access/DataSetSeries.hpp"
#include "../../../core/Shared.hpp"
#include "DataManager.hpp"
#include "Analysis.hpp"
#include "Typedef.hpp"

// Python
#include <Python.h>

#include <terralib/raster/Raster.h>
#include <terralib/raster/Interpolator.h>

namespace te
{
  namespace srs
  {
    class Converter;
  } /* srs */
} /* te */

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {

        /*!
          \brief Composed key for accessing a ContextDataSeries.
        */
        struct ObjectKey
        {
          uint32_t objectId_; //!< Object identifier.
          std::string dateFilterBegin_; //!< Begin date restriction.
          std::string dateFilterEnd_; //!< End date restriction.
        };

        struct ObjectKeyHash
        {
          std::size_t operator()(ObjectKey const& key) const
          {
            return std::hash<std::string>()(std::to_string(key.objectId_)+key.dateFilterBegin_+key.dateFilterEnd_);
          }
        };


        /*!
          \brief Comparator the context key.
        */
        struct LessKeyComparator
        {
          /*!
            \brief Operator less then.
          */
          bool operator()(const ObjectKey& lhs, const ObjectKey& rhs) const
          {
            if(lhs.objectId_ < rhs.objectId_)
            {
              return true;
            }
            else if(lhs.objectId_ > rhs.objectId_)
            {
              return false;
            }
            else
            {
              return lhs.dateFilterBegin_.compare(rhs.dateFilterBegin_) < 0;
            }
          }
        };

        struct EqualKeyComparator
        {
          bool operator()(const ObjectKey& lhs, const ObjectKey& rhs) const
          {
            return lhs.objectId_ == rhs.objectId_&& lhs.objectId_ == rhs.objectId_;
          }
        };

        class BaseContext : public std::enable_shared_from_this<BaseContext>
        {
          public:
            BaseContext(terrama2::services::analysis::core::DataManagerPtr dataManager, terrama2::services::analysis::core::AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime);

            virtual ~BaseContext();
            BaseContext(const BaseContext& other) = default;
            BaseContext(BaseContext&& other) = default;
            BaseContext& operator=(const BaseContext& other) = default;
            BaseContext& operator=(BaseContext&& other) = default;

            inline std::set<std::string> getErrors() const { return errosSet_; }

            /*!
              \brief Returns a weak pointer to the data manager.
            */
            inline std::weak_ptr<terrama2::services::analysis::core::DataManager> getDataManager() const { return dataManager_; }

            /*!
              \brief Returns the analysis configuration.
            */
            inline AnalysisPtr getAnalysis() const { return analysis_; }
            inline std::shared_ptr<te::dt::TimeInstantTZ> getStartTime() const { return startTime_; }


            /*!
              \brief Returns the python interpreter main thread state.

              \return The python interpreter main thread state.
            */

            /*!
              \brief Adds an error message to list of errors that occurred in the analysis execution.
              \param errorMessage The error message.
            */
            void addError(const std::string& errorMessage);

            terrama2::core::DataSeriesPtr findDataSeries(const std::string& dataSeriesName);

            /*!
              \brief Returns a vector of raster for the given dataset id.

              \param datasetId The DataSet identifier.
              \return A vector of smart pointers to the raster.
            */
            std::vector< std::shared_ptr<te::rst::Raster> > getRasterList(const terrama2::core::DataSeriesPtr& dataSeries,
                const DataSetId datasetId, const std::string& dateDiscardBefore = "", const std::string& dateDiscardAfter = "");

            std::shared_ptr<te::rst::Interpolator> getInterpolator(std::shared_ptr<te::rst::Raster> raster);


            std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > getSeriesMap(DataSeriesId dataSeriesId,
                const std::string& dateDiscardBefore = "",
                const std::string& dateDiscardAfter = "");

          protected:
            /*!
              \brief Return the a multimap of DataSetGridPtr to Raster

              The parameters dateDiscardBefore and dateDiscardAfter are optional,
              if they are not set only the last raster is returned.
            */
            std::unordered_multimap<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> >
            getGridMap(DataManagerPtr dataManager, DataSeriesId dataSeriesId, const std::string& dateDiscardBefore = "", const std::string& dateDiscardAfter = "");

            terrama2::core::Filter createFilter(const std::string& dateDiscardBefore = "", const std::string& dateDiscardAfter = "");

            /*!
              \brief Adds the given raster to the context map.

              \param datasetId The DataSet identifier.
              \param raster The raster to be added to the context.

            */
            inline void addRaster(ObjectKey key, std::shared_ptr<te::rst::Raster> raster) { rasterMap_[key].push_back(raster); };

            virtual std::shared_ptr<te::rst::Raster> resampleRaster(std::shared_ptr<te::rst::Raster> raster) { return raster; }


            mutable std::recursive_mutex mutex_; //!< A mutex to synchronize all operations.

            std::weak_ptr<terrama2::services::analysis::core::DataManager> dataManager_;
            AnalysisPtr analysis_;
            std::shared_ptr<te::dt::TimeInstantTZ> startTime_;
            std::set<std::string> errosSet_;
            std::shared_ptr<terrama2::core::FileRemover> remover_;

            std::unordered_map<std::string, terrama2::core::DataSeriesPtr > dataSeriesMap_;
            std::unordered_map<Srid, std::shared_ptr<te::srs::Converter> > converterMap_;
            std::unordered_map<ObjectKey, std::unordered_multimap<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> >, ObjectKeyHash, EqualKeyComparator> analysisGridMap_;
            std::unordered_map<ObjectKey, std::unordered_map<terrama2::core::DataSetPtr,terrama2::core::DataSetSeries >, ObjectKeyHash, EqualKeyComparator> analysisSeriesMap_;
            std::unordered_map<ObjectKey, std::vector<std::shared_ptr<te::rst::Raster> >, ObjectKeyHash, EqualKeyComparator > rasterMap_;
            std::unordered_map<std::shared_ptr<te::rst::Raster>, std::shared_ptr<te::rst::Interpolator> > interpolatorMap_;
        };

      }
    }
  }
}

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_BASE_CONTEXT_HPP__
