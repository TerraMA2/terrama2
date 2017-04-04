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

void terrama2::services::collector::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    auto collectorLogger = std::dynamic_pointer_cast<CollectorLogger>(logger_->clone());
    assert(collectorLogger);
    taskQueue_.emplace(std::bind(&terrama2::services::collector::core::Service::collect, this, executionPackage, collectorLogger, dataManager_));
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

    RegisterId registerId = logger_->start(collectorId);

    terrama2::core::ExecutionPackage executionPackage;
    executionPackage.processId = collectorId;
    executionPackage.executionDate = startTime;
    executionPackage.registerId = registerId;

    // if this collector id is already being processed put it on the wait queue
    auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), collectorId);
    if(pqIt == processingQueue_.end())
    {
      processQueue_.push_back(executionPackage);
      processingQueue_.push_back(collectorId);

      //wake loop thread
      mainLoopCondition_.notify_one();
    }
    else
    {
      waitQueue_[collectorId].push(executionPackage);
      logger_->result(CollectorLogger::ON_QUEUE, nullptr, executionPackage.registerId);
      TERRAMA2_LOG_INFO() << tr("Collector %1 added to wait queue.").arg(collectorId);
    }


    mainLoopCondition_.notify_one();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::services::collector::core::Service::collect(terrama2::core::ExecutionPackage executionPackage,
                                                           std::shared_ptr<CollectorLogger> logger,
                                                           std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << tr("Unable to access DataManager");
    notifyWaitQueue(executionPackage.processId);
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
    return;
  }

  try
  {


    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto lock = dataManager->getLock();

    auto collectorPtr = dataManager->findCollector(executionPackage.processId);

    // input data
    auto inputDataSeries = dataManager->findDataSeries(collectorPtr->inputDataSeries);
    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);


    TERRAMA2_LOG_DEBUG() << tr("Starting collection for data series '%1'").arg(inputDataSeries->name.c_str());

    // output data
    auto outputDataSeries = dataManager->findDataSeries(collectorPtr->outputDataSeries);
    auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    //  recovering data

    terrama2::core::Filter filter = collectorPtr->filter;
    //update filter based on last collected data timestamp
    std::shared_ptr<te::dt::TimeInstantTZ> lastCollectedDataTimestamp = logger->getDataLastTimestamp(executionPackage.processId);

    if(lastCollectedDataTimestamp.get() && filter.discardBefore.get())
    {
      if(filter.discardBefore < lastCollectedDataTimestamp)
        filter.discardBefore = lastCollectedDataTimestamp;
    }
    else if(lastCollectedDataTimestamp.get())
      filter.discardBefore = lastCollectedDataTimestamp;

    auto remover = std::make_shared<terrama2::core::FileRemover>();
    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);

    auto uriMap = dataAccessor->getFiles(filter, remover);
    auto dataMap = dataAccessor->getSeries(uriMap, filter, remover);
    if(dataMap.empty())
    {
      QString errMsg = tr("No data to collect.");
      logger->result(CollectorLogger::DONE, nullptr, executionPackage.registerId);
      logger->log(CollectorLogger::WARNING_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      TERRAMA2_LOG_WARNING() << errMsg;

      notifyWaitQueue(executionPackage.processId);
      sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
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
        //FIXME: the datamanager is being used outside the lock
        item.second = processIntersection(dataManager, collectorPtr->intersection, item.second);
      }


      // store each item
      DataSetId outputDataSetId = inputOutputMap.at(item.first->id);
      auto outputDataSet = std::find_if(dataSetLst.cbegin(), dataSetLst.cend(), [outputDataSetId](terrama2::core::DataSetPtr dataSet) { return dataSet->id == outputDataSetId; });
      dataStorager->store(item.second, *outputDataSet);
    }

    TERRAMA2_LOG_INFO() << tr("Data from collector %1 collected successfully.").arg(executionPackage.processId);

    logger->result(CollectorLogger::DONE, lastDateTime, executionPackage.registerId);

    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true);
    notifyWaitQueue(executionPackage.processId);
    return;

  }
  catch(const terrama2::core::LogException& e)
  {
    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    if(executionPackage.registerId != 0 )
    {
      TERRAMA2_LOG_ERROR() << errMsg << std::endl;
      TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);
    }
  }
  catch(const terrama2::core::NoDataException& e)
  {
    TERRAMA2_LOG_INFO() << tr("Collection finished but there was no data available for collector %1.").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(CollectorLogger::WARNING_MESSAGE, tr("No data available").toStdString(), executionPackage.registerId);
      logger->result(CollectorLogger::DONE, nullptr, executionPackage.registerId);
    }

    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true);
    notifyWaitQueue(executionPackage.processId);
    return;
  }
  catch(const terrama2::Exception& e)
  {
    QString errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
    }
  }
  catch(const boost::exception& e)
  {
    std::string errMsg = boost::diagnostic_information(e);
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg, executionPackage.registerId);
      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
    }
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, e.what(), executionPackage.registerId);
      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
    }
  }
  catch(...)
  {
    QString errMsg = tr("Unknown error.");
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
    }

  }

  sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
  notifyWaitQueue(executionPackage.processId);
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
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [collectorId](const terrama2::core::ExecutionPackage& executionPackage)
                                       { return collectorId == executionPackage.processId; }), processQueue_.end());

    waitQueue_.erase(collectorId);


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
