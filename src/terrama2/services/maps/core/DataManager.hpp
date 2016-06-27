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
  \file terrama2/services/MAPS/core/Collector.hpp

  \brief Model class for the maps configuration.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_MAPS_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_SERVICES_MAPS_CORE_DATAMANAGER_HPP__

#include "../../../core/data-model/DataManager.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"

namespace terrama2
{
  namespace services
  {
    namespace maps
    {
      namespace core
      {
        class DataManager : public terrama2::core::DataManager
        {
          Q_OBJECT

        public:

          DataManager() = default;

          //! Default destructor
          virtual ~DataManager() = default;

          DataManager(const DataManager& other) = delete;
          DataManager(DataManager&& other) = delete;
          DataManager& operator=(const DataManager& other) = delete;
          DataManager& operator=(DataManager&& other) = delete;

          using terrama2::core::DataManager::add;
          using terrama2::core::DataManager::update;

          /*!
            \brief Register a map in the manager.

            At end it will emit MapAdded(MapsPtr) signal.

            \param map The map to be registered into the manager.

            \pre The map must not have a terrama2::core::InvalidId.
            \pre A map with the same name must not be already in the manager.

            \exception terrama2::InvalidArgumentException If it is not possible to add the map.

            \note Thread-safe.
          */
          virtual void add(MapsPtr map);

          /*!
            \brief Update a given map.

            Emits MapUpdated() signal if the map is updated successfully.

            \param dataseries     map to update.
            \param shallowSave If true it will update only the dataseries attributes.

            \pre The map must not have a terrama2::core::InvalidId.
            \pre The map must exist in the DataManager.

            \exception terrama2::InvalidArgumentException If it is not possible to update the map.

            \note Thread-safe.
          */
          virtual void update(MapsPtr map);

          /*!
            \brief Removes the map with the given id.

            Emits MapRemoved() signal if the DataSeries is removed successfully.

            \param id ID of the map to remove.

            \exception terrama2::InvalidArgumentException If it is not possible to remove the map.

            \note Thread-safe.
          */
          virtual void removeMap(MapsId mapId);

          /*!
            \brief Retrieves the Map with the given MapsId.

            \param id The map MapsId.

            \return DataProviderPtr A smart pointer to the map

            \exception terrama2::InvalidArgumentException If some error occur when trying to find the map.

            \note Thread-safe.
          */
          virtual MapsPtr findMap(MapsId id) const;

          //! Verify if the Collector already exists in the DataManager
          virtual bool hasMap(MapsId id) const;

          /*!
            \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Maps to be added.

            The valid tags are:
              - "dataproviders"
              - "dataseries"
              - "maps"
          */
          virtual void addJSon(const QJsonObject& obj) override;

          /*!
            \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Maps to be removed.

            The valid tags are:
              - "dataproviders"
              - "dataseries"
              - "maps"
          */
          virtual void removeJSon(const QJsonObject& obj) override;

        signals:
          //! Signal to notify that a Collector has been added.
          void mapAdded(MapsPtr);
          //! Signal to notify that a Collector has been updated.
          void mapUpdated(MapsPtr);
          //! Signal to notify that a Collector has been removed.
          void mapRemoved(MapsId);

        protected:
          std::map<MapsId, MapsPtr> maps_;//!< A map from MapsId to Collector.

        };
      } // end namespace core
    }   // end namespace maps
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_MAPS_CORE_DATAMANAGER_HPP__
