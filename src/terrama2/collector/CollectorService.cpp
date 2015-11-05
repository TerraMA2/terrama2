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
#include "Exception.hpp"
#include "Factory.hpp"
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
    std::lock_guard<std::mutex> l(mutex_);
    
    dataproviders_.insert(std::make_pair(provider.id(), provider));
  }
  catch(const std::exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
  }
}

void terrama2::collector::CollectorService::assignCollector(CollectorPtr collector)
{
  assert(datasetQueue_.contains(collector));
  auto& datasetTimerQueue = datasetQueue_[collector];

  while(!datasetTimerQueue.isEmpty())
  {
    //if dataprovider is already getting some file
    if(collector->isCollecting())
      break;

    assert(!datasetTimerQueue.isEmpty());
    //first dataset on queue
    auto datasetTimer = datasetTimerLst_.value(datasetTimerQueue.front());
    assert(datasetTimer);

    try
    {
      //aquire dataset files
      collector->collect(datasetTimer);
    }
    catch(terrama2::Exception& e)
    {
      //TODO: log de erro
      qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    }
    catch(...)
    {
      //TODO: log de erro: Erro desconhecido ao coletar DataSet: datasetTimer
      qDebug() << "Erro desconhecido ao coletar DataSet: datasetTimer";
    }

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

    std::lock_guard<std::mutex> lock(mutex_);

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

        try
        {
          firstCollectorInQueue->open();
          //start collecting
          assignCollector(firstCollectorInQueue);
        }
        catch(terrama2::collector::UnableToOpenCollectorError& e)
        {
          //TODO: log de erro
          qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();

          firstCollectorInQueue->close();
          collectorQueueByType.pop_front();
          continue;
        }
        catch(...)
        {
          //TODO: log de erro
          qDebug() << "Erro desconhecido ao iniciar ao acessar o provedor: firstCollectorInQueue";
        }

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

void terrama2::collector::CollectorService::addToQueueSlot(const uint64_t datasetId)
{
  try
  {
    //Lock Thread and add to the queue
    std::lock_guard<std::mutex> lock(mutex_);

    auto datasetTimer = datasetTimerLst_.value(datasetId);
    assert(datasetTimer);

    //Append the data provider to queue
    uint64_t dataProvider = datasetTimer->dataProvider();
    auto collector    = factory_->getCollector(dataProvider);

    assert(collector);
    auto& collectorQueue = collectorQueueMap_[collector->kind()];
    if(!collectorQueue.contains(collector))
      collectorQueue.append(collector);

    //Append the dataset to queue
    auto& datasetTimerQueue = datasetQueue_[collector];
    if(!datasetTimerQueue.contains(datasetId))
      datasetTimerQueue.append(datasetId);
  }
  catch(InvalidArgumentError& e)
  {
    //invalid dataProvider

    //TODO: log this

    removeDatasetById(datasetId);
  }
  catch(terrama2::Exception& e)
  {
    //TODO: log de erro
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    assert(0);
  }
  catch(te::common::Exception& e)
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

}



void terrama2::collector::CollectorService::removeProvider(terrama2::core::DataProvider dataProvider)
{
  for(const core::DataSet dataSet : dataProvider.datasets())
    removeDataset(dataSet);

  factory_->removeCollector(dataProvider.id());
}

void terrama2::collector::CollectorService::updateProvider(const terrama2::core::DataProvider dataProvider)
{
  try
  {
    factory_->removeCollector(dataProvider.id());
    factory_->getCollector(dataProvider.id());
  }
  catch(terrama2::Exception& e)
  {
    //TODO: log de erro
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    assert(0);
  }
  catch(te::common::Exception& e)
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
}

terrama2::collector::DataSetTimerPtr
terrama2::collector::CollectorService::addDataset(const core::DataSet dataset)
{
  //TODO: Debug?
  //sanity check: valid dataset
  //  assert(dataset.id());

  try
  {
    //Create a new dataset timer and connect the timeout signal to queue
    auto datasetTimer = std::shared_ptr<DataSetTimer>(new DataSetTimer(dataset));
    datasetTimerLst_.insert(dataset.id(), datasetTimer);
    connect(datasetTimer.get(), &terrama2::collector::DataSetTimer::timerSignal, this, &CollectorService::addToQueueSlot, Qt::UniqueConnection);

    return datasetTimer;
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

  return DataSetTimerPtr();
}

void terrama2::collector::CollectorService::removeDataset(const terrama2::core::DataSet dataset)
{
  removeDatasetById(dataset.id());
}

void terrama2::collector::CollectorService::removeDatasetById(uint64_t datasetId)
{
  if(datasetTimerLst_.contains(datasetId))
  {
    DataSetTimerPtr datasetTimer = datasetTimerLst_.value(datasetId);
    disconnect(datasetTimer.get(), nullptr, this, nullptr);

    datasetTimerLst_.remove(datasetId);
  }
}

void terrama2::collector::CollectorService::updateDataset(const terrama2::core::DataSet dataset)
{
  removeDataset(dataset);
  addDataset(dataset);
}

