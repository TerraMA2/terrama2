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
  \file terrama2/services/alert/core/Alert.hpp

  \brief Model class for the alert configuration.

  \author Jano Simas
*/

#include "Service.hpp"
#include "Alert.hpp"
#include "AlertLogger.hpp"
#include "RunAlert.hpp"

#include "../../../core/Shared.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"

terrama2::services::alert::core::Service::Service(std::weak_ptr<terrama2::services::alert::core::DataManager> dataManager)
  : dataManager_(dataManager)
{
  connectDataManager();
}

bool terrama2::services::alert::core::Service::hasDataOnQueue() noexcept
{
  return !alertQueue_.empty();
}

bool terrama2::services::alert::core::Service::processNextData()
{
  // check if there is data to collect
  if(alertQueue_.empty())
    return false;

  // get first data
  const auto& alertInfo = alertQueue_.front();

  // prepare task for collecting
  prepareTask(alertInfo);

  // remove from queue
  alertQueue_.pop_front();

  // is there more data to process?
  return !alertQueue_.empty();
}

void terrama2::services::alert::core::Service::prepareTask(std::pair<AlertId, std::shared_ptr<te::dt::TimeInstantTZ> > alertInfo)
{
  try
  {
    taskQueue_.emplace(std::bind(&runAlert, alertInfo, logger_, dataManager_));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::alert::core::Service::addToQueue(AlertId alertId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);
    TERRAMA2_LOG_DEBUG() << tr("Alert added to queue.");

    auto datamanager = dataManager_.lock();
    auto alert = datamanager->findAlert(alertId);

    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this alert should be executed in this instance
    if(alert->serviceInstanceId != serviceInstanceId)
      return;

    alertQueue_.push_back(std::make_pair(alertId, terrama2::core::TimeUtils::nowUTC()));
    mainLoopCondition_.notify_one();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::services::alert::core::Service::connectDataManager()
{
  auto dataManager = dataManager_.lock();
  connect(dataManager.get(), &terrama2::services::alert::core::DataManager::alertAdded, this,
          &terrama2::services::alert::core::Service::addAlert);
  connect(dataManager.get(), &terrama2::services::alert::core::DataManager::alertRemoved, this,
          &terrama2::services::alert::core::Service::removeAlert);
  connect(dataManager.get(), &terrama2::services::alert::core::DataManager::alertUpdated, this,
          &terrama2::services::alert::core::Service::updateAlert);
}

void terrama2::services::alert::core::Service::setLogger(std::shared_ptr<AlertLogger> logger) noexcept
{
  logger_ = logger;
}

void terrama2::services::alert::core::Service::addAlert(AlertPtr alert) noexcept
{
  try
  {
    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this alert should be executed in this instance
    if(alert->serviceInstanceId != serviceInstanceId)
      return;

    try
    {
      if(alert->active)
      {
        std::lock_guard<std::mutex> lock(mutex_);

        std::shared_ptr<te::dt::TimeInstantTZ> lastProcess;
        if(logger_.get())
          lastProcess = logger_->getLastProcessTimestamp(alert->id);

        terrama2::core::TimerPtr timer = createTimer(alert->schedule, alert->id, lastProcess);
        timers_.emplace(alert->id, timer);
      }
    }
    catch(const terrama2::core::InvalidFrequencyException&)
    {
      // invalid schedule, already logged
    }
    catch(const te::common::Exception& e)
    {
      TERRAMA2_LOG_ERROR() << e.what();
    }

    addToQueue(alert->id, terrama2::core::TimeUtils::nowUTC());
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }

}

void terrama2::services::alert::core::Service::removeAlert(AlertId alertId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);


    TERRAMA2_LOG_INFO() << tr("Removing alert %1.").arg(alertId);

    auto it = timers_.find(alertId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(alertId);
      timer->disconnect();
      timers_.erase(alertId);
    }

    // remove from queue
    alertQueue_.erase(std::remove_if(alertQueue_.begin(), alertQueue_.end(),
                                    [alertId](const std::pair<AlertId, std::shared_ptr<te::dt::TimeInstantTZ> >& alertInfo)
                                                { return alertId == alertInfo.first; }), alertQueue_.end());


    TERRAMA2_LOG_INFO() << tr("Alert %1 removed successfully.").arg(alertId);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove alert: %1.").arg(alertId);
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove alert: %1.").arg(alertId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove alert: %1.").arg(alertId);
  }
}

void terrama2::services::alert::core::Service::updateAlert(AlertPtr alert) noexcept
{
  //TODO: addAlert adds to queue, is this expected?
  addAlert(alert);
}
