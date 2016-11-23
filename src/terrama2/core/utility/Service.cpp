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
#include "../data-model/Process.hpp"

// QT
#include <QJsonObject>
#include <QCoreApplication>

terrama2::core::Service::Service()
  : stop_(false)
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
    threadNumber = verifyNumberOfThreads(threadNumber);

    //Starts collection threads
    for(uint i = 0; i < threadNumber; ++i)
      processingThreadPool_.push_back(std::async(std::launch::async, &Service::processingTaskThread, this));
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

size_t terrama2::core::Service::verifyNumberOfThreads(size_t numberOfThreads) const
{
  if(numberOfThreads == 0)
    numberOfThreads = std::thread::hardware_concurrency(); //looks for how many threads the hardware support
  if(numberOfThreads == 0)
    numberOfThreads = 1; //if not able to find out set to 1

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
    catch(std::exception& e)
    {
      TERRAMA2_LOG_ERROR() << e.what();
    }
  }
}

void terrama2::core::Service::processingTaskThread() noexcept
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
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::core::Service::updateNumberOfThreads(size_t numberOfThreads) noexcept
{
  //service not runnig, start service with numberOfThreads threads
  if(!mainLoopThread_.valid())
  {
    start(numberOfThreads);
    return;
  }

  std::unique_lock<std::mutex> lock(mutex_);
  TERRAMA2_LOG_DEBUG() << tr("Old number of threads: %1").arg(processingThreadPool_.size());

  numberOfThreads = verifyNumberOfThreads(numberOfThreads);
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
    processingThreadPool_.push_back(std::async(std::launch::async, &Service::processingTaskThread, this));

  TERRAMA2_LOG_DEBUG() << tr("Actual number of threads: %1").arg(processingThreadPool_.size());
}

terrama2::core::TimerPtr terrama2::core::Service::createTimer(const Schedule& schedule, ProcessId processId, std::shared_ptr<te::dt::TimeInstantTZ> lastProcess) const
{
  terrama2::core::TimerPtr timer = std::make_shared<const terrama2::core::Timer>(schedule, processId, lastProcess);
  connect(timer.get(), &terrama2::core::Timer::timeoutSignal, this, &terrama2::core::Service::addToQueue, Qt::UniqueConnection);

  return timer;
}

void terrama2::core::Service::sendProcessFinishedSignal(const ProcessId processId, const bool success, QJsonObject jsonAnswer)
{
  jsonAnswer.insert("process_id", static_cast<int>(processId));
  jsonAnswer.insert("result", success);

  emit processFinishedSignal(jsonAnswer);
}

void terrama2::core::Service::addProcessToSchedule(ProcessPtr process) noexcept
{
  try
  {
    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this collector should be executed in this instance
    if(process->serviceInstanceId != serviceInstanceId)
      return;

    try
    {
      if(process->active && process->schedule.id != 0)
      {
        std::lock_guard<std::mutex> lock(mutex_);

        std::shared_ptr<te::dt::TimeInstantTZ> lastProcess = logger_->getLastProcessTimestamp(process->id);
        terrama2::core::TimerPtr timer = createTimer(process->schedule, process->id, lastProcess);
        timers_.emplace(process->id, timer);
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
