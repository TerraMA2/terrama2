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
  \file terrama2/services/collector/core/Collector.hpp

  \brief Model class for the collector configuration.

  \author Jano Simas
*/

#include "Service.hpp"
#include "Collector.hpp"
#include "CollectorLogger.hpp"

#include "../../../core/Shared.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"

terrama2::services::collector::core::Service::Service(std::weak_ptr<terrama2::services::collector::core::DataManager> dataManager)
  : dataManager_(dataManager)
{
  connectDataManager();
}

void terrama2::services::collector::core::Service::updateNumberOfThreads(int numberOfThreads)
{
  //TODO: review updateNumberOfThreads. launch and join as needed instead of stop?
  stop();
  start(numberOfThreads);
}


bool terrama2::services::collector::core::Service::mainLoopWaitCondition() noexcept
{
  return !collectorQueue_.empty() || stop_;
}

bool terrama2::services::collector::core::Service::checkNextData()
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
    taskQueue_.emplace(std::bind(&collect, collectorId, loggers_.at(collectorId), dataManager_));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::collector::core::Service::addToQueue(CollectorId collectorId)
{
  std::lock_guard<std::mutex> lock(mutex_);
  TERRAMA2_LOG_DEBUG() << tr("Collector added to queue.");

  collectorQueue_.push_back(collectorId);
  mainLoopCondition_.notify_one();
}

void terrama2::services::collector::core::Service::collect(CollectorId collectorId, std::shared_ptr< terrama2::services::collector::core::CollectorLogger > logger, std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << tr("Unable to access DataManager");
    return;
  }

  try
  {
    auto logId = logger->start();

    TERRAMA2_LOG_DEBUG() << tr("Starting collector");

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
    dataManager.reset();

    /////////////////////////////////////////////////////////////////////////
    //  recovering data

    terrama2::core::Filter filter = collectorPtr->filter;
    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);
    auto dataMap = dataAccessor->getSeries(filter);
    if(dataMap.empty())
    {
      logger->done(nullptr, logId);
      TERRAMA2_LOG_WARNING() << tr("No data to collect.");
      return;
    }
    auto lastDateTime = dataAccessor->lastDateTime();

    /////////////////////////////////////////////////////////////////////////
    // storing data

    auto inputOutputMap = collectorPtr->inputOutputMap;
    auto dataSetLst = outputDataSeries->datasetList;
    auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(outputDataProvider);
    for(const auto& item : dataMap)
    {
      // store each item
      DataSetId outputDataSetId = inputOutputMap.at(item.first->id);
      auto outputDataSet = std::find_if(dataSetLst.cbegin(), dataSetLst.cend(), [outputDataSetId](terrama2::core::DataSetPtr dataSet) { return dataSet->id == outputDataSetId; });
      dataStorager->store(item.second, *outputDataSet);
    }

    logger->done(lastDateTime, logId);
  }
  catch(const terrama2::Exception& e)
  {
    // should have been logged on emition
  }
  catch(const boost::exception& e)
  {

    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unkonwn error.");
  }
}

void terrama2::services::collector::core::Service::connectDataManager()
{
  auto dataManager = dataManager_.lock();
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorAdded, this,
          &terrama2::services::collector::core::Service::addCollector);
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorRemoved, this,
          &terrama2::services::collector::core::Service::removeCollector);
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorUpdated, this,
          &terrama2::services::collector::core::Service::updateCollector);
}

void terrama2::services::collector::core::Service::addCollector(CollectorPtr collector)
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);

    std::map<std::string, std::string> connInfo = terrama2::core::ServiceManager::getInstance().logConnectionInfo();
    std::shared_ptr< CollectorLogger > collectorLog = std::make_shared<CollectorLogger>(collector->id, connInfo);
    loggers_.emplace(collector->id, collectorLog);

    terrama2::core::TimerPtr timer = std::make_shared<const terrama2::core::Timer>(collector->schedule, collector->id, collectorLog);
    connect(timer.get(), &terrama2::core::Timer::timeoutSignal, this, &terrama2::services::collector::core::Service::addToQueue, Qt::UniqueConnection);
    timers_.emplace(collector->id, timer);
  }
  catch(terrama2::core::InvalidFrequencyException& e)
  {
    // invalid schedule, already logged
  }

  addToQueue(collector->id);
}

void terrama2::services::collector::core::Service::removeCollector(CollectorId collectorId)
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);

    auto timer = timers_.at(collectorId);
    timer->disconnect();
    timers_.erase(collectorId);

    // remove from queue
    collectorQueue_.erase(std::remove(collectorQueue_.begin(), collectorQueue_.end(), collectorId), collectorQueue_.end());
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
  }
}

void terrama2::services::collector::core::Service::updateCollector(CollectorPtr collector)
{
  // Only the Id of the collector is stored, no need to update
}
