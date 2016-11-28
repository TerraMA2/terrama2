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
  \file terrama2/services/analysis/core/AnalysisExecutor.hpp

  \brief Prepare context for an analysis execution.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_ANALYSIS_EXECUTOR_HPP__
#define __TERRAMA2_ANALYSIS_CORE_ANALYSIS_EXECUTOR_HPP__

// TerraMA2
#include "Shared.hpp"
#include "AnalysisLogger.hpp"
#include "GridContext.hpp"

// STL
#include <vector>

// QT
#include <QObject>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        // Forward declaration
        struct Analysis;


        class AnalysisExecutor : public QObject
        {
          Q_OBJECT

          public:

            //! Default constructor
            AnalysisExecutor();

            //! Default destructor
            ~AnalysisExecutor() = default;

            //! Default copy constructor
            AnalysisExecutor(const AnalysisExecutor& other) = delete;

            //! Default move constructor
            AnalysisExecutor(AnalysisExecutor&& other) = delete;

            //! Default const assignment operator
            AnalysisExecutor& operator=(const AnalysisExecutor& other) = delete;

            //! Default assignment operator
            AnalysisExecutor& operator=(AnalysisExecutor&& other) = delete;


            /*!
              \brief Starts the process of an analysis execution.
              \param dataManager A smart pointer to the data manager.
              \param logger Smart pointer to the analysis process logger.
              \param startTime Start time of analysis execution.
              \param analysis The analysis to be executed.
              \param threadPool Smart pointer to the thread pool.
            */
            void runAnalysis(DataManagerPtr dataManager,
                             terrama2::core::StoragerManagerPtr storagerManager,
                             std::shared_ptr<terrama2::services::analysis::core::AnalysisLogger> logger,
                             std::shared_ptr<te::dt::TimeInstantTZ> startTime,
                             AnalysisPtr analysis,
                             ThreadPoolPtr threadPool,
                             PyThreadState* mainThreadState);

            /*!
              \brief Prepare the context for a monitored object analysis and run the analysis.
              \param dataManager A smart pointer to the data manager.

              \param threadPool Smart pointer to the thread pool.
            */
            void runMonitoredObjectAnalysis(DataManagerPtr dataManager,
                                            terrama2::core::StoragerManagerPtr storagerManager,
                                            AnalysisPtr analysis,
                                            std::shared_ptr<te::dt::TimeInstantTZ> startTime,
                                            ThreadPoolPtr threadPool,
                                            PyThreadState* mainThreadState);

            /*!
              \brief Prepare the context for a DCP analysis and run the analysis.
              \param dataManager A smart pointer to the data manager.
              \param startTime Start time of analysis execution.
              \param analysis The analysis to be executed.
              \param threadPool Smart pointer to the thread pool.
            */
            void runDCPAnalysis(DataManagerPtr dataManager,
                                terrama2::core::StoragerManagerPtr storagerManager,
                                AnalysisPtr analysis,
                                std::shared_ptr<te::dt::TimeInstantTZ> startTime,
                                ThreadPoolPtr threadPool,
                                PyThreadState* mainThreadState);

            /*!
              \brief Prepare the context for a grid analysis and run the analysis.
              \param dataManager A smart pointer to the data manager.
              \param startTime Start time of analysis execution.
              \param analysis The analysis to be executed.
              \param threadPool Smart pointer to the thread pool.
            */
            void runGridAnalysis(DataManagerPtr shared_ptr,
                                 terrama2::core::StoragerManagerPtr storagerManager,
                                 AnalysisPtr analysis,
                                 std::shared_ptr<te::dt::TimeInstantTZ> startTime ,
                                 ThreadPoolPtr threadPool,
                                 PyThreadState* mainThreadState);

            /*!
              \brief Reads the analysis result from context and stores it to the configured output dataset.
              \param dataManager A smart pointer to the data manager.
            */
            void storeMonitoredObjectAnalysisResult(DataManagerPtr dataManager, terrama2::core::StoragerManagerPtr storagerManager, MonitoredObjectContextPtr context);

            /*!
              \brief Reads the analysis result from context and stores it to the configured output dataset.
              \param dataManager A smart pointer to the data manager.
            */
            void storeGridAnalysisResult(terrama2::core::StoragerManagerPtr storagerManager, terrama2::services::analysis::core::GridContextPtr context);

            /*!
              \brief Verifies if the given analysis in using a inactive data series.
              \note In case there is a inactive data series, this information must be logged but the analysis will be executed normally.
              \param dataManager A smart pointer to the data manager.
              \param logger Smart pointer to the analysis process logger.
              \param analysis The analysis to be executed.
            */
            void verifyInactiveDataSeries(DataManagerPtr dataManager, AnalysisPtr analysis, std::shared_ptr<terrama2::services::analysis::core::AnalysisLogger> logger);

          signals:
            //! Signal to notify that a analysis execution has finished.
            void analysisFinished(int, bool);

        };


      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_ANALYSIS_EXECUTOR_HPP__
