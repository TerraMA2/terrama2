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
  \file interpolator/core/Service.hpp

  \brief

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__

#include "../../../core/utility/Service.hpp"
#include "../../../core/Typedef.hpp"
#include "../../../core/Shared.hpp"

#include "Typedef.hpp"

#include "Interpolator.hpp"

// STL
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        // Forward declarations
        class DataManager;
//        struct Interpolator;
        class InterpolatorLogger;

        /*!
         * \class
         *
         * \brief The Service class
         */
        class Service : public terrama2::core::Service
        {
          Q_OBJECT

        public:
          /*!
           * \brief Service
           * \param dataManager
           */
          Service(std::weak_ptr<DataManager> dataManager);

          /*!
           * \brief
           */
          ~Service() = default;

          /*!
           * \brief Service
           * \param other
           */
          Service(const Service& other) = delete;

          /*!
           * \brief
           * \param other
           */
          Service(Service&& other) = default;

          /*!
           * \brief operator =
           * \param other
           * \return
           */
          Service& operator=(const Service& other) = delete;

          /*!
           * \brief
           * \param
           * \return
           */
          Service& operator=(Service&& other) = default;

        public slots:

          /*!
           * \brief addToQueue
           * \param interpolatorId
           * \param startTime
           */
          void addToQueue(InterpolatorId interpolatorId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept override;

          /*!
           * \brief Receive a jSon and update service information with it
           * \param obj jSon with additional information for service
           */
          void updateAdditionalInfo(const QJsonObject& obj) noexcept override;

          /*!
           * \brief removeInterpolator
           * \param interpolatorId
           */
          void removeInterpolator(InterpolatorId interpolatorId) noexcept;

          /*!
           * \brief updateInterpolator
           * \param interpolator
           */
          void updateInterpolator(InterpolatorPtr interpolator) noexcept;

        protected:

          //*! Create a process task and add to taskQueue_
          virtual void prepareTask(const terrama2::core::ExecutionPackage& executionPackage) override;

          /*!
           * \brief interpolate
           * \param executionPackage
           * \param logger
           * \param weakDataManager
           */
          void interpolate(terrama2::core::ExecutionPackage executionPackage, std::shared_ptr<InterpolatorLogger> logger,
                           std::weak_ptr<DataManager> weakDataManager);

          /*!
           * \brief connectDataManager
           */
          void connectDataManager();

          std::weak_ptr<DataManager> dataManager_; //!< Weak pointer to the DataManager
        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__
