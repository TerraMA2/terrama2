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

//STL
#include "Analysis.hpp"
#include "../Shared.hpp"
#include "../../../core/utility/Service.hpp"

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
              \brief Adds the analysis to the queue of execution and starts the schedule for future executions.
              \param analysisId Analysis identifier.
            */
            void addAnalysis(AnalysisId analysisId);

            /*!
              \brief Removes an analysis from the queue of execution.
              \param analysisId Analysis identifier.
            */
            void removeAnalysis(AnalysisId analysisId);

            /*!
              \brief Updates an analysis in the queue of execution.
              \param analysisId Analysis identifier.
            */
            void updateAnalysis(AnalysisId analysisId);


            void addToQueue(AnalysisId analysisId);

            virtual void updateNumberOfThreads(int) override;

          protected:

            /*!
             \brief Returns if the main loop should continue.
             \return True if there is data to be tasked OR is stop is true.
            */
            virtual bool mainLoopWaitCondition() noexcept override;

            /*!
              \brief Check if there is data to be processed.
              \return True if there is more data to be processed.
             */
            virtual bool checkNextData() override;

            /*!
              \brief Binds the method of execution to the task queue.
              \param analysisId Analysis identifier.
            */
            void prepareTask(uint64_t analysisId);

            /*!
              \brief Connects data manager signals to analysis management methods.
            */
            void connectDataManager();


            std::map<AnalysisId, terrama2::core::TimerPtr> timers_; //!< Map of timers by analysis.
            std::vector<AnalysisId> analysisQueue_; //!< Analysis queue.
            DataManagerPtr dataManager_; //!< Data manager.

        };

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_SERVICE_HPP__
