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

terrama2::services::analysis::core::Service::Service(std::weak_ptr<terrama2::core::DataManager> dataManager)
: terrama2::core::Service(dataManager)
{
  connectDataManager();
  mainThreadState_ = PyThreadState_Get();

  auto analysisDataManager = std::static_pointer_cast<terrama2::services::analysis::core::DataManager>(dataManager_.lock());
  storagerManager_ = std::make_shared<terrama2::core::StoragerManager>(analysisDataManager);

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
    auto dataManager = std::static_pointer_cast<terrama2::services::analysis::core::DataManager>(dataManager_.lock());
    auto analysisPtr = dataManager->findAnalysis(executionPackage.processId);

    taskQueue_.emplace(std::bind(&terrama2::services::analysis::core::AnalysisExecutor::runAnalysis, std::ref(analysisExecutor_), dataManager, storagerManager_, std::static_pointer_cast<AnalysisLogger>(logger_->clone()), executionPackage, analysisPtr, threadPool_, mainThreadState_));
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::analysis::core::Service::connectDataManager()
{
  auto dataManager = std::static_pointer_cast<terrama2::services::analysis::core::DataManager>(dataManager_.lock());
  connect(dataManager.get(), &DataManager::analysisAdded, this, &Service::addProcessToSchedule);
  connect(dataManager.get(), &DataManager::analysisRemoved, this, &Service::removeAnalysis);
  connect(dataManager.get(), &DataManager::analysisUpdated, this, &Service::updateAnalysis);
  connect(dataManager.get(), &DataManager::validateAnalysis, this, &Service::validateAnalysis);
}

void terrama2::services::analysis::core::Service::start(size_t threadNumber)
{
  terrama2::core::Service::start(threadNumber);
  threadNumber = std::ceil(verifyNumberOfThreads(threadNumber)/2.);
  if(threadNumber < 1) threadNumber = 1;
  threadPool_.reset(new ThreadPool(threadNumber));
}

void terrama2::services::analysis::core::Service::erasePreviousResult(terrama2::core::ProcessPtr process, std::shared_ptr<te::dt::TimeInstantTZ> timestamp) const
{
  auto analysis = std::static_pointer_cast<const Analysis>(process);

  auto dataManager = dataManager_.lock();
  terrama2::core::erasePreviousResult(dataManager, analysis->outputDataSeriesId, timestamp);
}

void terrama2::services::analysis::core::Service::analysisFinished(AnalysisId analysisId, std::shared_ptr< te::dt::TimeInstantTZ > executionDate, bool success,  QJsonObject jsonAnswer)
{
  auto dataManager = std::static_pointer_cast<terrama2::services::analysis::core::DataManager>(dataManager_.lock());

  // Remove from processing queue
  auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), analysisId);
  if(pqIt != processingQueue_.end())
    processingQueue_.erase(pqIt);

  sendProcessFinishedSignal(analysisId, executionDate, success, jsonAnswer);
  notifyWaitQueue(analysisId);
}

void terrama2::services::analysis::core::Service::updateAdditionalInfo(const QJsonObject& /*obj*/) noexcept
{

}

void terrama2::services::analysis::core::Service::validateAnalysis(AnalysisPtr analysis) noexcept
{
  try
  {
    auto dataManager = std::static_pointer_cast<terrama2::services::analysis::core::DataManager>(dataManager_.lock());
    ValidateResult result = analysisExecutor_.validateAnalysis(dataManager, analysis);
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

terrama2::core::ProcessPtr terrama2::services::analysis::core::Service::getProcess(ProcessId processId)
{
  auto dataManager = std::static_pointer_cast<terrama2::services::analysis::core::DataManager>(dataManager_.lock());
  return dataManager->findAnalysis(processId);
}
