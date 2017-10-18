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
  \file interpolator/core/Service.cpp

  \brief

  \author Frederico Augusto Bedê
*/

#include "Service.hpp"

#include "../../../core/data-access/DataStorager.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/ServiceManager.hpp"

#include "DataManager.hpp"
#include "InterpolatorFactories.h"
#include "InterpolatorLogger.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/raster/Band.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/RasterProperty.h>


/*!
 * \brief Returns a te::da::DataSetType for a given raster object.
 *
 * \param raster A pointer to a valid raster object.
 *
 * \return The DataSetType.
 */
te::da::DataSetType* GetDataSetType(te::rst::Raster* raster)
{
  te::da::DataSetType* dataSetType = new te::da::DataSetType(raster->getName());

  te::rst::RasterProperty* rp = new te::rst::RasterProperty("raster");

  te::dt::DateTimeProperty* dp = new te::dt::DateTimeProperty("date_time", te::dt::TIME_INSTANT_TZ);

  dataSetType->add(rp);

  dataSetType->add(dp);

  for(size_t i = 0; i < raster->getNumberOfBands(); i++)
    rp->add(new te::rst::BandProperty(*raster->getBand(i)->getProperty()));

  rp->set(new te::rst::Grid(*raster->getGrid()));

  return dataSetType;
}

/*!
 * \brief Returns a DataSet in memory for the given raster.
 *
 * \param raster Pointer to a valid raster object.
 *
 * \param time The time of the data processing.
 *
 * \return A valid dataset.
 */
te::mem::DataSet* GetDataSet(te::rst::Raster* raster, std::shared_ptr<te::dt::TimeInstantTZ> time)
{
  te::mem::DataSet* dset = new te::mem::DataSet(GetDataSetType(raster));

  te::mem::DataSetItem* item = new te::mem::DataSetItem(dset);

  item->setDateTime("date_time", new te::dt::TimeInstantTZ(*time.get()));

  dset->add(item);

  dset->moveFirst();

  dset->setRaster(0, raster);

  return dset;
}

/*!
 * \brief Stores the result into a valid data provider.
 *
 * \param raster Pointer to a valid raster.
 *
 * \param par Parameters used by the interpolator.
 *
 * \param weakDataManager The Dataanager.
 *
 * \param time The time of the data processing.
 *
 * \exception A terrama2::core::NoDataException can be raised.
 */
void StoreInterpolateResult(te::rst::Raster* raster, terrama2::services::interpolator::core::InterpolatorParams* par, std::weak_ptr<terrama2::services::interpolator::core::DataManager> weakDataManager,
                            std::shared_ptr<te::dt::TimeInstantTZ> time)
{
  auto dataManager = weakDataManager.lock();

  if(!dataManager.get())
  {
    QString errMsg = QObject::tr("Unable to access DataManager");

    TERRAMA2_LOG_WARNING() << errMsg.toStdString();

    throw terrama2::core::NoDataException() << terrama2::ErrorDescription(errMsg);
  }

  //////////////////////////////////////////////////////////
  // output data
  auto dataSeriesPtr = dataManager->findDataSeries(par->outSeries_);
  auto dataProvider = dataManager->findDataProvider(dataSeriesPtr->dataProviderId);
  /////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////
  // storing data
  auto accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
  auto remover = std::make_shared<terrama2::core::FileRemover>();

  std::shared_ptr<te::da::DataSet> dsetRasterPtr(GetDataSet(raster, time));

  auto teDset = new terrama2::core::SynchronizedDataSet(dsetRasterPtr);
  std::shared_ptr<terrama2::core::SynchronizedDataSet> teDsetPtr(teDset);

  terrama2::core::DataSetSeries series;
  series.syncDataSet = teDsetPtr;

  auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(dataSeriesPtr, dataProvider);

  dataStorager->store(series, *dataSeriesPtr->datasetList.begin());
}

/// End utilities functions section.
/*! -------------------------------*/

terrama2::services::interpolator::core::Service::Service(std::weak_ptr<DataManager> dataManager):
  dataManager_(dataManager)
{
  connectDataManager();

  InterpolatorFactories::initialize();
}

terrama2::services::interpolator::core::Service::~Service()
{
  InterpolatorFactories::finalize();
}

void terrama2::services::interpolator::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    auto interpolatorLogger = std::dynamic_pointer_cast<InterpolatorLogger>(logger_->clone());
    assert(interpolatorLogger);
    taskQueue_.emplace(std::bind(&terrama2::services::interpolator::core::Service::interpolate, this, executionPackage, interpolatorLogger, dataManager_));
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::interpolator::core::Service::interpolate(terrama2::core::ExecutionPackage executionPackage, std::shared_ptr<InterpolatorLogger> logger, std::weak_ptr<terrama2::services::interpolator::core::DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();

  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << tr("Unable to access DataManager");
    notifyWaitQueue(executionPackage.processId);
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
    return;
  }

  try
  {
    //////////////////////////////////////////////////////////
    //  aquiring metadata

    auto lock = dataManager->getLock();
    auto interpolatorParamsPtr = dataManager->findInterpolatorParams(executionPackage.processId);

    // dataManager no longer in use
    lock.unlock();
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    //  recovering data

    auto processingStartTime = terrama2::core::TimeUtils::nowUTC();

    terrama2::core::Filter filter = interpolatorParamsPtr->filter_;

    if(filter.lastValues.get() == 0 || *filter.lastValues.get() != 1)
      filter.lastValues = std::make_shared<long unsigned int>(1);

    if(filter.discardAfter.get() && filter.discardBefore.get()
       && (*filter.discardAfter) < (*filter.discardBefore))
    {
      QString errMsg = QObject::tr("Empty filter time range.");

      TERRAMA2_LOG_WARNING() << errMsg.toStdString();
      throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
    }

    auto interpolatorPtr(InterpolatorFactories::make(interpolatorParamsPtr->interpolationType_, interpolatorParamsPtr));

    auto res = interpolatorPtr->makeInterpolation();

    TERRAMA2_LOG_INFO() << tr("Data from process %1 interpolated successfully.").arg(executionPackage.processId);

    StoreInterpolateResult(res.get(), interpolatorParamsPtr.get(), dataManager, executionPackage.executionDate);

    TERRAMA2_LOG_INFO() << tr("Data from process %1 stored successfully.").arg(executionPackage.processId);

    auto processingEndTime = terrama2::core::TimeUtils::nowUTC();

    logger->setStartProcessingTime(processingStartTime, executionPackage.registerId);

    logger->setEndProcessingTime(processingEndTime, executionPackage.registerId);

    logger->result(InterpolatorLogger::DONE, processingEndTime, executionPackage.registerId);

    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true);

    notifyWaitQueue(executionPackage.processId);
  }
  catch(const terrama2::core::LogException& e)
  {
    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    if(executionPackage.registerId != 0 )
    {
      TERRAMA2_LOG_ERROR() << errMsg << std::endl;
      TERRAMA2_LOG_INFO() << tr("Collection for interpolator %1 finished with error(s).").arg(executionPackage.processId);
    }
  }
  catch(const terrama2::core::NoDataException&)
  {
    TERRAMA2_LOG_INFO() << tr("Collection finished but there was no data available for interpolator %1.").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(InterpolatorLogger::WARNING_MESSAGE, tr("No data available").toStdString(), executionPackage.registerId);
      logger->result(InterpolatorLogger::DONE, nullptr, executionPackage.registerId);
    }

    QJsonObject jsonAnswer;
    jsonAnswer.insert(terrama2::core::ReturnTags::AUTOMATIC, false);
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true, jsonAnswer);
    notifyWaitQueue(executionPackage.processId);
  }
  catch(const terrama2::Exception& e)
  {
    QString errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Collection for interpolator %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(InterpolatorLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      logger->result(InterpolatorLogger::ERROR, nullptr, executionPackage.registerId);
    }
  }
  catch(const boost::exception& e)
  {
    std::string errMsg = boost::diagnostic_information(e);
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << tr("Collection for interpolator %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(InterpolatorLogger::ERROR_MESSAGE, errMsg, executionPackage.registerId);
      logger->result(InterpolatorLogger::ERROR, nullptr, executionPackage.registerId);
    }
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Collection for interpolator %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(InterpolatorLogger::ERROR_MESSAGE, e.what(), executionPackage.registerId);
      logger->result(InterpolatorLogger::ERROR, nullptr, executionPackage.registerId);
    }
  }
  catch(...)
  {
    QString errMsg = tr("Unknown error.");
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << tr("Collection for interpolator %1 finished with error(s).").arg(executionPackage.processId);

    if(executionPackage.registerId != 0)
    {
      logger->log(InterpolatorLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      logger->result(InterpolatorLogger::ERROR, nullptr, executionPackage.registerId);
    }
  }
}

void terrama2::services::interpolator::core::Service::addToQueue(InterpolatorId interpolatorId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);

    auto datamanager1 = dataManager_.lock();
    auto interpolator = datamanager1->findInterpolatorParams(interpolatorId);

    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this interpolator should be executed in this instance
    if(interpolator->serviceInstanceId_ != serviceInstanceId)
      return;

    RegisterId registerId = logger_->start(interpolatorId);

    terrama2::core::ExecutionPackage executionPackage;
    executionPackage.processId = interpolatorId;
    executionPackage.executionDate = startTime;
    executionPackage.registerId = registerId;

    // if this interpolator id is already being processed put it on the wait queue
    auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), interpolatorId);
    if(pqIt == processingQueue_.end())
    {
      processQueue_.push_back(executionPackage);
      processingQueue_.push_back(interpolatorId);

      //wake loop thread
      mainLoopCondition_.notify_one();
    }
    else
    {
      waitQueue_[interpolatorId].push(executionPackage);
      logger_->result(InterpolatorLogger::ON_QUEUE, nullptr, executionPackage.registerId);
      TERRAMA2_LOG_INFO() << tr("Interpolator %1 added to wait queue.").arg(interpolatorId);
    }

    mainLoopCondition_.notify_one();
  }
  catch(const terrama2::core::LogException&)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access log database.");
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception durring interpolator edd to queue...");
  }
}

void terrama2::services::interpolator::core::Service::connectDataManager()
{
  auto dataManager1 = dataManager_.lock();

  connect(dataManager1.get(), &terrama2::services::interpolator::core::DataManager::interpolatorAdded, this,
          &terrama2::services::interpolator::core::Service::addInterpolator);
  connect(dataManager1.get(), &terrama2::services::interpolator::core::DataManager::interpolatorRemoved, this,
          &terrama2::services::interpolator::core::Service::removeInterpolator);
  connect(dataManager1.get(), &terrama2::services::interpolator::core::DataManager::interpolatorUpdated, this,
          &terrama2::services::interpolator::core::Service::updateInterpolator);
}

void terrama2::services::interpolator::core::Service::addInterpolator(terrama2::services::interpolator::core::InterpolatorParamsPtr params)
{
  InterpolatorPtr i(InterpolatorFactories::make(params->interpolationType_, params));

  addProcessToSchedule(i);
}

void terrama2::services::interpolator::core::Service::removeInterpolator(InterpolatorId interpolatorId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);

    TERRAMA2_LOG_INFO() << tr("Removing interpolator %1.").arg(interpolatorId);

    auto it = timers_.find(interpolatorId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(interpolatorId);
      timer->disconnect();
      timers_.erase(interpolatorId);
    }

    // remove from queue
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [interpolatorId](const terrama2::core::ExecutionPackage& executionPackage)
    { return interpolatorId == executionPackage.processId; }), processQueue_.end());

    waitQueue_.erase(interpolatorId);

    TERRAMA2_LOG_INFO() << tr("Interpolator %1 removed successfully.").arg(interpolatorId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove interpolator: %1.").arg(interpolatorId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove interpolator: %1.").arg(interpolatorId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove interpolator: %1.").arg(interpolatorId);
  }
}

void terrama2::services::interpolator::core::Service::updateInterpolator(InterpolatorParamsPtr params) noexcept
{
  removeInterpolator(params->id_);
  addInterpolator(params);
}

void terrama2::services::interpolator::core::Service::updateAdditionalInfo(const QJsonObject& obj) noexcept
{

}
