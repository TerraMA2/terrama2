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
  \file terrama2/services/maps/core/Service.hpp

  \brief Class for the maps configuration.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_MAPS_SERVICE_HPP__
#define __TERRAMA2_SERVICES_MAPS_SERVICE_HPP__

// TerraMA2
#include "../../../core/utility/Service.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"
#include "DataManager.hpp"
#include "MapsLogger.hpp"

namespace terrama2
{
  namespace services
  {
    namespace maps
    {
      namespace core
      {
        class Service : public terrama2::core::Service
        {
          Q_OBJECT

        public:

          Service(std::weak_ptr<DataManager> dataManager);

          ~Service() = default;
          Service(const Service& other) = delete;
          Service(Service&& other) = default;
          Service& operator=(const Service& other) = delete;
          Service& operator=(Service&& other) = default;

          //! Set ProcessLogger
          void setLogger(std::shared_ptr<MapsLogger> logger) noexcept;

        public slots:

          //! Slot to be called when a DataSetTimer times out.
          virtual void addToQueue(MapsId mapsId) noexcept override;

          /*!
            \brief Add a Map to the service

            Check if this is the instance where the Maps should run.
          */
          void addMap(MapsPtr map) noexcept;

          /*!
            \brief Updates the Map.

            calls addMap()
          */
          void updateMap(MapsPtr map) noexcept;

          /*!
            \brief Removes the Map.

            Rennuning processes will continue until finished.
          */
          void removeMap(MapsId mapId) noexcept;

        protected:

          // comments on base class
          virtual bool hasDataOnQueue() noexcept override;

          // comments on base class
          virtual bool processNextData() override;

          //*! Create a process task and add to taskQueue_
          virtual void prepareTask(MapsId mapId);

          static void makeMap(MapsId mapId, std::shared_ptr< terrama2::services::maps::core::MapsLogger > logger, std::weak_ptr<DataManager> weakDataManager);

          //! Connects signals from DataManager
          void connectDataManager();

          std::weak_ptr<DataManager> dataManager_; //!< Weak pointer to the DataManager

          std::map<MapsId, terrama2::core::TimerPtr> timers_;//!< List of running Maps timers
          std::deque<MapsId> mapsQueue_;//!< Maps queue
          std::shared_ptr< MapsLogger > logger_;//!< process logger
        };

      } // end namespace core
    }   // end namespace maps
  }     // end namespace services
} // end namespace terrama2

#endif // __TERRAMA2_SERVICES_MAPS_SERVICE_HPP__
