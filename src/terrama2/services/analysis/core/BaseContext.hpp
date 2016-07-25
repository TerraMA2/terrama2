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


#ifndef __TERRAMA2_SERVINCES_ANALYSIS_CORE_BASE_CONTEXT_HPP__
#define __TERRAMA2_SERVINCES_ANALYSIS_CORE_BASE_CONTEXT_HPP__

#include <set>
#include <memory>

#include "DataManager.hpp"
#include "Analysis.hpp"
#include "Typedef.hpp"

// Python
#include <Python.h>

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
            inline PyThreadState* getMainThreadState() const { return mainThreadState_; }

            /*!
              \brief Adds an error message to list of errors that occurred in the analysis execution.
              \param errorMessage The error message.
            */
            void addError(const std::string& errorMessage);

            terrama2::core::DataSeriesPtr findDataSeries(const std::string& dataSeriesName);

          protected:
            mutable std::recursive_mutex mutex_; //!< A mutex to synchronize all operations.

            std::weak_ptr<terrama2::services::analysis::core::DataManager> dataManager_;
            AnalysisPtr analysis_;
            std::shared_ptr<te::dt::TimeInstantTZ> startTime_;
            std::set<std::string> errosSet_;

            PyThreadState* mainThreadState_ = nullptr; //!< Python interpreter main thread state.

            std::unordered_map<std::string, terrama2::core::DataSeriesPtr > dataSeriesMap_;
            std::unordered_map<Srid, std::shared_ptr<te::srs::Converter> > converterMap_;
        };

      }
    }
  }
}

#endif // __TERRAMA2_SERVINCES_ANALYSIS_CORE_BASE_CONTEXT_HPP__
