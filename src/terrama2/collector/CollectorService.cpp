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
#include "Storager.hpp"
#include "Factory.hpp"
#include "Parser.hpp"
#include "../core/DataSet.hpp"
#include "../core/DataProvider.hpp"
#include "../core/DataManager.hpp"

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>

// QT
#include <QApplication>
#include <QDebug>
#include <QMap>

// STL
#include <memory>
#include <cassert>
#include <iostream>

//terralib
#include <terralib/common/Exception.h>

terrama2::collector::CollectorService::CollectorService(QObject *parent)
  : QObject(parent),
    stop_(false),
    factory_(new Factory())
{
  connectDataManager();
}

terrama2::collector::CollectorService::~CollectorService()
{
  stop();
}

void terrama2::collector::CollectorService::start()
{
// if service already running, throws
  if(loopThread_.joinable())
    throw ServiceAlreadyRunnningError() << terrama2::ErrorDescription(tr("Collector service already running."));

  try
  {
    stop_ = false;
    loopThread_ = std::thread(&CollectorService::processingLoop, this);
  }
  catch(const std::exception& e)
  {
    QString errMsg(tr("Unable to start collector service: %1."));
    errMsg = errMsg.arg(e.what());
    
    throw UnableToStartServiceError() << terrama2::ErrorDescription(errMsg);
  }
}

void terrama2::collector::CollectorService::stop() noexcept
{
  mutex_.lock();
// finish the thread
  stop_ = true;
  mutex_.unlock();

  if(loopThread_.joinable())
    loopThread_.join();
}

void terrama2::collector::CollectorService::addProvider(const core::DataProvider& provider)
{
// TODO: check if provider-id is already registered and log it

  try
  {
    std::lock_guard<std::mutex> lock (mutex_);
    
    dataproviders_.insert(std::make_pair(provider.id(), provider)); // register the data provider
  }
  catch(const std::exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
  }
}

void terrama2::collector::CollectorService::process(uint64_t dataProviderID, const std::vector<uint64_t> &dataSetVect)
{
  std::list<core::DataSet> datasetLst;
  for(auto datasetID : dataSetVect)
    datasetLst.push_back(datasets_.at(datasetID));

  collectAsThread(dataproviders_.at(dataProviderID), datasetLst);
}

void terrama2::collector::CollectorService::collectAsThread(const terrama2::core::DataProvider &dataProvider, const std::list<terrama2::core::DataSet> &dataSetList)
{
  DataRetrieverPtr retriever = Factory::makeRetriever(dataProvider);
  retriever->open();

  for(auto &dataSet : dataSetList)
  {
    if(dataSet.dataSetItems().empty())
    {
      //TODO: LOG empty dataset
      continue;
    }

    //aquire all data
    for(auto& dataSetItem : dataSet.dataSetItems())
    {

      try
      {
        ParserPtr     parser = Factory::makeParser(dataSetItem);
        StoragerPtr   storager = Factory::makeStorager(dataSetItem);
        DataFilterPtr filter(new DataFilter(dataSetItem));

        //TODO: conditions to collect Data?
        //retrieve remote data to local temp file
        std::string uri = retriever->retrieveData(filter);

        //read data and create a terralib dataset
        std::vector<std::shared_ptr<te::da::DataSet> > datasetVec;
        std::shared_ptr<te::da::DataSetType> datasetType;
        parser->read(uri, filter, datasetVec, datasetType);

        //filter dataset data (date, geometry, ...)
        for(int i = 0, size = datasetVec.size(); i < size; ++i)
        {
          std::shared_ptr<te::da::DataSet> tempDataSet = datasetVec.at(i);

          //std::vector::at is NON-const, Qt containers use 'at' as const
          datasetVec.at(i) = filter->filterDataSet(tempDataSet);
        }

        //store dataset
        storager->store(datasetVec, datasetType);
      }
      catch(terrama2::Exception& e)
      {
        //TODO: log this
        continue;
      }
      catch(...)
      {
        //TODO: log this
        // Unkown exception ocurred while.....
        continue;
      }
    }
  }
}

void terrama2::collector::CollectorService::processingLoop()
{
  while(true)
  {
    if(stop_)
      return;

    std::lock_guard<std::mutex> lock(mutex_);

    // For each provider type verifies if the first provider in the queue is acquiring new data,
    // in case it's collecting moves to next type of provider, when it's done remove it from the queue
    // in case it's not collecting, starts the collection calling the collect method.
    // It allows multiples providers to collect at the same time but only one provider of each type.
    for (auto it = collectQueue_.begin(); it != collectQueue_.end(); ++it)
    {
      auto dataProviderId = it->first;
      const auto& dataSetQueue = it->second;

      process(dataProviderId, dataSetQueue);

      collectQueue_.erase(it);
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
  connect(&dataManager, &terrama2::core::DataManager::dataSetAdded  , this, &terrama2::collector::CollectorService::addDataset   , Qt::UniqueConnection);
  connect(&dataManager, &terrama2::core::DataManager::dataSetRemoved, this, &terrama2::collector::CollectorService::removeDataset, Qt::UniqueConnection);
  connect(&dataManager, &terrama2::core::DataManager::dataSetUpdated, this, &terrama2::collector::CollectorService::updateDataset, Qt::UniqueConnection);
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
    //TODO: multiple copies of dataset can occur in the queue, is this the expected behavior?
    datasetQueue.push_back(datasetId);
  }
  catch(...)
  {
    //TODO: log de erro
    assert(0);
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
    //TODO: log de erro
    qDebug() << e.what();
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
    //TODO: log de erro
    qDebug() << e.what();
  }
}

void
terrama2::collector::CollectorService::addDataset(const core::DataSet &dataset)
{
  //TODO: Debug?
  //sanity check: valid dataset
  //  assert(dataset.id());

  try
  {
    {
      std::lock_guard<std::mutex> lock (mutex_);

// create a new dataset timer and connect the timeout signal to queue
      auto datasetTimer = std::make_shared<DataSetTimer>(dataset); // std::shared_ptr<DataSetTimer>(new DataSetTimer(dataset));
      timers_.at(dataset.id()) = datasetTimer;
    
      connect(datasetTimer.get(), SIGNAL(timerSignal(uint64_t)), this, SLOT(addToQueue(uint64_t)), Qt::UniqueConnection);
    }
    
// add to queue to collect a first time
    addToQueue(dataset.id());
  }
  catch(terrama2::collector::InvalidDataSetError& e)
  {
    //TODO: log de erro
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    assert(0);
  }
  catch(...)
  {
    //TODO: log de erro
    assert(0);
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

  const DataSetTimerPtr& datasetTimer = timers_.at(datasetId);
  disconnect(datasetTimer.get(), nullptr, this, nullptr);

  datasets_.erase(datasetId);
  timers_.erase(datasetId);
}

void terrama2::collector::CollectorService::updateDataset(const core::DataSet &dataset)
{
  removeDataset(dataset);
  addDataset(dataset);
}

