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
  \file terrama2/services/alert/core/DataManager.hpp

  \brief Model class for the alert configuration.

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_DATAMANAGER_HPP__

// TerraMa2
#include "Config.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        /*!
          \brief Manages metadata of terrama2::core::DataProvider, terrama2::core::DataSeries and Alert.

          Extends terrama2::core::DataManager by managing Alert.

          \sa terrama2::core::DataManager
        */
        class TMALERTCOREEXPORT DataManager : public terrama2::core::DataManager
        {
            Q_OBJECT

          public:
            //! Default constructor
            DataManager() = default;

            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Alert to be added.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "alerts"
            */
            virtual void addJSon(const QJsonObject& obj) override;
            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Alert to be removed.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "alerts"
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
              \brief Register a Alert in the manager.

              At end it will emit alertAdded(AlertPtr) signal.

              \param alert The Alert to be registered into the manager.

              \pre The Alert must not have a terrama2::core::InvalidId.
              \pre A Alert with the same name must not be already in the manager.

              \exception terrama2::InvalidArgumentException If it is not possible to add the Alert.

              \note Thread-safe.
            */
            virtual void add(AlertPtr alert);
            /*!
              \brief Update a given Alert.

              Emits alertUpdated() signal if the Alert is updated successfully.

              \param dataseries     Alert to update.

              \pre The Alert must not have a terrama2::core::InvalidId.
              \pre The Alert must exist in the DataManager.

              \exception terrama2::InvalidArgumentException If it is not possible to update the Alert.

              \note Thread-safe.
            */
            virtual void update(AlertPtr alert);
            /*!
              \brief Removes the Alert with the given id.

              Emits alertRemoved() signal if the DataSeries is removed successfully.

              \param id ID of the Alert to remove.

              \exception terrama2::InvalidArgumentException If it is not possible to remove the Alert.

              \note Thread-safe.
            */
            virtual void removeAlert(AlertId alertId);
            /*!
              \brief Retrieves the Alert with the given AlertId.

              \param id The Alert AlertId.

              \return DataProviderPtr A smart pointer to the Alert

              \exception terrama2::InvalidArgumentException If some error occur when trying to find the Alert.

              \note Thread-safe.
            */
            virtual AlertPtr findAlert(AlertId id) const;

            //! Verify if the Alert already exists in the DataManager
            virtual bool hasAlert(AlertId id) const;

          signals:
            //! Signal to notify that a Alert has been added.
            void alertAdded(AlertPtr);
            //! Signal to notify that a Alert has been updated.
            void alertUpdated(AlertPtr);
            //! Signal to notify that a Alert has been removed.
            void alertRemoved(AlertId);

          protected:
            std::map<AlertId, AlertPtr> alerts_;//!< A map from AlertId to Alert.
        };
      } // end namespace core
    }   // end namespace alert
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_ALERT_CORE_DATAMANAGER_HPP__
