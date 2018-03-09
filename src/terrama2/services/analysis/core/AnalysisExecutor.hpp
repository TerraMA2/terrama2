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
#include "Config.hpp"
#include "Shared.hpp"
#include "AnalysisLogger.hpp"
#include "GridContext.hpp"
#include "MonitoredObjectContext.hpp"
#include "../../../core/utility/Service.hpp"

// STL
#include <vector>

// Boost
#include <boost/any.hpp>

// QT
#include <QObject>

// Python
#ifdef _DEBUG
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif

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


        /*
          \brief Class that executes the analysis

          The AnalysisExecutor identify and execute the appropriate analysis type.
        */
        class TMANALYSISEXPORT AnalysisExecutor : public QObject
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
              \param logger Analysis process logger.
              \param startTime Start time of analysis execution.
              \param analysis The analysis to be executed.
              \param threadPool Smart pointer to the thread pool.
            */
            void runAnalysis(DataManagerPtr dataManager,
                             terrama2::core::StoragerManagerPtr storagerManager,
                             std::shared_ptr<AnalysisLogger> logger,
                             const terrama2::core::ExecutionPackage& executionPackage,
                             AnalysisPtr analysis,
                             ThreadPoolPtr threadPool,
                             PyThreadState* mainThreadState);

            /*!
              \brief Validates the parameters of an analysis.
              \param dataManager A smart pointer to the data manager.
              \param dataManager The analysis configuration.
              \return The validation result.
            */
            ValidateResult validateAnalysis(DataManagerPtr dataManager, AnalysisPtr analysis);

          signals:
            //! Signal to notify that a analysis execution has finished.
            void analysisFinished(size_t, std::shared_ptr< te::dt::TimeInstantTZ >, bool, QJsonObject = QJsonObject());

          private:
            const std::string EXECUTION_DATE_PROPERTY = "execution_date";
            const std::string ALTERNATE_EXECUTION_DATE_PROPERTY = "execution_date_1";


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


            std::shared_ptr<te::mem::DataSet> addDataToDataSet(std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> moDsContext,
                                                               std::shared_ptr<te::da::DataSetType> dt,
                                                               te::da::PrimaryKey* pkMonitoredObject,
                                                               te::dt::Property* identifierProperty,
                                                               std::unordered_map<int, std::map<std::string, boost::any> > resultMap,
                                                               std::shared_ptr<te::dt::TimeInstantTZ>  date);
            std::shared_ptr<te::da::DataSetType> createDatasetType(std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> moDsContext,
                                                                   te::da::PrimaryKey* pkMonitoredObject,
                                                                   te::dt::Property* identifierProperty,
                                                                   std::set<std::pair<std::string, int> > attributes,
                                                                   std::string outputDatasetName);
        };


      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_ANALYSIS_EXECUTOR_HPP__
