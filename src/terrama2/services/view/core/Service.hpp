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
#include "../../../core/utility/Service.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"
#include "DataManager.hpp"
#include "ViewLogger.hpp"

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

          //! Set ProcessLogger
          void setLogger(std::shared_ptr<ViewLogger> logger) noexcept;

        public slots:

          //! Slot to be called when a DataSetTimer times out.
          virtual void addToQueue(ViewId viewId) noexcept override;

          /*!
            \brief Add a View to the service

            Check if this is the instance where the View should run.
          */
          void addView(ViewPtr view) noexcept;

          /*!
            \brief Updates the View.

            calls addMap()
          */
          void updateView(ViewPtr view) noexcept;

          /*!
            \brief Removes the View.

            Rennuning processes will continue until finished.
          */
          void removeView(ViewId viewId) noexcept;

        protected:

          // comments on base class
          virtual bool hasDataOnQueue() noexcept override;

          // comments on base class
          virtual bool processNextData() override;

          //*! Create a process task and add to taskQueue_
          virtual void prepareTask(ViewId viewId);

          //! Connects signals from DataManager
          void connectDataManager();

          std::weak_ptr<DataManager> dataManager_; //!< Weak pointer to the DataManager

          std::map<ViewId, terrama2::core::TimerPtr> timers_;//!< List of running View timers
          std::deque<ViewId> viewQueue_;//!< View queue
          std::shared_ptr< ViewLogger > logger_;//!< process logger
        };

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
} // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_SERVICE_HPP__
