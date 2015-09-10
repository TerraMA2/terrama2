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
  \file terrama2/ws/collector/server/CollectorService.cpp

  \brief  Manages the collection of data in the appropriate time.

  \author Jano Simas, Paulo R. M. Oliveira
*/

#include "CollectorService.hpp"
#include "CollectorFactory.hpp"

#include "../../../core/DataSet.hpp"
#include "../../../core/DataProvider.hpp"
#include "../../../core/DataManager.hpp"

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


terrama2::ws::collector::server::CollectorService::CollectorService(QObject *parent)
  : QObject(parent),
    stop_(false)
{
}

void terrama2::ws::collector::server::CollectorService::assignCollector(CollectorPtr firstCollectorInQueue)
{
  if(firstCollectorInQueue->open())
  {
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
}

void terrama2::ws::collector::server::CollectorService::start()
{
  while(true)
  {
    if(stop_)
      break;

    // For each provider type verifies if the first provider in the queue is acquiring new data,
    // in case it's collecting moves to next type of provider, when it's done remove it from the queue
    // in case it's not collecting, starts the collection calling the collect method.
    // It allows multiples providers to collect at the same time but only one provider of each type.
    for (auto it = collectorQueueMap_.begin(); it != collectorQueueMap_.end(); ++it)
    {
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

    QApplication::processEvents();
  }
}

void terrama2::ws::collector::server::CollectorService::stop()
{
  stop_ = true;
}

void terrama2::ws::collector::server::CollectorService::addToQueueSlot(const uint64_t datasetId)
{
  auto datasetTimer = datasetTimerLst_.value(datasetId);
  assert(datasetTimer);

  //Append the data provider to queue
  auto collector = datasetTimer->getCollector();
  auto collectorQueue = collectorQueueMap_.value(collector->kind());
  if(!collectorQueue.contains(collector))
    collectorQueue.append(collector);

  //Append the dataset to queue
  auto& datasetTimerQueue = datasetQueue_[collector];
  if(!datasetTimerQueue.contains(datasetId))
    datasetTimerQueue.append(datasetId);
}

terrama2::ws::collector::server::CollectorPtr terrama2::ws::collector::server::CollectorService::addProvider(const core::DataProviderPtr dataProvider)
{
  //Create a collector and add it to the list
  auto collector = CollectorFactory::instance().getCollector(dataProvider);

  return collector;
}

terrama2::ws::collector::server::DataSetTimerPtr terrama2::ws::collector::server::CollectorService::addDataset(const core::DataSetPtr dataset)
{
  //Create a new dataset timer and connect the timeout signal to queue
  auto datasetTimer = std::shared_ptr<DataSetTimer>(new DataSetTimer(dataset));
  datasetTimerLst_.insert(dataset->id(), datasetTimer);
  connect(datasetTimer.get(), &terrama2::ws::collector::server::DataSetTimer::timerSignal, this, &CollectorService::addToQueueSlot, Qt::UniqueConnection);

  return datasetTimer;
}


