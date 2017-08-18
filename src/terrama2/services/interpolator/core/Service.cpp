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
  \file terrama2/services/interpolator/core/Service.cpp

  \brief

  \author Frederico Augusto Bedê
*/

#include "Service.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/ServiceManager.hpp"
#include "../../../core/Exception.hpp"

#include "DataManager.hpp"
#include "InterpolatorFactories.h"
#include "InterpolatorLogger.hpp"

terrama2::services::interpolator::core::Service::Service(std::weak_ptr<terrama2::services::interpolator::core::DataManager> dataManager):
  dataManager_(dataManager)
{
  connectDataManager();

  InterpolatorFactories::initialize();
}

terrama2::services::interpolator::core::Service::~Service()
{
  InterpolatorFactories::finalize();
}

//void terrama2::services::interpolator::core::Service::addInterpolator(const terrama2::services::interpolator::core::InterpolatorParamsPtr& params)
//{

//}

void terrama2::services::interpolator::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    auto interpolatorLogger = std::dynamic_pointer_cast<InterpolatorLogger>(logger_->clone());
    assert(interpolatorLogger);
    taskQueue_.emplace(std::bind(&terrama2::services::interpolator::core::Service::interpolate, this, executionPackage, interpolatorLogger, dataManager_));
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::interpolator::core::Service::interpolate(terrama2::core::ExecutionPackage executionPackage, std::shared_ptr<InterpolatorLogger> logger, std::weak_ptr<terrama2::services::interpolator::core::DataManager> weakDataManager)
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

//    auto lock = dataManager->getLock();
//    auto interpolatorPtr = dataManager->findInterpolator(executionPackage.processId);

//    // input data
//    auto inputDataSeries = dataManager->findDataSeries(interpolatorPtr->interpolationParams_->series_);
//    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

//    TERRAMA2_LOG_DEBUG() << tr("Starting interpolation for data series '%1'").arg(inputDataSeries->name.c_str());

//    //    // output data
//    //    auto outputDataSeries = dataManager->findDataSeries(collectorPtr->outputDataSeries);
//    //    auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);

//        // dataManager no longer in use
//        lock.unlock();

//        /////////////////////////////////////////////////////////////////////////
//        //  recovering data

//        auto processingStartTime = terrama2::core::TimeUtils::nowUTC();

//        terrama2::core::Filter* filter = interpolatorPtr->interpolationParams_->filter_.get();

//        if(filter->discardAfter.get() && filter->discardBefore.get()
//            && (*filter->discardAfter) < (*filter->discardBefore))
//        {
//          QString errMsg = QObject::tr("Empty filter time range.");

//          TERRAMA2_LOG_WARNING() << errMsg.toStdString();
//          throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
//        }

//        //update filter based on last collected data timestamp
//        updateFilterDiscardDates(*filter, logger, executionPackage.processId);

    //    auto remover = std::make_shared<terrama2::core::FileRemover>();
    //    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);

    //    auto uriMap = dataAccessor->getFiles(filter, remover);
    //    auto dataMap = dataAccessor->getSeries(uriMap, filter, remover);
    //    if(dataMap.empty())
    //    {
    //      QString errMsg = tr("No data to collect.");
    //      logger->result(CollectorLogger::DONE, nullptr, executionPackage.registerId);
    //      logger->log(CollectorLogger::WARNING_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    //      TERRAMA2_LOG_WARNING() << errMsg;

    //      notifyWaitQueue(executionPackage.processId);
    //      sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
    //      return;
    //    }
    //    auto lastDateTime = dataAccessor->lastDateTime();

    //    /////////////////////////////////////////////////////////////////////////
    //    // data intersection

    //    for(auto& item : dataMap)
    //    {
    //      // intersection
    //      if(collectorPtr->intersection)
    //      {
    //        //FIXME: the datamanager is being used outside the lock
    //        item.second = processIntersection(dataManager, collectorPtr->intersection, item.second);
    //      }
    //    }

    //    /////////////////////////////////////////////////////////////////////////
    //    // storing data

    //    auto inputOutputMap = collectorPtr->inputOutputMap;
    //    auto dataSetLst = outputDataSeries->datasetList;
    //    auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(outputDataSeries, outputDataProvider);

    //    dataStorager->store(dataMap, dataSetLst, inputOutputMap);

    //    TERRAMA2_LOG_INFO() << tr("Data from collector %1 collected successfully.").arg(executionPackage.processId);

    //    auto processingEndTime = terrama2::core::TimeUtils::nowUTC();

    //    logger->setStartProcessingTime(processingStartTime, executionPackage.registerId);
    //    logger->setEndProcessingTime(processingEndTime, executionPackage.registerId);

    //    logger->result(CollectorLogger::DONE, lastDateTime, executionPackage.registerId);

    //    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true);
    //    notifyWaitQueue(executionPackage.processId);
    //    return;
  }
  catch(std::exception&)
  {

  }
  catch(...)
  {

  }
}

void terrama2::services::interpolator::core::Service::addToQueue(InterpolatorId interpolatorId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);

    auto datamanager1 = dataManager_.lock();
    auto interpolator = datamanager1->findInterpolator(interpolatorId);

    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this collector should be executed in this instance
    if(interpolator->serviceInstanceId != serviceInstanceId)
      return;

    RegisterId registerId = logger_->start(interpolatorId);

    terrama2::core::ExecutionPackage executionPackage;
    executionPackage.processId = interpolatorId;
    executionPackage.executionDate = startTime;
    executionPackage.registerId = registerId;

    // if this collector id is already being processed put it on the wait queue
    auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), interpolatorId);
    if(pqIt == processingQueue_.end())
    {
      processQueue_.push_back(executionPackage);
      processingQueue_.push_back(interpolatorId);

      //wake loop thread
      mainLoopCondition_.notify_one();
    }
    else
    {
      waitQueue_[interpolatorId].push(executionPackage);
      logger_->result(InterpolatorLogger::ON_QUEUE, nullptr, executionPackage.registerId);
      TERRAMA2_LOG_INFO() << tr("Interpolator %1 added to wait queue.").arg(interpolatorId);
    }

    mainLoopCondition_.notify_one();
  }
  catch(const terrama2::core::LogException&)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access log database.");
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception durring collector edd to queue...");
  }
}

//void terrama2::services::interpolator::core::Service::collect(terrama2::core::ExecutionPackage executionPackage,
//                                                           std::shared_ptr<CollectorLogger> logger,
//                                                           std::weak_ptr<DataManager> weakDataManager)
//{
//  auto dataManager = weakDataManager.lock();
//  if(!dataManager.get())
//  {
//    TERRAMA2_LOG_ERROR() << tr("Unable to access DataManager");
//    notifyWaitQueue(executionPackage.processId);
//    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
//    return;
//  }

//  try
//  {
//    //////////////////////////////////////////////////////////
//    //  aquiring metadata
//    auto lock = dataManager->getLock();

//    auto collectorPtr = dataManager->findCollector(executionPackage.processId);

//    // input data
//    auto inputDataSeries = dataManager->findDataSeries(collectorPtr->inputDataSeries);
//    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);


//    TERRAMA2_LOG_DEBUG() << tr("Starting collection for data series '%1'").arg(inputDataSeries->name.c_str());

//    // output data
//    auto outputDataSeries = dataManager->findDataSeries(collectorPtr->outputDataSeries);
//    auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);

//    // dataManager no longer in use
//    lock.unlock();

//    /////////////////////////////////////////////////////////////////////////
//    //  recovering data

//    auto processingStartTime = terrama2::core::TimeUtils::nowUTC();

//    terrama2::core::Filter filter = collectorPtr->filter;
//    if(filter.discardAfter.get() && filter.discardBefore.get()
//        && (*filter.discardAfter) < (*filter.discardBefore))
//    {
//      QString errMsg = QObject::tr("Empty filter time range.");

//      TERRAMA2_LOG_WARNING() << errMsg.toStdString();
//      throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
//    }

//    //update filter based on last collected data timestamp
//    updateFilterDiscardDates(filter, logger, executionPackage.processId);

//    auto remover = std::make_shared<terrama2::core::FileRemover>();
//    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);

//    auto uriMap = dataAccessor->getFiles(filter, remover);
//    auto dataMap = dataAccessor->getSeries(uriMap, filter, remover);
//    if(dataMap.empty())
//    {
//      QString errMsg = tr("No data to collect.");
//      logger->result(CollectorLogger::DONE, nullptr, executionPackage.registerId);
//      logger->log(CollectorLogger::WARNING_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
//      TERRAMA2_LOG_WARNING() << errMsg;

//      notifyWaitQueue(executionPackage.processId);
//      sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
//      return;
//    }
//    auto lastDateTime = dataAccessor->lastDateTime();

//    /////////////////////////////////////////////////////////////////////////
//    // data intersection

//    for(auto& item : dataMap)
//    {
//      // intersection
//      if(collectorPtr->intersection)
//      {
//        //FIXME: the datamanager is being used outside the lock
//        item.second = processIntersection(dataManager, collectorPtr->intersection, item.second);
//      }
//    }

//    /////////////////////////////////////////////////////////////////////////
//    // storing data

//    auto inputOutputMap = collectorPtr->inputOutputMap;
//    auto dataSetLst = outputDataSeries->datasetList;
//    auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(outputDataSeries, outputDataProvider);

//    dataStorager->store(dataMap, dataSetLst, inputOutputMap);

//    TERRAMA2_LOG_INFO() << tr("Data from collector %1 collected successfully.").arg(executionPackage.processId);

//    auto processingEndTime = terrama2::core::TimeUtils::nowUTC();

//    logger->setStartProcessingTime(processingStartTime, executionPackage.registerId);
//    logger->setEndProcessingTime(processingEndTime, executionPackage.registerId);

//    logger->result(CollectorLogger::DONE, lastDateTime, executionPackage.registerId);

//    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true);
//    notifyWaitQueue(executionPackage.processId);
//    return;

//  }
//  catch(const terrama2::core::LogException& e)
//  {
//    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
//    if(executionPackage.registerId != 0 )
//    {
//      TERRAMA2_LOG_ERROR() << errMsg << std::endl;
//      TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);
//    }
//  }
//  catch(const terrama2::core::NoDataException& e)
//  {
//    TERRAMA2_LOG_INFO() << tr("Collection finished but there was no data available for collector %1.").arg(executionPackage.processId);

//    if(executionPackage.registerId != 0)
//    {
//      logger->log(CollectorLogger::WARNING_MESSAGE, tr("No data available").toStdString(), executionPackage.registerId);
//      logger->result(CollectorLogger::DONE, nullptr, executionPackage.registerId);
//    }

//    QJsonObject jsonAnswer;
//    jsonAnswer.insert(terrama2::core::ReturnTags::AUTOMATIC, false);
//    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true, jsonAnswer);
//    notifyWaitQueue(executionPackage.processId);
//    return;
//  }
//  catch(const terrama2::Exception& e)
//  {
//    QString errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
//    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

//    if(executionPackage.registerId != 0)
//    {
//      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
//      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
//    }
//  }
//  catch(const boost::exception& e)
//  {
//    std::string errMsg = boost::diagnostic_information(e);
//    TERRAMA2_LOG_ERROR() << errMsg;
//    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

//    if(executionPackage.registerId != 0)
//    {
//      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg, executionPackage.registerId);
//      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
//    }
//  }
//  catch(const std::exception& e)
//  {
//    TERRAMA2_LOG_ERROR() << e.what();
//    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

//    if(executionPackage.registerId != 0)
//    {
//      logger->log(CollectorLogger::ERROR_MESSAGE, e.what(), executionPackage.registerId);
//      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
//    }
//  }
//  catch(...)
//  {
//    QString errMsg = tr("Unknown error.");
//    TERRAMA2_LOG_ERROR() << errMsg;
//    TERRAMA2_LOG_INFO() << tr("Collection for collector %1 finished with error(s).").arg(executionPackage.processId);

//    if(executionPackage.registerId != 0)
//    {
//      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
//      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
//    }

//  }

//  sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
//  notifyWaitQueue(executionPackage.processId);
//}

void terrama2::services::interpolator::core::Service::connectDataManager()
{
  auto dataManager1 = dataManager_.lock();

  connect(dataManager1.get(), &terrama2::services::interpolator::core::DataManager::interpolatorAdded, this,
          &terrama2::services::interpolator::core::Service::addInterpolator);
  connect(dataManager1.get(), &terrama2::services::interpolator::core::DataManager::interpolatorRemoved, this,
          &terrama2::services::interpolator::core::Service::removeInterpolator);
  connect(dataManager1.get(), &terrama2::services::interpolator::core::DataManager::interpolatorUpdated, this,
          &terrama2::services::interpolator::core::Service::updateInterpolator);
}

void terrama2::services::interpolator::core::Service::addInterpolator(const terrama2::services::interpolator::core::InterpolatorParamsPtr& params)
{
  InterpolatorPtr i(InterpolatorFactories::make(params->interpolationType_, *params.get()));

  addProcessToSchedule(i);
}

void terrama2::services::interpolator::core::Service::removeInterpolator(InterpolatorId interpolatorId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);


    TERRAMA2_LOG_INFO() << tr("Removing interpolator %1.").arg(interpolatorId);

    auto it = timers_.find(interpolatorId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(interpolatorId);
      timer->disconnect();
      timers_.erase(interpolatorId);
    }

    // remove from queue
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [interpolatorId](const terrama2::core::ExecutionPackage& executionPackage)
    { return interpolatorId == executionPackage.processId; }), processQueue_.end());

    waitQueue_.erase(interpolatorId);


    TERRAMA2_LOG_INFO() << tr("Interpolator %1 removed successfully.").arg(interpolatorId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove interpolator: %1.").arg(interpolatorId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove interpolator: %1.").arg(interpolatorId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove interpolator: %1.").arg(interpolatorId);
  }
}

void terrama2::services::interpolator::core::Service::updateInterpolator(InterpolatorParamsPtr params) noexcept
{
  removeInterpolator(params->id_);

  addInterpolator(params);
}

void terrama2::services::interpolator::core::Service::updateAdditionalInfo(const QJsonObject& obj) noexcept
{

}
