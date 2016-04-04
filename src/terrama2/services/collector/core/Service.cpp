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

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"

#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessFactory.hpp"

terrama2::services::collector::core::Service::Service(std::weak_ptr<terrama2::services::collector::core::DataManager> dataManager)
  : dataManager_(dataManager)
{

}

bool terrama2::services::collector::core::Service::mainLoopWaitCondition() noexcept
{
  return !collectorQueue_.empty() || stop_;
}

bool terrama2::services::collector::core::Service::checkNextData()
{
    //check if there is data to collect
  if(collectorQueue_.empty())
    return false;


  //get first data
  const auto& collectorId = collectorQueue_.front();

  //prepare task for collecting
  prepareTask(collectorId);

  //remove from queue
  collectorQueue_.pop();

  //is there more data to process?
  return !collectorQueue_.empty();
}

void terrama2::services::collector::core::Service::prepareTask(CollectorId collectorId)
{
  try
  {
    taskQueue_.emplace(std::bind(&collect, collectorId, dataManager_));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::collector::core::Service::addToQueue(CollectorId collectorId)
{
  collectorQueue_.push(collectorId);
}

void terrama2::services::collector::core::Service::collect(CollectorId collectorId, std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();

  auto collectorPtr = dataManager->findCollector(collectorId);

  auto inputDataSeries = dataManager->findDataSeries(collectorPtr->inputDataSeries);
  auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

  terrama2::core::Filter filter;
  auto dataAccessor = terrama2::core::DataAccessFactory::getInstance().makeDataAccessor(inputDataProvider, inputDataSeries);
  auto dataMap = dataAccessor->getSeries(filter);

  auto outputDataSeries = dataManager->findDataSeries(collectorPtr->outputDataSeries);
  auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);
  //FIXME: create storager
}
