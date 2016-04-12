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

#ifndef __TERRAMA2_SERVICES_COLLECTOR_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_SERVICES_COLLECTOR_CORE_DATAMANAGER_HPP__

#include "../../../core/data-model/DataManager.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"

namespace terrama2
{
  namespace services
  {
    namespace collector
    {
      namespace core
      {
        class DataManager : public terrama2::core::DataManager
        {
            Q_OBJECT

          public:
            DataManager() {}

            virtual void addFromJSON(const QJsonValue& jsonValue) override;

            virtual ~DataManager() {}
            DataManager(const DataManager& other) = delete;
            DataManager(DataManager&& other) = delete;
            DataManager& operator=(const DataManager& other) = delete;
            DataManager& operator=(DataManager&& other) = delete;

            using terrama2::core::DataManager::add;
            using terrama2::core::DataManager::update;

            virtual void add(CollectorPtr collector);
            virtual void update(CollectorPtr collector);
            virtual void removeCollector(CollectorId collectorId);
            virtual CollectorPtr findCollector(CollectorId id) const;

          signals:
            void collectorAdded(CollectorPtr);
            void collectorUpdated(CollectorPtr);
            void collectorRemoved(CollectorId);

          protected:
            std::map<CollectorId, CollectorPtr> collectors_;
        };
      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_COLLECTOR_CORE_DATAMANAGER_HPP__
