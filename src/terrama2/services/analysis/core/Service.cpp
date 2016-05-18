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
#include "Context.hpp"
#include "../../../core/utility/ServiceManager.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Timer.hpp"

terrama2::services::analysis::core::Service::Service(DataManagerPtr dataManager)
: terrama2::core::Service(),
  dataManager_(dataManager)
{
  terrama2::services::analysis::core::Context::getInstance().setDataManager(dataManager);
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

  AnalysisId analysisId = analysisQueue_.front();
  //prepare task for collecting
  prepareTask(analysisId);

  //remove from queue
  analysisQueue_.erase(analysisQueue_.begin());

  //is there more data to process?
  return !analysisQueue_.empty();
}

void terrama2::services::analysis::core::Service::updateNumberOfThreads(int numberOfThreads)
{
  numberOfThreads = verifyNumberOfThreads(numberOfThreads);
  stop();
  start(numberOfThreads);
}

void terrama2::services::analysis::core::Service::addAnalysis(AnalysisId analysisId)
{
  try
  {
    Analysis analysis = dataManager_->findAnalysis(analysisId);

    std::map<std::string, std::string> connInfo = terrama2::core::ServiceManager::getInstance().logConnectionInfo();
    std::shared_ptr< AnalysisLogger > analysisLog(new AnalysisLogger(analysisId, connInfo));
    loggers_.emplace(analysisId, analysisLog);

    auto lastProcess = logger_->getLastProcessTimestamp(analysis.id);
    terrama2::core::TimerPtr timer = std::make_shared<const terrama2::core::Timer>(analysis.schedule, analysisId, lastProcess);
    connect(timer.get(), &terrama2::core::Timer::timeoutSignal, this, &terrama2::services::analysis::core::Service::addToQueue, Qt::UniqueConnection);
    timers_.emplace(analysisId, timer);

    // add to queue to run now
    addToQueue(analysisId);
  }
  catch(terrama2::Exception& e)
  {
    QString errMsg = QObject::tr("Could not add analysis %1 to the queue").arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
  }
}

void terrama2::services::analysis::core::Service::updateLoggerConnectionInfo(const std::map<std::string, std::string>& connInfo)
{
  logger_ = std::make_shared<AnalysisLogger>(connInfo);
}

void terrama2::services::analysis::core::Service::removeAnalysis(AnalysisId analysisId)
{
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = std::find(analysisQueue_.begin(), analysisQueue_.end(), analysisId);
  if(it != analysisQueue_.end())
    analysisQueue_.erase(it);
}

void terrama2::services::analysis::core::Service::updateAnalysis(AnalysisId analysisId)
{
  // the analysis object will only be fetched when the execution proccess begin.
  // we only have the id so there is no need to update.
}

void terrama2::services::analysis::core::Service::prepareTask(AnalysisId analysisId)
{
  try
  {
    Analysis analysis = dataManager_->findAnalysis(analysisId);
    taskQueue_.emplace(std::bind(&terrama2::services::analysis::core::runAnalysis, dataManager_, analysis));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}


void terrama2::services::analysis::core::Service::addToQueue(AnalysisId analysisId)
{
  try
  {
    //Lock Thread and add to the queue
    std::lock_guard<std::mutex> lock(mutex_);

    analysisQueue_.push_back(analysisId);

    //wake loop thread
    mainLoopCondition_.notify_one();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::analysis::core::Service::connectDataManager()
{
  connect(dataManager_.get(), &DataManager::analysisAdded, this, &Service::addAnalysis);
  connect(dataManager_.get(), &DataManager::analysisRemoved, this, &Service::removeAnalysis);
  connect(dataManager_.get(), &DataManager::analysisUpdated, this, &Service::updateAnalysis);
}
