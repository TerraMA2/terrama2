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

#include "CollectorService.hpp"
#include "CollectorFactory.hpp"
#include "Exception.hpp"

#include "../core/DataSet.hpp"
#include "../core/DataProvider.hpp"
#include "../core/DataManager.hpp"

// QT
#include <QApplication>
#include <QDebug>
#include <QMap>

// STL
#include <memory>
#include <cassert>
#include <iostream>

//Boost
#include <boost/log/trivial.hpp>


void terrama2::collector::CollectorService::connectDataManager()
{
  core::DataManager &dataManager = core::DataManager::getInstance();
  //DataProvider signals
  connect(&dataManager, SIGNAL(dataProviderAdded(core::DataProviderPtr)), SLOT(addProvider(core::DataProviderPtr)),    Qt::UniqueConnection);
  connect(&dataManager, SIGNAL(dataProviderRemoved(DataProviderPtr)),     SLOT(removeProvider(core::DataProviderPtr)), Qt::UniqueConnection);
  connect(&dataManager, SIGNAL(dataProviderUpdated(DataProviderPtr)),     SLOT(updateProvider(core::DataProviderPtr)), Qt::UniqueConnection);

  //dataset signals
  connect(&dataManager, SIGNAL(dataSetAdded(core::DataSetPtr)),   SLOT(addDataset(core::DataSetPtr)),    Qt::UniqueConnection);
  connect(&dataManager, SIGNAL(dataSetRemoved(core::DataSetPtr)), SLOT(removeDataset(core::DataSetPtr)), Qt::UniqueConnection);
  connect(&dataManager, SIGNAL(dataSetUpdated(core::DataSetPtr)), SLOT(updateDataset(core::DataSetPtr)), Qt::UniqueConnection);
}

terrama2::collector::CollectorService::CollectorService(QObject *parent)
  : QObject(parent),
    stop_(false)
{
  connectDataManager();

}

terrama2::collector::CollectorService::~CollectorService()
{

  mutex_.lock();
  //Finish the thread
  stop_ = true;
  mutex_.unlock();

  if(loopThread_.joinable())
    loopThread_.join();
}

void terrama2::collector::CollectorService::exec()
{
  //if service already running, throws
  if(loopThread_.joinable())
    throw ServiceAlreadyRunnningException() << terrama2::ErrorDescription(tr("Collector service already running."));

  loopThread_ = std::thread(&CollectorService::processingLoop, this);

  QCoreApplication::exec();
}

void terrama2::collector::CollectorService::assignCollector(CollectorPtr firstCollectorInQueue)
{
  try
  {
    firstCollectorInQueue->open();
  }
  catch(...)
  {
    //TODO: what to do when cant open collector connection?
  }


  assert(datasetQueue_.contains(firstCollectorInQueue));
  auto& datasetTimerQueue = datasetQueue_[firstCollectorInQueue];

  while(!datasetTimerQueue.isEmpty())
  {
    //if dataprovider is already getting some file
    if(firstCollectorInQueue->isCollecting())
      break;

    assert(!datasetTimerQueue.isEmpty());
    //first dataset on queue
    auto datasetTimer = datasetTimerLst_.value(datasetTimerQueue.front());
    assert(datasetTimer);

    //aquire dataset files
    firstCollectorInQueue->collect(datasetTimer);

    //remove first dataset from queue
    datasetTimerQueue.pop_front();
  }

}

void terrama2::collector::CollectorService::processingLoop()
{
  while(true)
  {
    if(stop_)
      return;

    // For each provider type verifies if the first provider in the queue is acquiring new data,
    // in case it's collecting moves to next type of provider, when it's done remove it from the queue
    // in case it's not collecting, starts the collection calling the collect method.
    // It allows multiples providers to collect at the same time but only one provider of each type.
    for (auto it = collectorQueueMap_.begin(); it != collectorQueueMap_.end(); ++it)
    {
      std::lock_guard<std::mutex> lock(mutex_);

      auto collectorQueueByType = it.value();
      if(collectorQueueByType.size())
      {
        assert(collectorQueueByType.size() > 0);

        auto firstCollectorInQueue = collectorQueueByType.front();
        //start collecting
        assignCollector(firstCollectorInQueue);

        //It remains in the queue until it's done collecting
        if(!firstCollectorInQueue->isCollecting())
        {
          //close dataprovider connection
          firstCollectorInQueue->close();
          collectorQueueByType.pop_front();
        }
      }
    }
  }
}

void terrama2::collector::CollectorService::addToQueueSlot(const uint64_t datasetId)
{
  //Lock Thread and add to the queue
  std::lock_guard<std::mutex> lock(mutex_);

  auto datasetTimer = datasetTimerLst_.value(datasetId);
  assert(datasetTimer);

  //Append the data provider to queue
  auto collector = datasetTimer->collector();
  auto collectorQueue = collectorQueueMap_.value(collector->kind());
  if(!collectorQueue.contains(collector))
    collectorQueue.append(collector);

  //Append the dataset to queue
  auto& datasetTimerQueue = datasetQueue_[collector];
  if(!datasetTimerQueue.contains(datasetId))
    datasetTimerQueue.append(datasetId);
}

terrama2::collector::CollectorPtr terrama2::collector::CollectorService::addProvider(const core::DataProviderPtr dataProvider)
{
  //sanity check: valid dataprovider
  assert(dataProvider->id());

  //TODO: catch? rethrow?
  //Create a collector and add it to the list
  CollectorPtr collector = CollectorFactory::getInstance().getCollector(dataProvider);

  return collector;

}

void terrama2::collector::CollectorService::removeProvider(const terrama2::core::DataProviderPtr dataProvider)
{
  for(core::DataSetPtr dataSet : dataProvider->dataSets())
    removeDataset(dataSet);

  CollectorFactory& factory = CollectorFactory::getInstance();
  factory.removeCollector(dataProvider);
}

void terrama2::collector::CollectorService::updateProvider(const terrama2::core::DataProviderPtr dataProvider)
{
  CollectorFactory& factory = CollectorFactory::getInstance();
  factory.removeCollector(dataProvider);
  factory.getCollector(dataProvider);
}

terrama2::collector::DataSetTimerPtr terrama2::collector::CollectorService::addDataset(const core::DataSetPtr dataset)
{
  //sanity check: valid dataset
  assert(dataset->id());

  //Create a new dataset timer and connect the timeout signal to queue
  auto datasetTimer = std::shared_ptr<DataSetTimer>(new DataSetTimer(dataset));
  datasetTimerLst_.insert(dataset->id(), datasetTimer);
  connect(datasetTimer.get(), &terrama2::collector::DataSetTimer::timerSignal, this, &CollectorService::addToQueueSlot, Qt::UniqueConnection);

  return datasetTimer;
}

void terrama2::collector::CollectorService::removeDataset(const terrama2::core::DataSetPtr dataset)
{
  if(datasetTimerLst_.contains(dataset->id()))
  {
    DataSetTimerPtr datasetTimer = datasetTimerLst_.value(dataset->id());
    disconnect(datasetTimer.get(), nullptr, this, nullptr);

    datasetTimerLst_.remove(dataset->id());
  }
}

void terrama2::collector::CollectorService::updateDataset(const terrama2::core::DataSetPtr dataset)
{
  removeDataset(dataset);
  addDataset(dataset);
}

