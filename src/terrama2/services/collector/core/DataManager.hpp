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

// TerraMa2
#include "Config.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../core/Typedef.hpp"
#include "../core/Shared.hpp"

namespace terrama2
{
  namespace services
  {
    namespace collector
    {
      namespace core
      {
        /*!
          \brief Manages metadata of terrama2::core::DataProvider, terrama2::core::DataSeries and Collector.

          Extends terrama2::core::DataManager by managing Collector.

          \sa terrama2::core::DataManager
        */
        class TMCOLLECTOREXPORT DataManager : public terrama2::core::DataManager
        {
            Q_OBJECT

          public:
            //! Default constructor
            DataManager() = default;

            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Collector to be added.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "collectors"
            */
            virtual void addJSon(const QJsonObject& obj) override;
            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Collector to be removed.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "collectors"
            */
            virtual void removeJSon(const QJsonObject& obj) override;

            //! Default destructor
            virtual ~DataManager() = default;
            //! Deleted copy constructor
            DataManager(const DataManager& other) = delete;
            //! Deleted copy constructor
            DataManager(DataManager&& other) = delete;
            //! Deleted copy constructor
            DataManager& operator=(const DataManager& other) = delete;
            //! Deleted copy constructor
            DataManager& operator=(DataManager&& other) = delete;

            using terrama2::core::DataManager::add;
            using terrama2::core::DataManager::update;

            /*!
              \brief Register a Collector in the manager.

              At end it will emit collectorAdded(CollectorPtr) signal.

              \param collector The Collector to be registered into the manager.

              \pre The Collector must not have a terrama2::core::InvalidId.
              \pre A Collector with the same name must not be already in the manager.

              \exception terrama2::InvalidArgumentException If it is not possible to add the Collector.

              \note Thread-safe.
            */
            virtual void add(CollectorPtr collector);
            /*!
              \brief Update a given Collector.

              Emits collectorUpdated() signal if the Collector is updated successfully.

              \param dataseries     Collector to update.
              \param shallowSave If true it will update only the dataseries attributes.

              \pre The Collector must not have a terrama2::core::InvalidId.
              \pre The Collector must exist in the DataManager.

              \exception terrama2::InvalidArgumentException If it is not possible to update the Collector.

              \note Thread-safe.
            */
            virtual void update(CollectorPtr collector);
            /*!
              \brief Removes the Collector with the given id.

              Emits collectorRemoved() signal if the DataSeries is removed successfully.

              \param id ID of the Collector to remove.

              \exception terrama2::InvalidArgumentException If it is not possible to remove the Collector.

              \note Thread-safe.
            */
            virtual void removeCollector(CollectorId collectorId);
            /*!
              \brief Retrieves the Collector with the given CollectorId.

              \param id The Collector CollectorId.

              \return DataProviderPtr A smart pointer to the Collector

              \exception terrama2::InvalidArgumentException If some error occur when trying to find the Collector.

              \note Thread-safe.
            */
            virtual CollectorPtr findCollector(CollectorId id) const;

            //! Verify if the Collector already exists in the DataManager
            virtual bool hasCollector(CollectorId id) const;

          signals:
            //! Signal to notify that a Collector has been added.
            void collectorAdded(CollectorPtr);
            //! Signal to notify that a Collector has been updated.
            void collectorUpdated(CollectorPtr);
            //! Signal to notify that a Collector has been removed.
            void collectorRemoved(CollectorId);

          protected:
            std::map<CollectorId, CollectorPtr> collectors_;//!< A map from CollectorId to Collector.
        };
      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_COLLECTOR_CORE_DATAMANAGER_HPP__
