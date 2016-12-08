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
  \file terrama2/services/view/core/Service.hpp

  \brief Class for the view configuration.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_SERVICE_HPP__
#define __TERRAMA2_SERVICES_VIEW_SERVICE_HPP__

// TerraMA2
#include "Typedef.hpp"
#include "Shared.hpp"
#include "DataManager.hpp"
#include "ViewLogger.hpp"
#include "../../../core/utility/Service.hpp"

// Qt
#include "QFileInfoList"

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        class Service : public terrama2::core::Service
        {
          Q_OBJECT

        public:

          explicit Service(std::weak_ptr<DataManager> dataManager);

          ~Service() = default;
          Service(const Service& other) = delete;
          Service(Service&& other) = default;
          Service& operator=(const Service& other) = delete;
          Service& operator=(Service&& other) = default;

        public slots:

          //! Slot to be called when a DataSetTimer times out.
          virtual void addToQueue(ViewId viewId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept override;

          /*!
            \brief Updates the View.

            calls addView()
          */
          void updateView(ViewPtr view) noexcept;

          /*!
            \brief Removes the View.

            Rennuning processes will continue until finished.
          */
          void removeView(ViewId viewId) noexcept;

          /*!
            \brief Verifies if there is job to be done in the waiting queue and add it to the processing queue.
          */
          void notifyWaitQueue(ViewId viewId);

          /*!
           * \brief Receive a jSon and update service information with it
           * \param obj jSon with additional information for service
           */
          virtual void updateAdditionalInfo(const QJsonObject& obj) noexcept override;

        protected:

          // comments on base class
          virtual bool hasDataOnQueue() noexcept override;

          // comments on base class
          virtual bool processNextData() override;

          //*! Create a process task and add to taskQueue_
          virtual void prepareTask(ViewId viewId);

          //! Connects signals from DataManager
          void connectDataManager();

          /*!
           * \brief viewJob
           * \param viewId
           * \param logger A copy of a logger object, to avoid errors if it changes during the job
           * \param weakDataManager
           */
          void viewJob(ViewId viewId,
                       ViewLogger logger,
                       std::weak_ptr<DataManager> weakDataManager);

          std::weak_ptr<DataManager> dataManager_; //!< Weak pointer to the DataManager
          std::deque<ViewId> viewQueue_;//!< View queue
          te::core::URI mapsServerUri_;
        };

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
} // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_SERVICE_HPP__
