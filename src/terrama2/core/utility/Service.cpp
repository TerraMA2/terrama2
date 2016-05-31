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
#include "Logger.hpp"

terrama2::core::Service::Service()
  : stop_(false)
{

}

terrama2::core::Service::~Service()
{
  stop();
}

void terrama2::core::Service::start(uint threadNumber)
{

  // if service already running, throws
  if(mainLoopThread_.valid())
  {
    QString errMsg = tr("Service alredy running.");
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
}

int terrama2::core::Service::verifyNumberOfThreads(int numberOfThreads) const
{
  if(numberOfThreads)
    numberOfThreads = std::thread::hardware_concurrency(); //looks for how many threads the hardware support
  if(!numberOfThreads)
    numberOfThreads = 1; //if not able to find out set to 1

  return numberOfThreads;
}

void terrama2::core::Service::stop() noexcept
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
  if(mainLoopThread_.valid())
    mainLoopThread_.get();

  //wait for each collecting thread
  for (auto& future : processingThreadPool_)
  {
    if(future.valid())
      future.get();
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
