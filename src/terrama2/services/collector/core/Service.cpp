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
  \file terrama2/services/collector/core/Service.cpp

  \brief

  \author Jano Simas
*/

#include "Service.hpp"
#include "Collector.hpp"
#include "CollectorLogger.hpp"
#include "IntersectionOperation.hpp"

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
#include "../../../core/utility/FileRemover.hpp"

terrama2::services::collector::core::Service::Service(std::weak_ptr<terrama2::services::collector::core::DataManager> dataManager)
  : dataManager_(dataManager)
{
  connectDataManager();
}

bool terrama2::services::collector::core::Service::hasDataOnQueue() noexcept
{
  return !collectorQueue_.empty();
}

bool terrama2::services::collector::core::Service::processNextData()
{
  // check if there is data to collect
  if(collectorQueue_.empty())
    return false;

  // get first data
  const auto& collectorId = collectorQueue_.front();

  // prepare task for collecting
  prepareTask(collectorId);

  // remove from queue
  collectorQueue_.pop_front();

  // is there more data to process?
  return !collectorQueue_.empty();
}

void terrama2::services::collector::core::Service::prepareTask(CollectorId collectorId)
{
  try
  {
    taskQueue_.emplace(std::bind(&terrama2::services::collector::core::Service::collect, this, collectorId, std::dynamic_pointer_cast<CollectorLogger>(logger_), dataManager_));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::collector::core::Service::addToQueue(CollectorId collectorId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);

    auto datamanager = dataManager_.lock();
    auto collector = datamanager->findCollector(collectorId);

    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this collector should be executed in this instance
    if(collector->serviceInstanceId != serviceInstanceId)
      return;


    // if this collector id is already being processed put it on the wait queue
    auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), collectorId);
    if(pqIt == processingQueue_.end())
    {
      auto pair = std::make_pair(collectorId, startTime);
      collectorQueue_.push_back(collectorId);
      processingQueue_.push_back(collectorId);
      TERRAMA2_LOG_DEBUG() << tr("Collector %1 added to processing queue.").arg(collectorId);

      //wake loop thread
      mainLoopCondition_.notify_one();
    }
    else
    {
      waitQueue_[collectorId].push(startTime);
      TERRAMA2_LOG_DEBUG() << tr("Collector %1 added to wait queue.").arg(collectorId);
    }


    mainLoopCondition_.notify_one();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::services::collector::core::Service::collect(CollectorId collectorId,
                                                           std::shared_ptr< terrama2::services::collector::core::CollectorLogger > logger,
                                                           std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << tr("Unable to access DataManager");
    notifyWaitQueue(collectorId);
    sendProcessFinishedSignal(collectorId, false);
    return;
  }

  if(!logger.get())
  {
    QString errMsg = QObject::tr("Unable to access Logger class in collector %1").arg(collectorId);
    TERRAMA2_LOG_ERROR() << errMsg;
    notifyWaitQueue(collectorId);
    sendProcessFinishedSignal(collectorId, false);
    return;
  }

  RegisterId logId = 0;
  try
  {
    TERRAMA2_LOG_DEBUG() << tr("Starting collector");

    logId = logger->start(collectorId);

    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto lock = dataManager->getLock();

    auto collectorPtr = dataManager->findCollector(collectorId);

    // input data
    auto inputDataSeries = dataManager->findDataSeries(collectorPtr->inputDataSeries);
    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

    // output data
    auto outputDataSeries = dataManager->findDataSeries(collectorPtr->outputDataSeries);
    auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    //  recovering data

    terrama2::core::Filter filter = collectorPtr->filter;
    //update filter based on last collected data timestamp
    std::shared_ptr<te::dt::TimeInstantTZ> lastCollectedDataTimestamp = logger->getDataLastTimestamp(collectorId);

    if(lastCollectedDataTimestamp.get() && filter.discardBefore.get())
    {
      if(filter.discardBefore < lastCollectedDataTimestamp)
        filter.discardBefore = lastCollectedDataTimestamp;
    }
    else if(lastCollectedDataTimestamp.get())
      filter.discardBefore = lastCollectedDataTimestamp;

    auto remover = std::make_shared<terrama2::core::FileRemover>();
    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);
    auto dataMap = dataAccessor->getSeries(filter, remover);
    if(dataMap.empty())
    {
      QString errMsg = tr("No data to collect.");
      logger->result(CollectorLogger::DONE, nullptr, logId);
      logger->log(CollectorLogger::WARNING_MESSAGE, errMsg.toStdString(), logId);
      TERRAMA2_LOG_WARNING() << errMsg;

      notifyWaitQueue(collectorId);
      sendProcessFinishedSignal(collectorId, false);
      return;
    }
    auto lastDateTime = dataAccessor->lastDateTime();

    /////////////////////////////////////////////////////////////////////////
    // storing data

    auto inputOutputMap = collectorPtr->inputOutputMap;
    auto dataSetLst = outputDataSeries->datasetList;
    auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(outputDataSeries->semantics.dataFormat, outputDataProvider);
    for(auto& item : dataMap)
    {
      // intersection
      if(collectorPtr->intersection)
      {
        //FIXME: the datamanager is beeing used outside the lock
        item.second = processIntersection(dataManager, collectorPtr->intersection, item.second);
      }


      // store each item
      DataSetId outputDataSetId = inputOutputMap.at(item.first->id);
      auto outputDataSet = std::find_if(dataSetLst.cbegin(), dataSetLst.cend(), [outputDataSetId](terrama2::core::DataSetPtr dataSet) { return dataSet->id == outputDataSetId; });
      dataStorager->store(item.second, *outputDataSet);
    }

    TERRAMA2_LOG_INFO() << tr("Data from collector %1 collected successfully.").arg(collectorId);

    logger->result(CollectorLogger::DONE, lastDateTime, logId);


    sendProcessFinishedSignal(collectorId, true);
    notifyWaitQueue(collectorId);
    return;

  }
  catch(const terrama2::Exception& e)
  {
    QString errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(collectorId);

    if(logId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg.toStdString(), logId);
      logger->result(CollectorLogger::ERROR, nullptr, logId);
    }
  }
  catch(const boost::exception& e)
  {
    std::string errMsg = boost::diagnostic_information(e);
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(collectorId);

    if(logId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg, logId);
      logger->result(CollectorLogger::ERROR, nullptr, logId);
    }
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(collectorId);

    if(logId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, e.what(), logId);
      logger->result(CollectorLogger::ERROR, nullptr, logId);
    }
  }
  catch(...)
  {
    QString errMsg = tr("Unknown error.");
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(collectorId);

    if(logId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg.toStdString(), logId);
      logger->result(CollectorLogger::ERROR, nullptr, logId);
    }
  }

  sendProcessFinishedSignal(collectorId, false);
  notifyWaitQueue(collectorId);

}

void terrama2::services::collector::core::Service::notifyWaitQueue(CollectorId collectorId)
{
  // Remove from processing queue
  auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), collectorId);
  if(pqIt != processingQueue_.end())
    processingQueue_.erase(pqIt);

  // Verify if the there is an process waiting for the same collector id
  if(!waitQueue_[collectorId].empty())
  {
    waitQueue_[collectorId].pop();

    // Adds to the processing queue
    processingQueue_.push_back(collectorId);
    collectorQueue_.push_back(collectorId);

    //wake loop thread
    mainLoopCondition_.notify_one();
  }
}

void terrama2::services::collector::core::Service::connectDataManager()
{
  auto dataManager = dataManager_.lock();
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorAdded, this,
          &terrama2::services::collector::core::Service::addProcessToSchedule);
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorRemoved, this,
          &terrama2::services::collector::core::Service::removeCollector);
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorUpdated, this,
          &terrama2::services::collector::core::Service::updateCollector);
}

void terrama2::services::collector::core::Service::removeCollector(CollectorId collectorId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);


    TERRAMA2_LOG_INFO() << tr("Removing collector %1.").arg(collectorId);

    auto it = timers_.find(collectorId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(collectorId);
      timer->disconnect();
      timers_.erase(collectorId);
    }

    // remove from queue
    collectorQueue_.erase(std::remove(collectorQueue_.begin(), collectorQueue_.end(), collectorId), collectorQueue_.end());


    TERRAMA2_LOG_INFO() << tr("Collector %1 removed successfully.").arg(collectorId);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove collector: %1.").arg(collectorId);
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove collector: %1.").arg(collectorId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove collector: %1.").arg(collectorId);
  }
}

void terrama2::services::collector::core::Service::updateCollector(CollectorPtr collector) noexcept
{
  removeCollector(collector->id);
  addProcessToSchedule(collector);
}

void terrama2::services::collector::core::Service::updateAdditionalInfo(const QJsonObject& obj) noexcept
{

}
