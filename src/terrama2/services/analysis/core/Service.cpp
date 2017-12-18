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
#include "python/PythonInterpreter.hpp"
#include "MonitoredObjectContext.hpp"
#include "ContextManager.hpp"
#include "../../../core/utility/ServiceManager.hpp"
#include "../../../core/utility/StoragerManager.hpp"
#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"
#include "utility/JSonUtils.hpp"

#include <ThreadPool.h>

terrama2::services::analysis::core::Service::Service(DataManagerPtr dataManager)
: terrama2::core::Service(),
  dataManager_(dataManager)
{
  connectDataManager();
  mainThreadState_ = PyThreadState_Get();

  storagerManager_ = std::make_shared<terrama2::core::StoragerManager>(dataManager_);

  connect(&analysisExecutor_, &AnalysisExecutor::analysisFinished, this, &Service::analysisFinished);
}

terrama2::services::analysis::core::Service::~Service()
{
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
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [analysisId](const terrama2::core::ExecutionPackage& executionPackage)
                                       { return analysisId == executionPackage.processId; }), processQueue_.end());

    waitQueue_.erase(analysisId);

    TERRAMA2_LOG_INFO() << tr("Analysis %1 removed successfully.").arg(analysisId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove analysis: %1.").arg(analysisId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove analysis: %1.").arg(analysisId);
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
    TERRAMA2_LOG_INFO() << tr("Could not remove analysis: %1.").arg(analysisId);
  }
}

void terrama2::services::analysis::core::Service::updateAnalysis(AnalysisPtr analysis) noexcept
{
  removeAnalysis(analysis->id);
  addProcessToSchedule(analysis);
}

void terrama2::services::analysis::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    auto analysisPtr = dataManager_->findAnalysis(executionPackage.processId);
    taskQueue_.emplace(std::bind(&terrama2::services::analysis::core::AnalysisExecutor::runAnalysis, std::ref(analysisExecutor_), dataManager_, storagerManager_, std::dynamic_pointer_cast<AnalysisLogger>(logger_->clone()), executionPackage, analysisPtr, threadPool_, mainThreadState_));
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}


void terrama2::services::analysis::core::Service::addToQueue(AnalysisId analysisId, std::shared_ptr<te::dt::TimeInstantTZ> executionDate) noexcept
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


    RegisterId registerId = logger_->start(analysis->id);

    if(analysis->reprocessingHistoricalData)
    {
      auto reprocessingHistoricalData = analysis->reprocessingHistoricalData;

      auto executionDate = reprocessingHistoricalData->startDate;
      boost::local_time::local_date_time endDate = terrama2::core::TimeUtils::nowBoostLocal();

      if(analysis->reprocessingHistoricalData->endDate)
        endDate = analysis->reprocessingHistoricalData->endDate->getTimeInstantTZ();

      boost::local_time::local_date_time titz = executionDate->getTimeInstantTZ();

      double frequencySeconds = terrama2::core::TimeUtils::frequencySeconds(analysis->schedule);
      double scheduleSeconds = terrama2::core::TimeUtils::scheduleSeconds(analysis->schedule, executionDate);
      if(frequencySeconds < 0 || scheduleSeconds < 0)
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Invalid schedule");
        return;
      }

      while(titz <= endDate)
      {

        terrama2::core::ExecutionPackage executionPackage;
        executionPackage.processId = analysisId;
        executionPackage.executionDate = executionDate;
        executionPackage.registerId = registerId;

        erasePreviousResult(dataManager_, analysis->outputDataSeriesId, executionDate);
        auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), analysisId);
        if(pqIt == processingQueue_.end())
        {
          processQueue_.push_back(executionPackage);
          processingQueue_.push_back(analysisId);

          //wake loop thread
          mainLoopCondition_.notify_one();
        }
        else
        {
          waitQueue_[analysisId].push(executionPackage);
        }


        if(frequencySeconds > 0.)
        {
          titz += boost::posix_time::seconds(frequencySeconds);
        }
        else if(scheduleSeconds > 0.)
        {
          titz += boost::posix_time::seconds(scheduleSeconds);
        }
        executionDate.reset(new te::dt::TimeInstantTZ(titz));
      }
    }
    else
    {

      terrama2::core::ExecutionPackage executionPackage;
      executionPackage.processId = analysisId;
      executionPackage.executionDate = executionDate;
      executionPackage.registerId = registerId;

      auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), analysisId);
      if(pqIt == processingQueue_.end())
      {
        processQueue_.push_back(executionPackage);
        processingQueue_.push_back(analysisId);

        //wake loop thread
        mainLoopCondition_.notify_one();
      }
      else
      {
        waitQueue_[analysisId].push(executionPackage);
        logger_->result(AnalysisLogger::Status::ON_QUEUE, nullptr, executionPackage.registerId);
        TERRAMA2_LOG_INFO() << tr("Analysis %1 added to wait queue.").arg(analysisId);
      }
    }

  }
  catch(const terrama2::Exception& e)
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

void terrama2::services::analysis::core::Service::connectDataManager()
{
  connect(dataManager_.get(), &DataManager::analysisAdded, this, &Service::addProcessToSchedule);
  connect(dataManager_.get(), &DataManager::analysisRemoved, this, &Service::removeAnalysis);
  connect(dataManager_.get(), &DataManager::analysisUpdated, this, &Service::updateAnalysis);
  connect(dataManager_.get(), &DataManager::validateAnalysis, this, &Service::validateAnalysis);
}

void terrama2::services::analysis::core::Service::start(size_t threadNumber)
{
  terrama2::core::Service::start(threadNumber);
  threadPool_.reset(new ThreadPool(processingThreadPool_.size()));
}

void terrama2::services::analysis::core::Service::analysisFinished(AnalysisId analysisId, std::shared_ptr< te::dt::TimeInstantTZ > executionDate, bool success,  QJsonObject jsonAnswer)
{
  auto analysis = dataManager_->findAnalysis(analysisId);

  // Remove from processing queue
  auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), analysisId);
  if(pqIt != processingQueue_.end())
    processingQueue_.erase(pqIt);

  sendProcessFinishedSignal(analysisId, executionDate, success, jsonAnswer);

  // Verify if there is another execution for the same analysis waiting
  auto& packageQueue = waitQueue_[analysisId];

  if(!packageQueue.empty())
  {
    // erase the previous execution result and run another time
    auto executionPackage = packageQueue.front();
    packageQueue.pop();

    processQueue_.push_back(executionPackage);
    if(packageQueue.empty())
      waitQueue_.erase(analysisId);
    //erase previous result from the analysis NEXT date
    erasePreviousResult(dataManager_, analysis->outputDataSeriesId, executionPackage.executionDate);

    //wake loop thread
    mainLoopCondition_.notify_one();
  }
}

void terrama2::services::analysis::core::Service::updateAdditionalInfo(const QJsonObject& /*obj*/) noexcept
{

}

void terrama2::services::analysis::core::Service::validateAnalysis(AnalysisPtr analysis) noexcept
{
  try
  {
    ValidateResult result = analysisExecutor_.validateAnalysis(dataManager_, analysis);
    QJsonObject obj = toJson(result);

    emit validateProcessSignal(obj);

  }
  catch(const terrama2::Exception& e)
  {
    //logged on throw
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }

}
