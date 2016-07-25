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
  \file terrama2/services/analysis/core/Service.cpp

  \brief Service class to the analysis module.

  \author Paulo R. M. Oliveira
*/

#include "Service.hpp"
#include "Exception.hpp"
#include "DataManager.hpp"
#include "AnalysisExecutor.hpp"
#include "PythonInterpreter.hpp"
#include "MonitoredObjectContext.hpp"
#include "../../../core/utility/ServiceManager.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"

terrama2::services::analysis::core::Service::Service(DataManagerPtr dataManager)
: terrama2::core::Service(),
  dataManager_(dataManager)
{
  connectDataManager();
}

terrama2::services::analysis::core::Service::~Service()
{
}



bool terrama2::services::analysis::core::Service::hasDataOnQueue() noexcept
{
  return !analysisQueue_.empty();
}

bool terrama2::services::analysis::core::Service::processNextData()
{
  //check if there is data to collect
  if(analysisQueue_.empty())
    return false;

  auto analysisPair = analysisQueue_.front();
  //prepare task for collecting
  prepareTask(analysisPair.first, analysisPair.second);

  //remove from queue
  analysisQueue_.erase(analysisQueue_.begin());

  //is there more data to process?
  return !analysisQueue_.empty();
}

void terrama2::services::analysis::core::Service::addAnalysis(AnalysisId analysisId) noexcept
{
  try
  {
    AnalysisPtr analysis = dataManager_->findAnalysis(analysisId);

    if(analysis->serviceInstanceId != terrama2::core::ServiceManager::getInstance().instanceId())
    {
      return;
    }

    if(analysis->active)
    {
      std::lock_guard<std::mutex> lock(mutex_);

      auto lastProcess = logger_->getLastProcessTimestamp(analysis->id);
      terrama2::core::TimerPtr timer = createTimer(analysis->schedule, analysisId, lastProcess);
      timers_.emplace(analysisId, timer);
    }

    // add to queue to run now
    addToQueue(analysisId);
  }
  catch(terrama2::core::InvalidFrequencyException&)
  {
    // invalid schedule, already logged
  }
  catch(terrama2::Exception&)
  {

  }
  catch(const te::common::Exception& e)
  {
    //TODO: should be caught elsewhere?
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }

}

void terrama2::services::analysis::core::Service::setLogger(std::shared_ptr<AnalysisLogger> logger) noexcept
{
  logger_ = logger;
}

void terrama2::services::analysis::core::Service::removeAnalysis(AnalysisId analysisId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);

    TERRAMA2_LOG_INFO() << tr("Removing analysis %1.").arg(analysisId);

    auto it = timers_.find(analysisId);

    if (it != timers_.end())
    {
      it->second->disconnect();
      timers_.erase(analysisId);
    }

    // remove from queue
    auto rit = analysisQueue_.rbegin();
    auto rend = analysisQueue_.rend();
    while(rit != rend)
    {
      if(rit->first == analysisId)
      {
        analysisQueue_.erase(rit.base());
      }

      ++rit;
    }

    TERRAMA2_LOG_INFO() << tr("Analysis %1 removed successfully.").arg(analysisId);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove analysis: %1.").arg(analysisId);
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove analysis: %1.").arg(analysisId);
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
    TERRAMA2_LOG_INFO() << tr("Could not remove analysis: %1.").arg(analysisId);
  }
}

void terrama2::services::analysis::core::Service::updateAnalysis(AnalysisId analysisId) noexcept
{
  //TODO: addAnalysis adds to queue, is this expected?
  addAnalysis(analysisId);
}

void terrama2::services::analysis::core::Service::prepareTask(AnalysisId analysisId, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
{
  try
  {
    auto analysisPtr = dataManager_->findAnalysis(analysisId);
    taskQueue_.emplace(std::bind(&terrama2::services::analysis::core::runAnalysis, dataManager_, logger_, startTime, analysisPtr, threadPool_));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}


void terrama2::services::analysis::core::Service::addToQueue(AnalysisId analysisId) noexcept
{
  try
  {
    //Lock Thread and add to the queue
    std::lock_guard<std::mutex> lock(mutex_);

    auto analysis = dataManager_->findAnalysis(analysisId);

    if(analysis->serviceInstanceId != terrama2::core::ServiceManager::getInstance().instanceId())
    {
      return;
    }

    auto startTime = terrama2::core::TimeUtils::nowUTC();

    analysisQueue_.push_back(std::make_pair(analysisId, startTime));

    //wake loop thread
    mainLoopCondition_.notify_one();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::services::analysis::core::Service::connectDataManager()
{
  connect(dataManager_.get(), &DataManager::analysisAdded, this, &Service::addAnalysis);
  connect(dataManager_.get(), &DataManager::analysisRemoved, this, &Service::removeAnalysis);
  connect(dataManager_.get(), &DataManager::analysisUpdated, this, &Service::updateAnalysis);
}

void terrama2::services::analysis::core::Service::start(size_t threadNumber)
{
  terrama2::core::Service::start(threadNumber);
  threadPool_.reset(new ThreadPool(processingThreadPool_.size()));
}
