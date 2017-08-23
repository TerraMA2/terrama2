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
  \file interpolator/core/Interpolator.hpp

  \brief .

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_DATAMANAGER_HPP__

#include "../../../core/data-model/DataManager.hpp"

#include "Typedef.hpp"

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        /*!
         * \class
         *
         * \brief The DataManager class
         */
        class DataManager : public terrama2::core::DataManager
        {
            Q_OBJECT

          public:

            /*!
              \brief Default constructor
             */
            DataManager() = default;

            /*!
              \brief Default destructor
             */
            ~DataManager() = default;

            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Collector to be added.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "collectors"
            */
            void addJSon(const QJsonObject& obj);

            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Collector to be removed.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "collectors"
            */
            void removeJSon(const QJsonObject& obj);

            /*!
             * \brief DataManager
             * \param other
             */
            DataManager(const DataManager& other) = delete;

            /*!
             * \brief DataManager
             * \param other
             */
            DataManager(DataManager&& other) = delete;

            /*!
             * \brief operator =
             * \param other
             * \return
             */
            DataManager& operator=(const DataManager& other) = delete;

            /*!
             * \brief operator =
             * \param other
             * \return
             */
            DataManager& operator=(DataManager&& other) = delete;

            using terrama2::core::DataManager::add;
            using terrama2::core::DataManager::update;

            /*!
              \brief

              \exception terrama2::InvalidArgumentException If it is not possible to add the Interpolator.

              \note Thread-safe.
            */
            void add(InterpolatorParamsPtr params);

            /*!
              \brief

              \exception terrama2::InvalidArgumentException If it is not possible to update the Interpolator.

              \note Thread-safe.
            */
            void update(InterpolatorParamsPtr params);

            /*!
              \brief .

              \exception terrama2::InvalidArgumentException If it is not possible to remove the Interpolator.

              \note Thread-safe.
            */
            void removeInterpolator(InterpolatorId id);
            /*!
              \brief

              \exception terrama2::InvalidArgumentException If some error occur when trying to find the Interpolator.

              \note Thread-safe.
            */
            InterpolatorParamsPtr findInterpolatorParams(InterpolatorId id) const;

            /*!
             * \brief hasCollector
             * \param id
             * \return
             */
            bool hasInterpolator(InterpolatorId id) const;

          signals:

            void interpolatorAdded(InterpolatorParamsPtr);

            void interpolatorUpdated(InterpolatorParamsPtr);

            void interpolatorRemoved(InterpolatorId);

          protected:
            std::map<InterpolatorId, InterpolatorParamsPtr> interpolatorsParams_;//!< A map from InterpolatorId to Interpolator.
        };
      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_DATAMANAGER_HPP__
