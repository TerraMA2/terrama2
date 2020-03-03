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
  \file terrama2/services/collector/core/Service.cpp

  \brief

  \author Jano Simas
*/

#include "Service.hpp"
#include "Collector.hpp"
#include "CollectorLogger.hpp"
#include "IntersectionOperation.hpp"

#include "../../../core/Shared.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"
#include "../../../core/utility/FileRemover.hpp"

#include "../../../core/Exception.hpp"

terrama2::services::collector::core::Service::Service(std::weak_ptr<terrama2::core::DataManager> dataManager)
  : terrama2::core::Service(dataManager)
{
  connectDataManager();
}

void terrama2::services::collector::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    auto dataManager = std::static_pointer_cast<terrama2::services::collector::core::DataManager>(dataManager_.lock());
    auto collectorLogger = std::static_pointer_cast<CollectorLogger>(logger_->clone());
    assert(collectorLogger);
    taskQueue_.emplace(std::bind(&terrama2::services::collector::core::Service::collect, this, executionPackage, collectorLogger, dataManager));
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::collector::core::Service::collect(terrama2::core::ExecutionPackage executionPackage,
                                                           std::shared_ptr<CollectorLogger> logger,
                                                           std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << tr("Unable to access DataManager");
    notifyWaitQueue(executionPackage.processId);
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
    return;
  }

  // error message to be logged
  QString errMsg;
  try
  {
    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto lock = dataManager->getLock();

    auto collectorPtr = dataManager->findCollector(executionPackage.processId);

    // input data
    auto inputDataSeries = dataManager->findDataSeries(collectorPtr->inputDataSeries);
    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

    TERRAMA2_LOG_DEBUG() << tr("Starting collection for data series '%1'").arg(inputDataSeries->name.c_str());

    // output data
    auto outputDataSeries = dataManager->findDataSeries(collectorPtr->outputDataSeries);
    auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    //  recovering data

    auto processingStartTime = terrama2::core::TimeUtils::nowUTC();

    terrama2::core::Filter filter = collectorPtr->filter;
    if(filter.discardAfter && filter.discardBefore
        && (*filter.discardAfter) < (*filter.discardBefore))
    {
      QString errMsg = QObject::tr("Empty filter time range.");

      TERRAMA2_LOG_WARNING() << errMsg.toStdString();
      throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
    }

    //update filter based on last collected data timestamp
    updateFilterDiscardDates(filter, logger, executionPackage.processId);

    auto remover = std::make_shared<terrama2::core::FileRemover>();
    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);

    std::shared_ptr<te::dt::TimeInstantTZ> lastDateTime;
    auto status = CollectorLogger::ProcessLogger::Status::START;
    bool verifyStatusDoneExist = false;

    /////////////////////////////////
    //
    // begin processing
    //
    dataAccessor->getSeriesCallback(filter, remover, [&](const DataSetId& datasetId, const std::string& uri) {
      std::shared_ptr<te::dt::TimeInstantTZ> thisFileLastDateTime;
      try
      {
        // if some error happened
        // don't continue
        if(status == CollectorLogger::ProcessLogger::Status::ERROR)
          return;

        auto dataMap = dataAccessor->getSeries({{datasetId, uri}}, filter, remover);
        if(dataMap.empty())
        {
          if(!verifyStatusDoneExist)
          {
            QString errMsg = tr("No data to collect.");
            logger->log(CollectorLogger::MessageType::WARNING_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
            logger->result(CollectorLogger::Status::WARNING, nullptr, executionPackage.registerId);
            TERRAMA2_LOG_WARNING() << errMsg;

            // yellow status to the user
            status = CollectorLogger::ProcessLogger::Status::WARNING;
            return;
          }
        }

        thisFileLastDateTime = dataAccessor->lastDateTime();

        /////////////////////////////////////////////////////////////////////////
        // data intersection

        // intersection
        if(collectorPtr->intersection)
        {
          for(auto& item : dataMap)
          {
            //FIXME: the datamanager is being used outside the lock
            item.second = processIntersection(dataManager, collectorPtr->intersection, item.second, executionPackage.executionDate);
          }
        }

        /////////////////////////////////////////////////////////////////////////
        // storing data

        auto inputOutputMap = collectorPtr->inputOutputMap;
        auto dataSetLst = outputDataSeries->datasetList;
        auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(outputDataSeries, outputDataProvider);

        dataStorager->store(dataMap, dataSetLst, inputOutputMap);

        // if any exception happens, don't update the data timestamp
        if( terrama2::core::TimeUtils::isValid(thisFileLastDateTime)
            && (!terrama2::core::TimeUtils::isValid(lastDateTime) || (*thisFileLastDateTime > *lastDateTime))) {
          lastDateTime = thisFileLastDateTime;
        }

        verifyStatusDoneExist = true;

        status = CollectorLogger::ProcessLogger::Status::DONE;
      }
      catch(const terrama2::core::LogException& e)
      {
        status = CollectorLogger::ProcessLogger::Status::ERROR;
        std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
        TERRAMA2_LOG_INFO() << tr("Collector %1 finished with a log exception.").arg(QString::fromStdString(outputDataSeries->name));
        TERRAMA2_LOG_ERROR() << errMsg;
      }
      catch(const terrama2::core::NoDataException& e)
      {
        if(!verifyStatusDoneExist)
        {
          status = CollectorLogger::ProcessLogger::Status::WARNING;
          std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
          TERRAMA2_LOG_INFO() << tr("No data available for collector %1.").arg(QString::fromStdString(outputDataSeries->name));

          if(executionPackage.registerId != 0)
            logger->log(CollectorLogger::MessageType::WARNING_MESSAGE, errMsg, executionPackage.registerId);
        }
      }
      catch(const terrama2::core::DataAccessorException& e)
      {
        status = CollectorLogger::ProcessLogger::Status::ERROR;
        std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
        TERRAMA2_LOG_INFO() << errMsg;

        if(executionPackage.registerId != 0)
          logger->log(CollectorLogger::MessageType::ERROR_MESSAGE, errMsg, executionPackage.registerId);
      }
      catch(const terrama2::Exception& e)
      {
        status = CollectorLogger::ProcessLogger::Status::ERROR;
        QString errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
        TERRAMA2_LOG_INFO() << tr("Collector %1 finished with error(s).").arg(QString::fromStdString(outputDataSeries->name));

        if(executionPackage.registerId != 0)
          logger->log(CollectorLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      }
      catch(const boost::exception& e)
      {
        status = CollectorLogger::ProcessLogger::Status::ERROR;
        std::string errMsg = boost::diagnostic_information(e);
        TERRAMA2_LOG_ERROR() << errMsg;
        TERRAMA2_LOG_INFO() << tr("Collector %1 finished with error(s).").arg(QString::fromStdString(outputDataSeries->name));

        if(executionPackage.registerId != 0)
          logger->log(CollectorLogger::MessageType::ERROR_MESSAGE, errMsg, executionPackage.registerId);
      }
      catch(const std::exception& e)
      {
        status = CollectorLogger::ProcessLogger::Status::ERROR;
        TERRAMA2_LOG_ERROR() << e.what();
        TERRAMA2_LOG_INFO() << tr("Collector %1 finished with error(s).").arg(QString::fromStdString(outputDataSeries->name));

        if(executionPackage.registerId != 0)
          logger->log(CollectorLogger::MessageType::ERROR_MESSAGE, e.what(), executionPackage.registerId);
      }
      catch(...)
      {
        QString errMsg = tr("Unknown error.");
        TERRAMA2_LOG_ERROR() << errMsg;
        TERRAMA2_LOG_INFO() << tr("Collector %1 finished with error(s).").arg(QString::fromStdString(outputDataSeries->name));

        status = CollectorLogger::ProcessLogger::Status::ERROR;
        if(executionPackage.registerId != 0)
          logger->log(CollectorLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      }

      logger->result(CollectorLogger::ProcessLogger::Status::START, thisFileLastDateTime, executionPackage.registerId);
    });
    //
    // end processing
    //
    /////////////////////////////////

    TERRAMA2_LOG_INFO() << tr("Collector %1 finished successfully.").arg(QString::fromStdString(outputDataSeries->name));

    auto processingEndTime = terrama2::core::TimeUtils::nowUTC();

    // log processing time
    logger->setStartProcessingTime(processingStartTime, executionPackage.registerId);
    logger->setEndProcessingTime(processingEndTime, executionPackage.registerId);

    logger->result(status, lastDateTime, executionPackage.registerId);

    QJsonObject jsonAnswer;
    // only execute linked automatic process if finished successfully
    jsonAnswer.insert(terrama2::core::ReturnTags::AUTOMATIC, status == CollectorLogger::ProcessLogger::Status::DONE);

    auto success = (status == CollectorLogger::ProcessLogger::Status::DONE || status == CollectorLogger::ProcessLogger::Status::WARNING);
    // notify process finished
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, success, jsonAnswer);
    notifyWaitQueue(executionPackage.processId);
    return;
  }
  catch(const terrama2::core::NoDataException& e)
  {
    TERRAMA2_LOG_INFO() << tr("Collector %1 finished but there was no data available.").arg(executionPackage.processId);

    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    if(executionPackage.registerId != 0)
    {
      logger->log(CollectorLogger::MessageType::WARNING_MESSAGE, errMsg, executionPackage.registerId);
      logger->result(CollectorLogger::Status::WARNING, nullptr, executionPackage.registerId);
    }

    QJsonObject jsonAnswer;
    jsonAnswer.insert(terrama2::core::ReturnTags::AUTOMATIC, false);
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true, jsonAnswer);
    notifyWaitQueue(executionPackage.processId);
    return;
  }
  catch(const terrama2::core::LogException& e)
  {
    errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(const terrama2::Exception& e)
  {
    errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(const boost::exception& e)
  {
    errMsg = QString::fromStdString(boost::diagnostic_information(e));
  }
  catch(const std::exception& e)
  {
    errMsg = e.what();
  }
  catch(...)
  {
    errMsg = tr("Unknown error.");
  }

  if(executionPackage.registerId != 0)
  {
    logger->log(CollectorLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    logger->result(CollectorLogger::Status::ERROR, nullptr, executionPackage.registerId);
  }

  TERRAMA2_LOG_ERROR() << errMsg;
  TERRAMA2_LOG_INFO() << tr("Collector %1 finished with error(s).").arg(executionPackage.processId);

  // if arrived here, always error
  sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
  notifyWaitQueue(executionPackage.processId);
}

void terrama2::services::collector::core::Service::connectDataManager()
{
  auto dataManager = std::static_pointer_cast<terrama2::services::collector::core::DataManager>(dataManager_.lock());
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorAdded, this,
          &terrama2::services::collector::core::Service::addProcessToSchedule);
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorRemoved, this,
          &terrama2::services::collector::core::Service::removeCollector);
  connect(dataManager.get(), &terrama2::services::collector::core::DataManager::collectorUpdated, this,
          &terrama2::services::collector::core::Service::updateCollector);
}

void terrama2::services::collector::core::Service::removeCollector(CollectorId collectorId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);


    TERRAMA2_LOG_INFO() << tr("Removing collector %1.").arg(collectorId);

    auto it = timers_.find(collectorId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(collectorId);
      timer->disconnect();
      timers_.erase(collectorId);
    }

    // remove from queue
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [collectorId](const terrama2::core::ExecutionPackage& executionPackage)
                                       { return collectorId == executionPackage.processId; }), processQueue_.end());

    waitQueue_.erase(collectorId);


    TERRAMA2_LOG_INFO() << tr("Collector %1 removed successfully.").arg(collectorId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove collector: %1.").arg(collectorId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove collector: %1.").arg(collectorId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove collector: %1.").arg(collectorId);
  }
}

void terrama2::services::collector::core::Service::updateCollector(CollectorPtr collector) noexcept
{
  removeCollector(collector->id);
  addProcessToSchedule(collector);
}

void terrama2::services::collector::core::Service::updateAdditionalInfo(const QJsonObject& /*obj*/) noexcept
{

}

terrama2::core::ProcessPtr terrama2::services::collector::core::Service::getProcess(ProcessId processId)
{
  auto dataManager = std::static_pointer_cast<terrama2::services::collector::core::DataManager>(dataManager_.lock());
  return dataManager->findCollector(processId);
}
