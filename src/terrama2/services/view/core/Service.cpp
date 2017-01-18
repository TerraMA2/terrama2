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
  \file terrama2/services/view/core/Service.hpp

  \brief Class for the view configuration.

  \author Vinicius Campanha
*/

// TerraMA2
#include "Service.hpp"
#include "View.hpp"
#include "MemoryDataSetLayer.hpp"
#include "Utils.hpp"
#include "MapsServerFactory.hpp"

#include "data-access/Geoserver.hpp"

#include "../../../core/Shared.hpp"
#include "../../../core/utility/TimeUtils.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../impl/DataAccessorFile.hpp"
#include "../../../impl/DataAccessorPostGIS.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"

// Qt
#include <QUrl>
#include <QJsonArray>

terrama2::services::view::core::Service::Service(std::weak_ptr<terrama2::services::view::core::DataManager> dataManager)
  : dataManager_(dataManager)
{
  connectDataManager();

  registerFactories();
}


void terrama2::services::view::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    taskQueue_.emplace(std::bind(&Service::viewJob, this, executionPackage, std::dynamic_pointer_cast<terrama2::services::view::core::ViewLogger>(logger_->clone()), dataManager_));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::view::core::Service::addToQueue(ViewId viewId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);
    TERRAMA2_LOG_DEBUG() << tr("View %1 added to queue.").arg(viewId);

    auto datamanager = dataManager_.lock();
    auto view = datamanager->findView(viewId);

    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this view should be executed in this instance
    if(view->serviceInstanceId != serviceInstanceId)
      return;

    RegisterId registerId = logger_->start(viewId);

    terrama2::core::ExecutionPackage executionPackage;
    executionPackage.processId = viewId;
    executionPackage.executionDate = startTime;
    executionPackage.registerId = registerId;

    if(std::find(processingQueue_.begin(), processingQueue_.end(), viewId) == processingQueue_.end())
    {
      processingQueue_.push_back(viewId);
      processQueue_.push_back(executionPackage);
      mainLoopCondition_.notify_one();
    }
    else
    {
      waitQueue_[viewId].push(executionPackage);
      logger_->result(ViewLogger::ON_QUEUE, nullptr, executionPackage.registerId);
      TERRAMA2_LOG_DEBUG() << tr("View %1 added to wait queue.").arg(viewId);
    }

  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::services::view::core::Service::connectDataManager()
{
  auto dataManager = dataManager_.lock();
  connect(dataManager.get(), &terrama2::services::view::core::DataManager::viewAdded, this,
          &terrama2::services::view::core::Service::addProcessToSchedule);
  connect(dataManager.get(), &terrama2::services::view::core::DataManager::viewRemoved, this,
          &terrama2::services::view::core::Service::removeView);
  connect(dataManager.get(), &terrama2::services::view::core::DataManager::viewUpdated, this,
          &terrama2::services::view::core::Service::updateView);
}

void terrama2::services::view::core::Service::removeView(ViewId viewId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);


    TERRAMA2_LOG_INFO() << tr("Removing view %1.").arg(viewId);

    auto it = timers_.find(viewId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(viewId);
      timer->disconnect();
      timers_.erase(viewId);
    }

    // remove from queue
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [viewId](const terrama2::core::ExecutionPackage& executionPackage)
                                       { return viewId == executionPackage.processId; }), processQueue_.end());

    auto itWaitQueue = waitQueue_.find(viewId);
    waitQueue_.erase(itWaitQueue);


    TERRAMA2_LOG_INFO() << tr("View %1 removed successfully.").arg(viewId);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove view: %1.").arg(viewId);
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove view: %1.").arg(viewId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove view: %1.").arg(viewId);
  }
}

void terrama2::services::view::core::Service::updateView(ViewPtr view) noexcept
{
  removeView(view->id);
  addProcessToSchedule(view);
}

void terrama2::services::view::core::Service::viewJob(const terrama2::core::ExecutionPackage& executionPackage,
                                                      std::shared_ptr<ViewLogger> logger,
                                                      std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();

  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access DataManager");
    notifyWaitQueue(executionPackage.processId);
    sendProcessFinishedSignal(executionPackage.processId, false);
    return;
  }

  RegisterId logId = executionPackage.registerId;
  ViewId viewId = executionPackage.processId;

  QJsonObject jsonAnswer;

  try
  {
    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting view %1 generation.").arg(viewId);

    auto mapsServer = MapsServerFactory::getInstance().make(mapsServerUri_, "GEOSERVER");

    /////////////////////////////////////////////////////////////////////////
    //  aquiring metadata

    auto lock = dataManager->getLock();

    auto viewPtr = dataManager->findView(viewId);

    std::unordered_map< terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr > dataSeriesProviders;
    for(auto dataSeriesId : viewPtr->dataSeriesList)
    {
      terrama2::core::DataSeriesPtr inputDataSeries = dataManager->findDataSeries(dataSeriesId);
      terrama2::core::DataProviderPtr inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

      dataSeriesProviders.emplace(inputDataSeries, inputDataProvider);
    }

    lock.unlock();

    /////////////////////////////////////////////////////////////////////////

    QJsonObject mapsServerAnswer = mapsServer->generateLayers(viewPtr, dataSeriesProviders, dataManager, logger, logId);

    jsonAnswer = mapsServerAnswer;
    jsonAnswer.insert("class", QString("RegisteredViews"));
    jsonAnswer.insert("process_id",static_cast<int32_t>(viewPtr->id));
    jsonAnswer.insert("maps_server_uri", QString::fromStdString(mapsServerUri_.uri()));

    TERRAMA2_LOG_INFO() << tr("View %1 generated successfully.").arg(viewId);

    logger->result(ViewLogger::DONE, terrama2::core::TimeUtils::nowUTC(), logId);

    sendProcessFinishedSignal(viewId, true, jsonAnswer);
    notifyWaitQueue(viewId);

    return;
  }
  catch(const terrama2::core::LogException& e)
  {
    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    TERRAMA2_LOG_ERROR() << errMsg << std::endl;
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);
  }
  catch(const terrama2::Exception& e)
  {
    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    TERRAMA2_LOG_ERROR() << errMsg << std::endl;
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->log(ViewLogger::ERROR_MESSAGE, errMsg, logId);
  }
  catch(const boost::exception& e)
  {
    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->log(ViewLogger::ERROR_MESSAGE, errMsg, logId);
  }
  catch(const std::exception& e)
  {
    std::string errMsg = e.what();
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->log(ViewLogger::ERROR_MESSAGE, errMsg, logId);
  }
  catch(...)
  {
    std::string errMsg = "Unknown error.";
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown error.");
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->log(ViewLogger::ERROR_MESSAGE, errMsg, logId);
  }

  sendProcessFinishedSignal(viewId, false);
  notifyWaitQueue(viewId);
}


void terrama2::services::view::core::Service::updateAdditionalInfo(const QJsonObject& obj) noexcept
{
  if(!obj.contains("maps_server_uri"))
  {
    TERRAMA2_LOG_ERROR() << tr("Missing the Maps Server URI in service additional info!");
  }
  else
  {
    mapsServerUri_ = te::core::URI(obj["maps_server_uri"].toString().toStdString());
  }
}
