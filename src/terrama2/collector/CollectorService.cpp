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
  \file terrama2/collector/CollectorService.cpp

  \brief  Manages the collection of data in the appropriate time.

  \author Jano Simas, Paulo R. M. Oliveira, Vinicius Campanha
*/

// TerraMA2
#include "CollectorService.hpp"
#include "DataRetriever.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "Log.hpp"
#include "Storager.hpp"
#include "Factory.hpp"
#include "Parser.hpp"
#include "Utils.hpp"
#include "IntersectionOperation.hpp"
#include "TransferenceData.hpp"
#include "../core/DataSet.hpp"
#include "../core/DataProvider.hpp"
#include "../core/DataManager.hpp"
#include "../core/ApplicationController.hpp"
#include "../core/Logger.hpp"

// Terralib
#include <terralib/common/Exception.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>


// QT
#include <QApplication>
#include <QDebug>
#include <QMap>

// STL
#include <memory>
#include <cassert>
#include <iostream>
#include <functional>
#include <utility>


void terrama2::collector::CollectorService::populateData()
{
  const auto& dataManager = core::DataManager::getInstance();
  const std::vector<terrama2::core::DataProvider>& dataproviderVec = dataManager.providers();
  foreach (const auto& dataprovider, dataproviderVec)
    addProvider(dataprovider);

  std::vector<terrama2::core::DataSet> datasetVec = dataManager.dataSets();
  foreach (const auto& dataset, datasetVec)
    addDataset(dataset);
}

terrama2::collector::CollectorService::CollectorService(QObject *parent)
  : QObject(parent),
    stop_(false)
{
  connectDataManager();
}

terrama2::collector::CollectorService::~CollectorService()
{
  stop();
}

void terrama2::collector::CollectorService::start(int threadNumber)
{
// if service already running, throws
  if(loopThread_.valid())
    throw ServiceAlreadyRunnningException() << terrama2::ErrorDescription(tr("Collector service already running."));

  try
  {
    stop_ = false;
    //start the loop thread
    loopThread_ = std::async(std::launch::async, &CollectorService::processingLoop, this);

    //check for the number o threads to create
    if(threadNumber)
      threadNumber = std::thread::hardware_concurrency(); //looks for how many threads the hardware support
    if(!threadNumber)
      threadNumber = 1; //if not able to find out set to 1

    //Starts collection threads
    for (int i = 0; i < threadNumber; ++i)
      threadPool_.push_back(std::async(std::launch::async, &CollectorService::threadProcess, this));
  }
  catch(const std::exception& e)
  {
    QString errMsg(tr("Unable to start collector service: %1."));
    errMsg = errMsg.arg(e.what());

    throw UnableToStartServiceException() << terrama2::ErrorDescription(errMsg);
  }

  populateData();
}

void terrama2::collector::CollectorService::stop() noexcept
{
  {
    std::lock_guard<std::mutex> lock (mutex_);
    // finish the thread
    stop_ = true;

    //wake loop thread and collecting threads
    loop_condition_.notify_one();
    thread_condition_.notify_all();
  }

  //wait for the loop thread
  if(loopThread_.valid())
    loopThread_.get();

  //wait for each collectiing thread
  for(auto & future : threadPool_)
  {
    if(future.valid())
      future.get();
  }
}

void terrama2::collector::CollectorService::addProvider(const core::DataProvider& provider)
{
  try
  {
    //lock thread
    std::lock_guard<std::mutex> lock (mutex_);

    //removes the provider if already in the map
    std::map<uint64_t, core::DataProvider>::const_iterator hasProvider = dataproviders_.find(provider.id());
    if(hasProvider != dataproviders_.end())
      dataproviders_.erase(hasProvider);

    //add the provider
    auto ok = dataproviders_.emplace(provider.id(), provider); // register the data provider
    assert(ok.second);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::collector::CollectorService::process(const uint64_t dataProviderID, const std::vector<uint64_t>& dataSetVect)
{
  try
  {
    std::list<core::DataSet> datasetLst;
    for(auto datasetID : dataSetVect)
      datasetLst.push_back(datasets_.at(datasetID));

    taskQueue_.emplace(std::bind(&collect, dataproviders_.at(dataProviderID), datasetLst));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::process " << e.what();
  }
}

void terrama2::collector::CollectorService::collect(const terrama2::core::DataProvider &dataProvider, const std::list<terrama2::core::DataSet>& dataSetList)
{
  //if not active, nothing to do
  if(dataProvider.status() != core::DataProvider::ACTIVE)
    return;

  try
  {
    DataRetrieverPtr retriever = Factory::makeRetriever(dataProvider);
    assert(retriever);

    OpenClose<DataRetrieverPtr> openClose(retriever);
    if(!retriever->isOpen())
    {
      TERRAMA2_LOG_WARNING() << "Could not open data retriever";
      return;
    }

    for(auto &dataSet : dataSetList)
    {
      //If not active, continue...
      if(dataSet.status() != core::DataSet::ACTIVE)
        continue;


      if(dataSet.dataSetItems().empty())
      {
        TERRAMA2_LOG_WARNING() << "There is no dataset items in " + dataSet.name();
        continue;
      }

      //acquire all data
      for(auto& dataSetItem : dataSet.dataSetItems())
      {
        //if not active, continue...
        if(dataSetItem.status() != core::DataSetItem::ACTIVE)
          continue;


        try
        {
//          std::shared_ptr< te::da::DataSourceTransactor > transactor(terrama2::core::ApplicationController::getInstance().getTransactor());
//          terrama2::collector::Log collectLog(transactor);


          std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime;// = collectLog.getDataSetItemLastDateTime(dataSetItem.id());
          DataFilterPtr filter = std::make_shared<DataFilter>(dataSetItem, lastLogTime);
          assert(filter);

          std::vector<TransferenceData> transferenceDataVec;
          //TODO: conditions to collect Data?
          if(retriever->isRetrivable())//retrieve remote data to local temp file.
          {
            retriever->retrieveData(dataSetItem, filter, transferenceDataVec);

            //Log: data downloaded
//            if(!transferenceDataVec.empty())
//              collectLog.log(transferenceDataVec, Log::Status::DOWNLOADED);

          }
          else// if data don't need to be retrieved (ex. local, wms, wmf)
          {
            TransferenceData tmp;
            tmp.uri_origin = dataProvider.uri() + "/" + dataSetItem.mask();
            transferenceDataVec.push_back(tmp);
          }

          //update tranferenceData info
          for(TransferenceData& transferenceData : transferenceDataVec)
          {
            boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("+00"));
            boost::local_time::local_date_time boostTime(boost::posix_time::second_clock::universal_time(), zone);
            transferenceData.date_collect.reset(new te::dt::TimeInstantTZ(boostTime));
            transferenceData.dataset = dataSet;
            transferenceData.datasetItem = dataSetItem;
          }

          ParserPtr parser = Factory::makeParser(dataSetItem);
          assert(parser);

          //read data and create a terralib dataset
          parser->read(filter, transferenceDataVec);

          //no new dataset found
          if(transferenceDataVec.empty())
            continue;

//          filter dataset data (date, geometry, ...)
          for(auto& transferenceData : transferenceDataVec)
          {
            filter->filterDataSet(transferenceData);
            terrama2::collector::processIntersection(transferenceData);
          }

          //store dataset
          StoragerPtr storager = Factory::makeStorager(dataSetItem);
          assert(storager);
          storager->store(transferenceDataVec);

//          collectLog.updateLog(transferenceDataVec, Log::Status::IMPORTED);

          // Dataset Logger Success
          TERRAMA2_LOG_INFO() << "DataSet \"" << dataSet.name() + "\" has just been collected!";
        }
        catch(terrama2::Exception& e)
        {
          TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::collectAsThread "
                               << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
          continue;
        }
        catch(std::exception& e)
        {
          TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::collectAsThread " << e.what();
          continue;
        }
      }
    }
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::collectAsThread " << e.what();
  }
}

void terrama2::collector::CollectorService::threadProcess()
{
  try
  {
    while (true) {
      std::packaged_task<void()> task;

      {
        std::unique_lock<std::mutex> lock(mutex_);
        //wait for new data to be collected
        thread_condition_.wait(lock, [this] { return !taskQueue_.empty() || stop_; });

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
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::threadProcess " << e.what();
  }
}

void terrama2::collector::CollectorService::processingLoop()
{
  while(true)
  {
    try
    {
      std::unique_lock<std::mutex> lock(mutex_);
      //wait for new data to collect
      loop_condition_.wait(lock, [this]{ return !collectQueue_.empty() || stop_; });

      if(stop_)
        break;

      // For each provider type verifies if the first provider in the queue is acquiring new data,
      // in case it's collecting moves to next type of provider, when it's done remove it from the queue
      // in case it's not collecting, starts the collection calling the collect method.
      // It allows multiples providers to collect at the same time but only one provider of each type.
      for (auto it = collectQueue_.begin(); it != collectQueue_.end(); ++it)
      {
        const auto& dataProviderId = it->first;
        const auto& dataSetQueue = it->second;

        process(dataProviderId, dataSetQueue);

        if(stop_)
          break;
      }

      collectQueue_.clear();

      //wake collecting threads
      thread_condition_.notify_all();

      if(stop_)
        break;
    }
    catch(std::exception& e)
    {
      TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::processingLoop " << e.what();
    }
  }
}

void terrama2::collector::CollectorService::connectDataManager()
{
  core::DataManager &dataManager = core::DataManager::getInstance();

  //DataProvider signals
  connect(&dataManager, &terrama2::core::DataManager::dataProviderAdded,   this, &terrama2::collector::CollectorService::addProvider,    Qt::UniqueConnection);
  connect(&dataManager, &terrama2::core::DataManager::dataProviderRemoved, this, &terrama2::collector::CollectorService::removeProvider, Qt::UniqueConnection);
  connect(&dataManager, &terrama2::core::DataManager::dataProviderUpdated, this, &terrama2::collector::CollectorService::updateProvider, Qt::UniqueConnection);

  //dataset signals
  connect(&dataManager, &terrama2::core::DataManager::dataSetAdded  , this, &terrama2::collector::CollectorService::addDataset       , Qt::UniqueConnection);
  connect(&dataManager, &terrama2::core::DataManager::dataSetRemoved, this, &terrama2::collector::CollectorService::removeDatasetById, Qt::UniqueConnection);
  connect(&dataManager, &terrama2::core::DataManager::dataSetUpdated, this, &terrama2::collector::CollectorService::updateDataset    , Qt::UniqueConnection);
}

void terrama2::collector::CollectorService::addToQueue(uint64_t datasetId)
{
  try
  {
    //Lock Thread and add to the queue
    std::lock_guard<std::mutex> lock(mutex_);

    const auto& dataset = datasets_.at(datasetId);

    //Append the dataset to queue
    auto& datasetQueue = collectQueue_[dataset.provider()];
    //NOTE: A dataset can only appear once in the queue.
    if(std::find(datasetQueue.begin(), datasetQueue.end(), datasetId) == datasetQueue.end())
      datasetQueue.push_back(datasetId);

    //wake loop thread
    loop_condition_.notify_one();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::addToQueue " << e.what();
  }

}

void terrama2::collector::CollectorService::removeProvider(const terrama2::core::DataProvider& dataProvider)
{
  for(const core::DataSet& dataSet : dataProvider.datasets())
    removeDatasetById(dataSet.id());


  std::lock_guard<std::mutex> lock (mutex_);

  try
  {
    dataproviders_.erase(dataProvider.id());
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::removeProvider " << e.what();
  }
}

void terrama2::collector::CollectorService::updateProvider(const core::DataProvider &dataProvider)
{
  try
  {
    std::lock_guard<std::mutex> lock (mutex_);

    dataproviders_.at(dataProvider.id()) = dataProvider;
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::updateProvider " << e.what();
  }
}

void
terrama2::collector::CollectorService::addDataset(const core::DataSet &dataset)
{
  if(dataset.status() != core::DataSet::ACTIVE
     || dataset.id() == 0)
    return;

  try
  {
    {
      std::lock_guard<std::mutex> lock (mutex_);

      datasets_.insert(std::make_pair(dataset.id(), dataset));

// create a new dataset timer and connect the timeout signal to queue
      auto datasetTimer = std::make_shared<DataSetTimer>(dataset); // std::shared_ptr<DataSetTimer>(new DataSetTimer(dataset));
      timers_.insert(std::make_pair(dataset.id(), datasetTimer));

      connect(datasetTimer.get(), SIGNAL(timerSignal(uint64_t)), this, SLOT(addToQueue(uint64_t)), Qt::UniqueConnection);
    }

// add to queue to collect a first time
    addToQueue(dataset.id());
  }
  catch(terrama2::collector::InvalidCollectFrequencyException& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::addDataset "
                         << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
  }
  catch(terrama2::collector::InvalidDataSetException& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::addDataset "
                         << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::addDataset " << e.what();
  }

  return;
}

void terrama2::collector::CollectorService::removeDataset(const terrama2::core::DataSet& dataset)
{
  removeDatasetById(dataset.id());
}

void terrama2::collector::CollectorService::removeDatasetById(uint64_t datasetId)
{
  std::lock_guard<std::mutex> lock (mutex_);

  try
  {
    //only valid dataset are added to the map,
    //if an inactive or invalid dataset is removed: nothing to do
    std::map<uint64_t, DataSetTimerPtr>::const_iterator pos = timers_.find(datasetId);
    if(pos == timers_.end())
      return;

    const DataSetTimerPtr& datasetTimer = pos->second;
    disconnect(datasetTimer.get(), nullptr, this, nullptr);

    datasets_.erase(datasetId);
    timers_.erase(datasetId);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "terrama2::collector::CollectorService::removeDatasetById " << e.what();
  }

}

void terrama2::collector::CollectorService::updateDataset(const core::DataSet &dataset)
{
  removeDataset(dataset);
  addDataset(dataset);
}
