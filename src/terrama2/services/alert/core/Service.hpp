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
  \file terrama2/services/alert/core/Service.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_SERVICE_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_SERVICE_HPP__

#include "../../../core/utility/Service.hpp"
#include "../../../core/Typedef.hpp"
#include "../../../core/Shared.hpp"
#include "Shared.hpp"
#include "Typedef.hpp"
#include "DataManager.hpp"
#include "AlertLogger.hpp"
#include "AlertExecutor.hpp"

// STL
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace alert
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


          public slots:
            //! Slot to be called when a DataSetTimer times out.
            virtual void addToQueue(AlertId alertId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept override;

            /*!
              \brief Updates the Alert.

              calls addProcessToSchedule()
            */
            void updateAlert(AlertPtr alert) noexcept;
            /*!
              \brief Removes the Alert.

              Rennuning processes will continue until finished.
            */
            void removeAlert(AlertId alertId) noexcept;

            /*!
             * \brief Receive a jSon and update service information with it
             * \param obj jSon with additional information for service
             */
            virtual void updateAdditionalInfo(const QJsonObject& obj) noexcept override;

            /*!
              \brief Nofifies that an alert has ended.
              \param AlertId Alert identifier.
              \param success Alert finsished with success.
            */
            void alertFinished(AlertId alertId,
                               std::shared_ptr< te::dt::TimeInstantTZ > executionDate,
                               bool success,
                               QJsonObject jsonAnswer = QJsonObject());

          protected:

            //*! Create a process task and add to taskQueue_
            virtual void prepareTask(const terrama2::core::ExecutionPackage& executionPackage) override;

            //! Connects signals from DataManager
            void connectDataManager();

            std::weak_ptr<DataManager> dataManager_; //!< Weak pointer to the DataManager
            std::map<std::string, std::string> serverMap_;
            AlertExecutor alertExecutor_;
        };

      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_COLLECTOR_CORE_SERVICE_HPP__
