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
  \file terrama2/services/collector/core/Collector.hpp

  \brief Model class for the collector configuration.

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_COLLECTOR_CORE_SERVICE_HPP__
#define __TERRAMA2_SERVICES_COLLECTOR_CORE_SERVICE_HPP__

#include "../../../core/utility/Service.hpp"
#include "../../../core/Typedef.hpp"
#include "../../../core/Shared.hpp"
#include "Shared.hpp"
#include "Typedef.hpp"
#include "DataManager.hpp"
#include "CollectorLogger.hpp"

// STL
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace collector
    {
      namespace core
      {
        class Service : public terrama2::core::Service
        {
            Q_OBJECT

          public:
            Service(std::weak_ptr<DataManager> dataManager);

            ~Service() = default;
            Service(const Service& other) = default;
            Service(Service&& other) = default;
            Service& operator=(const Service& other) = default;
            Service& operator=(Service&& other) = default;

          public slots:
            //! Slot to be called when a DataSetTimer times out.
            void addToQueue(CollectorId collectorId);
            void addCollector(CollectorPtr);
            void updateCollector(CollectorPtr collector);
            void removeCollector(CollectorId collectorId);

          protected:
            // comments on base class
            virtual bool mainLoopWaitCondition() noexcept override;
            // comments on base class
            virtual bool checkNextData() override;

            virtual void prepareTask(CollectorId collectorId);

            static void collect(CollectorId collectorId, std::shared_ptr< CollectorLogger > logger, std::weak_ptr<DataManager> weakDataManager);

            void connectDataManager();

            std::weak_ptr<DataManager> dataManager_;

            std::map<CollectorId, terrama2::core::TimerPtr> timers_;
            std::map<CollectorId, std::shared_ptr< CollectorLogger > > loggers_;
            std::deque<CollectorId> collectorQueue_;
        };

      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_COLLECTOR_CORE_SERVICE_HPP__
