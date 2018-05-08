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
  \file terrama2/core/Service.cpp

  \brief Core service for TerraMA² process.

  \author Jano Simas
*/

#include "Service.hpp"
#include "ServiceManager.hpp"
#include "Logger.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"
#include "../data-model/Process.hpp"
#include "../data-model/Schedule.hpp"
#include "../data-model/DataManager.hpp"
#include "../data-model/Project.hpp"

// QT
#include <QJsonObject>
#include <QCoreApplication>

terrama2::core::Service::Service(std::weak_ptr<DataManager> dataManager)
  : dataManager_(dataManager),
    stop_(false)
{
  auto& serviceManager = terrama2::core::ServiceManager::getInstance();

  QObject::connect(&serviceManager, &terrama2::core::ServiceManager::additionalInfoUpdated, this, &terrama2::core::Service::updateAdditionalInfo);
}

terrama2::core::Service::~Service()
{
  stopService();
}

void terrama2::core::Service::start(size_t threadNumber)
{

  // if service already running, throws
  if(mainLoopThread_.valid())
  {
    QString errMsg = tr("Service already running.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ServiceException() << ErrorDescription(errMsg);
  }

  try
  {
    stop_ = false;
    //start the loop thread
    mainLoopThread_ = std::async(std::launch::async, &Service::mainLoopThread, this);

    //check for the number o threads to create
    threadNumber = verifyNumberOfThreads(threadNumber, 4);

    //Starts collection threads
    for(uint i = 0; i < threadNumber; ++i)
      processingThreadPool_.push_back(std::async(std::launch::async, &Service::processingTaskThread, this, logger_));
  }
  catch(const std::exception& e)
  {
    QString errMsg(tr("Unable to start service: %1."));
    errMsg = errMsg.arg(e.what());

    TERRAMA2_LOG_ERROR() << errMsg;
    throw ServiceException() << ErrorDescription(errMsg);
  }
 
  TERRAMA2_LOG_DEBUG() << tr("Actual number of threads: %1").arg(processingThreadPool_.size());
}

size_t terrama2::core::Service::verifyNumberOfThreads(size_t numberOfThreads, size_t maxNumberOfThreads) const
{
  if(numberOfThreads == 0)
    numberOfThreads = std::thread::hardware_concurrency(); //looks for how many threads the hardware support
  if(numberOfThreads == 0)
    numberOfThreads = 1; //if not able to find out set to 1
  if(maxNumberOfThreads > 0 && numberOfThreads > maxNumberOfThreads)
    numberOfThreads = maxNumberOfThreads;

  return numberOfThreads;
}

void terrama2::core::Service::stopService() noexcept
{
  try
  {
    stop(false);
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::core::Service::stop(bool holdStopSignal) noexcept
{
  try
  {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      // finish the thread
      stop_ = true;

      //wake loop thread and collecting threads
      mainLoopCondition_.notify_one();
      processingThreadCondition_.notify_all();
    }

    //wait for the loop thread
    while(mainLoopThread_.valid())
    {
      auto status = mainLoopThread_.wait_for(std::chrono::seconds(1));
      if(status == std::future_status::ready)
      {
        mainLoopThread_.get();
      }
      else
      {
        QCoreApplication::processEvents();
      }
    }

    //wait for each collecting thread
    for(auto & thread : processingThreadPool_)
    {
      auto&& future = std::move(thread);
      while(future.valid())
      {
        auto status = future.wait_for(std::chrono::seconds(1));
        if(status == std::future_status::ready)
          future.get();
        else
          QCoreApplication::processEvents();
      }
    }

    if(!holdStopSignal)
      emit serviceFinishedSignal();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::core::Service::mainLoopThread() noexcept
{

  while(true)
  {
    try
    {
      {
        std::unique_lock<std::mutex> lock(mutex_);
        //wait for new data to collect
        mainLoopCondition_.wait(lock, [this] { return stop_ || hasDataOnQueue(); });

        if(stop_)
          break;

        while(processNextData());

        if(stop_)
          break;
      }

      //wake collecting threads
      processingThreadCondition_.notify_all();

      if(stop_)
        break;
    }
    catch(const std::exception& e)
    {
      TERRAMA2_LOG_ERROR() << e.what();
    }
  }
}

void terrama2::core::Service::processingTaskThread(const std::shared_ptr< const ProcessLogger > logger) noexcept
{
  try
  {
    while(true)
    {
      std::packaged_task<void()> task;

      {
        std::unique_lock<std::mutex> lock(mutex_);
        //wait for new data to be collected
        processingThreadCondition_.wait(lock, [this] { return !taskQueue_.empty() || stop_; });

        if(stop_)
          break;

        if(!logger->isValid())
        {
          TERRAMA2_LOG_DEBUG() << tr("Error in logger state.\nProcess not executed.");
          continue;
        }

        if(!taskQueue_.empty())
        {
          task = std::move(taskQueue_.front());
          taskQueue_.pop();
        }
      }

      if(stop_)
        break;

      if(task.valid())
        task();

      if(stop_)
        break;
    }
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::core::Service::updateNumberOfThreads(size_t numberOfThreads) noexcept
{
  numberOfThreads = verifyNumberOfThreads(numberOfThreads);

  //service not runnig, start service with numberOfThreads threads
  if(!mainLoopThread_.valid())
  {
    start(numberOfThreads);
    return;
  }

  std::unique_lock<std::mutex> lock(mutex_);
  TERRAMA2_LOG_DEBUG() << tr("Old number of threads: %1").arg(processingThreadPool_.size());

  //same number of threads, nothing to do
  if(numberOfThreads == processingThreadPool_.size())
    return;

  //stop all threads and start again.
  if(numberOfThreads < processingThreadPool_.size())
  {
    // joining individual threads is not possible because of the
    // waiting condition and the stop condition that are global
    stop(true);
    start(numberOfThreads);
  }

  //create threads until there are numberOfThreads threads
  while(numberOfThreads > processingThreadPool_.size())
    processingThreadPool_.push_back(std::async(std::launch::async, &Service::processingTaskThread, this, logger_));

  TERRAMA2_LOG_DEBUG() << tr("Actual number of threads: %1").arg(processingThreadPool_.size());
}

terrama2::core::TimerPtr terrama2::core::Service::createTimer(ProcessPtr process, std::shared_ptr<te::dt::TimeInstantTZ> lastProcess) const
{
  terrama2::core::TimerPtr timer = std::make_shared<const terrama2::core::Timer>(process, lastProcess);
  connect(timer.get(), &terrama2::core::Timer::timeoutSignal, this, &terrama2::core::Service::addToQueue, Qt::UniqueConnection);

  return timer;
}

void terrama2::core::Service::sendProcessFinishedSignal(const ProcessId processId, std::shared_ptr<te::dt::TimeInstantTZ> executionDate, const bool success, QJsonObject jsonAnswer)
{
  jsonAnswer.insert(ReturnTags::PROCESS_ID, static_cast<int>(processId));
  jsonAnswer.insert(ReturnTags::RESULT, success);
  jsonAnswer.insert(ReturnTags::EXECUTION_DATE, QString::fromStdString(TimeUtils::getISOString(executionDate)));

  emit processFinishedSignal(jsonAnswer);
}

void terrama2::core::Service::addReprocessingToQueue(ProcessPtr process) noexcept
{
  try
  {
    auto processId = process->id;
    auto schedule = process->schedule;
    auto reprocessingHistoricalData = schedule.reprocessingHistoricalData;

    auto executionDate = reprocessingHistoricalData->startDate;
    auto endDate = reprocessingHistoricalData->endDate->getTimeInstantTZ();
    boost::local_time::local_date_time titz = executionDate->getTimeInstantTZ();

    RegisterId registerId = logger_->start(processId);

    double frequencySeconds = terrama2::core::TimeUtils::frequencySeconds(schedule);
    double scheduleSeconds = terrama2::core::TimeUtils::scheduleSeconds(schedule, executionDate);
    if((frequencySeconds < 0 || scheduleSeconds < 0)
        || (frequencySeconds > 0 && scheduleSeconds > 0) )
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Invalid schedule");
      return;
    }

    while(titz <= endDate)
    {
      terrama2::core::ExecutionPackage executionPackage;
      executionPackage.processId = processId;
      executionPackage.executionDate = executionDate;
      executionPackage.registerId = registerId;

      erasePreviousResult(process, executionDate);

      waitQueue_[processId].push(executionPackage);

      titz += boost::posix_time::seconds(static_cast<long>(frequencySeconds));
      titz += boost::posix_time::seconds(static_cast<long>(scheduleSeconds));

      executionDate.reset(new te::dt::TimeInstantTZ(titz));
    }

    auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), processId);
    if(pqIt == processingQueue_.end())
    {
      notifyWaitQueue(processId);
    }
  }
  catch(const terrama2::core::FunctionNotImplementedException&)
  {
    //error already logged
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error in reprocessing historical data enqueue";
  }
}

void terrama2::core::Service::erasePreviousResult(ProcessPtr /*process*/, std::shared_ptr<te::dt::TimeInstantTZ> /*timestamp*/) const
{
  // reimplement as needed in other services
}

void terrama2::core::Service::addProcessToSchedule(ProcessPtr process) noexcept
{
  try
  {
    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    auto dataManager = dataManager_.lock();
    auto projectPtr = dataManager->findProject(process->projectId);
    if(!projectPtr)
    {
      QString errMsg = tr("Invalid Project for process: "+process->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ServiceException() << ErrorDescription(errMsg);
    }

    // Check if this collector should be executed in this instance
    if(process->serviceInstanceId != serviceInstanceId)
      return;

    try
    {
      if(process->active && projectPtr->active && process->schedule.id != 0)
      {
        std::lock_guard<std::mutex> lock(mutex_);

        // reprocessing hitorical data can only be started manually
        // referer to startProcess()
        if(!process->schedule.reprocessingHistoricalData)
        {
          std::shared_ptr<te::dt::TimeInstantTZ> lastProcess = logger_->getLastProcessTimestamp(process->id);
          terrama2::core::TimerPtr timer = createTimer(process, lastProcess);
          timers_.emplace(process->id, timer);
        }
      }
    }
    catch(const terrama2::core::LogException& e)
    {
      TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    }
    catch(const terrama2::core::InvalidFrequencyException&)
    {
      // invalid schedule, already logged
    }
    catch(const te::common::Exception& e)
    {
      TERRAMA2_LOG_ERROR() << e.what();
    }
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::core::Service::Service::setLogger(std::shared_ptr<ProcessLogger> logger) noexcept
{
  logger_ = logger;
}

bool terrama2::core::Service::hasDataOnQueue() noexcept
{
  return !processQueue_.empty();
}


bool terrama2::core::Service::processNextData()
{
  //check if there is data to collect
  if(processQueue_.empty())
    return false;

  auto executionPackage = processQueue_.front();

  //prepare task for collecting
  prepareTask(executionPackage);

  //remove from queue
  processQueue_.erase(processQueue_.begin());

  //is there more data to process?
  return !processQueue_.empty();
}

void terrama2::core::Service::notifyWaitQueue(ProcessId processId)
{
  // Remove from processing queue
  auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), processId);
  if(pqIt != processingQueue_.end())
    processingQueue_.erase(pqIt);

  // Verify if the there is an process waiting for the same collector id
  if(!waitQueue_[processId].empty())
  {
    auto executionPackage = waitQueue_[processId].front();
    waitQueue_[processId].pop();

    // Adds to the processing queue
    processingQueue_.push_back(processId);
    processQueue_.push_back(executionPackage);

    //wake loop thread
    mainLoopCondition_.notify_one();
  }
}

void terrama2::core::Service::updateFilterDiscardDates(terrama2::core::Filter& filter, std::shared_ptr<ProcessLogger> logger, ProcessId processId) const
{
  std::shared_ptr<te::dt::TimeInstantTZ> lastCollectedDataTimestamp = logger->getDataLastTimestamp(processId);
  if(lastCollectedDataTimestamp && filter.discardBefore)
  {
    if(*filter.discardBefore < *lastCollectedDataTimestamp)
      filter.discardBefore = lastCollectedDataTimestamp;
  }
  else if(lastCollectedDataTimestamp)
    filter.discardBefore = lastCollectedDataTimestamp;
}

void terrama2::core::Service::addToQueue(ProcessPtr process, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  try
  {
    //Lock Thread and add to the queue
    std::lock_guard<std::mutex> lock(mutex_);
    if(process->serviceInstanceId != terrama2::core::ServiceManager::getInstance().instanceId())
      return;

    auto processId = process->id;
    RegisterId registerId = logger_->start(processId);

    terrama2::core::ExecutionPackage executionPackage;
    executionPackage.processId = processId;
    executionPackage.executionDate = startTime;
    executionPackage.registerId = registerId;

    auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), processId);
    if(pqIt == processingQueue_.end())
    {
      processQueue_.push_back(executionPackage);
      processingQueue_.push_back(processId);

      //wake loop thread
      mainLoopCondition_.notify_one();
    }
    else
    {
      waitQueue_[processId].push(executionPackage);
      logger_->result(ProcessLogger::Status::ON_QUEUE, nullptr, executionPackage.registerId);
      TERRAMA2_LOG_INFO() << tr("Process %1 added to wait queue.").arg(processId);
    }
  }
  catch(const terrama2::Exception&)
  {
    //logged on throw
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::core::Service::startProcess(ProcessId processId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  auto process = getProcess(processId);

  if(process->schedule.reprocessingHistoricalData)
    addReprocessingToQueue(process);
  else
    addToQueue(process, startTime);
}
