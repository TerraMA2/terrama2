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
#include "AlertExecutor.hpp"

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

terrama2::services::alert::core::Service::Service(std::weak_ptr<terrama2::core::DataManager> dataManager)
 : terrama2::core::Service(dataManager)
{
  connectDataManager();
  connect(&alertExecutor_, &AlertExecutor::alertFinished, this, &Service::alertFinished);
}


void terrama2::services::alert::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    auto dataManager = std::static_pointer_cast<terrama2::services::alert::core::DataManager>(dataManager_.lock());
    taskQueue_.emplace(std::bind(&core::AlertExecutor::runAlert, std::ref(alertExecutor_), executionPackage, std::static_pointer_cast<terrama2::services::alert::core::AlertLogger>(logger_), dataManager, serverMap_));
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::alert::core::Service::connectDataManager()
{
  auto dataManager = std::static_pointer_cast<terrama2::services::alert::core::DataManager>(dataManager_.lock());
  connect(dataManager.get(), &terrama2::services::alert::core::DataManager::alertAdded, this,
          &terrama2::services::alert::core::Service::addProcessToSchedule);
  connect(dataManager.get(), &terrama2::services::alert::core::DataManager::alertRemoved, this,
          &terrama2::services::alert::core::Service::removeAlert);
  connect(dataManager.get(), &terrama2::services::alert::core::DataManager::alertUpdated, this,
          &terrama2::services::alert::core::Service::updateAlert);
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
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [alertId](const terrama2::core::ExecutionPackage& executionPackage)
    { return alertId == executionPackage.processId; }), processQueue_.end());

    auto itWaitQueue = waitQueue_.find(alertId);
    if(itWaitQueue != waitQueue_.end())
      waitQueue_.erase(itWaitQueue);


    TERRAMA2_LOG_INFO() << tr("Alert %1 removed successfully.").arg(alertId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove alert: %1.").arg(alertId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
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
  removeAlert(alert->id);
  addProcessToSchedule(alert);
}

void terrama2::services::alert::core::Service::updateAdditionalInfo(const QJsonObject& obj) noexcept
{
  if(!obj.contains("email_server"))
  {
    TERRAMA2_LOG_ERROR() << tr("Missing the Email Server URI in service additional info!");
  }
  else
  {
    serverMap_.emplace("email_server", obj["email_server"].toString().toStdString());
  }
}

void terrama2::services::alert::core::Service::alertFinished(AlertId alertId,
                                                             std::shared_ptr< te::dt::TimeInstantTZ > executionDate,
                                                             bool success,
                                                             QJsonObject jsonAnswer)
{
  notifyWaitQueue(alertId);
  sendProcessFinishedSignal(alertId, executionDate, success, jsonAnswer);
}

terrama2::core::ProcessPtr terrama2::services::alert::core::Service::getProcess(ProcessId processId)
{
  auto dataManager = std::static_pointer_cast<terrama2::services::alert::core::DataManager>(dataManager_.lock());
  return dataManager->findAlert(processId);
}
