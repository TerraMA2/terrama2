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
 * \file interpolator/core/Service.hpp
 *
 * \brief Defines the service of interpolation class.
 *
 * \author Frederico Augusto Bedê
 */

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__

#include "../../../core/utility/Service.hpp"
#include "../../../core/Typedef.hpp"
#include "../../../core/Shared.hpp"

#include "Interpolator.hpp"
#include "Typedef.hpp"

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
        class InterpolatorLogger;

        /*!
         * \class Service
         *
         * \brief Implementation of the interpolation service.
         *
         * \ingroup interpolator
         */
        class Service : public terrama2::core::Service
        {
          Q_OBJECT

        public:
          /*!
           * \brief Constructor.
           *
           * \param dataManager The manager to be used.
           */
          Service(std::weak_ptr<DataManager> dataManager);

          /*!
           * \brief Destructor.
           */
          ~Service();

          /** @name Copying Methods
          *  Methods related to copy. (Copies are NOT allowed)
          */
          //@{

          /*!
           * \brief Copy constructor.
           */
          Service(const Service&) = delete;

          /*!
           * \brief Copy constructor. (Unsing a pointer as argument).
           */
          Service(Service&&) = default;

          /*!
           * \brief Copy operator
           *
           * \return Does not matters.
           */
          Service& operator=(const Service&) = delete;

          /*!
           * \brief Copy operator. (Unsing a pointer as argument).
           *
           * \return Does not matters.
           */
          Service& operator=(Service&&) = default;
          //@}

        public slots:

          /*!
           * \brief Adds the params to set of params.
           *
           * \param params The parameters to be stored.
           */
          void addInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Adds the service to the execution queue.
           *
           * \param interpolatorId Identifier of the interpolation service.
           *
           * \param startTime Process start timing.
           */
          void addToQueue(InterpolatorId interpolatorId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept override;

          /*!
           * \brief Receive a jSon and update service information with it.
           *
           * \param obj jSon with additional information for service.
           */
          void updateAdditionalInfo(const QJsonObject& obj) noexcept override;

          /*!
           * \brief Reomves the interpolator from the service.
           *
           * \param interpolatorId Identifier of the Interpolator to be removed.
           */
          void removeInterpolator(InterpolatorId interpolatorId) noexcept;

          /*!
           * \brief Updates the information about the \a interpolator.
           *
           * \param interpolator The new parameters to be used.
           */
          void updateInterpolator(InterpolatorParamsPtr interpolator) noexcept;

        protected:

          /*!
           * \brief Prespares the task to be executed.
           *
           * \param executionPackage Informations about the service to be stored.
           */
          virtual void prepareTask(const terrama2::core::ExecutionPackage& executionPackage) override;

          /*!
           * \brief Executes the interpolation service.
           *
           * \param executionPackage Informations about the service.
           *
           * \param logger Object used to store informations about the execution of the process.
           *
           * \param weakDataManager The DataManager object being used.
           */
          void interpolate(terrama2::core::ExecutionPackage executionPackage, std::shared_ptr<InterpolatorLogger> logger,
                           std::weak_ptr<DataManager> weakDataManager);

          /*!
           * \brief Make the connections between the DataManager and the object itself.
           */
          void connectDataManager();

          std::weak_ptr<DataManager> dataManager_; //!< Weak pointer to the DataManager
        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__
