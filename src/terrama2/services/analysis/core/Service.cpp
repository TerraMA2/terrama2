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
#include "../../../core/utility/Logger.hpp"

terrama2::services::analysis::core::Service::Service(DataManagerPtr dataManager)
: terrama2::core::Service(),
  dataManager_(dataManager)
{
}

terrama2::services::analysis::core::Service::~Service()
{
}



bool terrama2::services::analysis::core::Service::mainLoopWaitCondition() noexcept
{
  return !analysisQueue_.empty() || stop_;
}

bool terrama2::services::analysis::core::Service::checkNextData()
{
  //check if there is data to collect
  if(analysisQueue_.empty())
    return false;

  uint64_t analysisId = analysisQueue_.front();
  //prepare task for collecting
  prepareTask(analysisId);

  //remove from queue
  analysisQueue_.pop();

  //is there more data to process?
  return !analysisQueue_.empty();
}


void terrama2::services::analysis::core::Service::addAnalysis(uint64_t analysisId)
{

  // add to queue to run now
    addToQueue(analysisId);

}


void terrama2::services::analysis::core::Service::prepareTask(uint64_t analysisId)
{
  try
  {
    Analysis analysis = dataManager_->findAnalysis(analysisId);
    taskQueue_.emplace(std::bind(&run, analysis));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}


void terrama2::services::analysis::core::Service::addToQueue(uint64_t analysisId)
{
  try
  {
    //Lock Thread and add to the queue
    std::lock_guard<std::mutex> lock(mutex_);

    analysisQueue_.push(analysisId);

    //wake loop thread
    mainLoopCondition_.notify_one();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::analysis::core::Service::run(Analysis analysis)
{
  terrama2::services::analysis::core::runAnalysis(analysis);
}
