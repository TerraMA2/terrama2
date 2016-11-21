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
  \file terrama2/services/analysis/core/Service.hpp

  \brief Service class to the analysis module.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_SERVICE_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_SERVICE_HPP__

#include "AnalysisLogger.hpp"
#include "Shared.hpp"
#include "../../../core/utility/Service.hpp"
#include "../../../core/Shared.hpp"
#include "python/PythonInterpreter.hpp"
#include "AnalysisExecutor.hpp"

//STL
#include <memory>
#include <map>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        class DataManager;

        /*!
           \brief Service class to the analysis module.
         */
        class Service : public terrama2::core::Service
        {
            Q_OBJECT

          public:
            //! Constructor
            Service(DataManagerPtr dataManager);

            //! Destructor
            virtual ~Service();

          public slots:

            /*!
              \brief Removes an analysis from the queue of execution.
              \param analysisId Analysis identifier.
            */
            void removeAnalysis(AnalysisId analysisId) noexcept;

            /*!
              \brief Updates an analysis in the queue of execution.
              \param analysisId Analysis identifier.
            */
            void updateAnalysis(AnalysisPtr analysis) noexcept;

            /*!
              \brief Nofifies that an analysis has ended.
              \param analysisId Analysis identifier.
              \param success Analysis finsished with success.
            */
            void analysisFinished(AnalysisId analysisId, bool success);

            /*!
              \brief Adds the analysis to the queue of execution.
             */
            virtual void addToQueue(AnalysisId analysisId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept override;


            /*!
               \brief Starts the server.
               \param threadNumber Number of threads to process tasks.

               Starts the server, starts to process waiting tasks.

               If the number of threads is 0 (default), this method will try to identify the number of processors,
               if it's not possible, only one thread will be created.

               Initializes the thread pool for analysis execution.

             */
            virtual void start(size_t threadNumber = 0) override;

            /*!
             * \brief Receive a jSon and update service information with it
             * \param obj jSon with additional information for service
             */
            virtual void updateAdditionalInfo(const QJsonObject& obj) noexcept override;

          protected:

            /*!
             \brief Returns if the main loop should continue.
             \return True if there is data to be tasked OR is stop is true.
            */
            virtual bool hasDataOnQueue() noexcept override;

            /*!
              \brief Check if there is data to be processed.
              \return True if there is more data to be processed.
             */
            virtual bool processNextData() override;

            /*!
              \brief Binds the method of execution to the task queue.
              \param analysisId Analysis identifier.
              \param startTime Start time of the analysis execution.
            */
            void prepareTask(AnalysisId analysisId, std::shared_ptr<te::dt::TimeInstantTZ> startTime);

            /*!
              \brief Connects data manager signals to analysis management methods.
            */
            void connectDataManager();

            PyThreadState* mainThreadState_; //!< Main thread state from Python interpreter.
            std::vector<std::pair<AnalysisId, std::shared_ptr<te::dt::TimeInstantTZ> > > analysisQueue_; //!< Analysis queue.
            DataManagerPtr dataManager_; //!< Data manager.
            ThreadPoolPtr threadPool_; //!< Pool of thread to run the analysis.
            terrama2::core::StoragerManagerPtr storagerManager_; //!< Manager to control the storage of analysis results.
            AnalysisExecutor analysisExecutor_; //! Analysis executor object.
        };

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_SERVICE_HPP__
