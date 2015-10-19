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
  \file terrama2/collector/Collector.cpp

  \brief Aquire data from server.

  \author Jano Simas
*/


#include "Collector.hpp"
#include "Exception.hpp"
#include "../core/DataSet.hpp"

//Boost
#include <boost/log/trivial.hpp>


terrama2::collector::Collector::Collector(const terrama2::core::DataProvider dataProvider, QObject *parent)
  : QObject(parent),
    dataProvider_(dataProvider)
{

}

terrama2::collector::Collector::~Collector()
{
  //If there is a thread running, join.
  if(collectingThread_.joinable())
    collectingThread_.join();
}

terrama2::core::DataProvider::Kind terrama2::collector::Collector::kind() const
{
  return dataProvider_.kind();
}

terrama2::core::DataProvider terrama2::collector::Collector::dataProvider() const
{
  return dataProvider_;
}

bool terrama2::collector::Collector::isCollecting() const
{
  //Test if is not locked
  if(mutex_.try_lock())
  {
    //if can lock no one is using and release lock
    mutex_.unlock();
    return false;
  }
  else
    //if cant get lock, is collecting
    return true;
}

void terrama2::collector::Collector::collectAsThread(const DataSetTimerPtr datasetTimer)
{
  //already locked by Collector::collect, lock_guard just to release when finished
  std::lock_guard<std::mutex> lock(mutex_, std::adopt_lock);
  //aquire all data
  for(auto& data : datasetTimer->data())
  {
    //TODO: conditions to collect Data?
    std::string localUri = retrieveData(data);

    data->import(localUri);
  }
}

void terrama2::collector::Collector::collect(const DataSetTimerPtr datasetTimer)
{
  if(datasetTimer->dataSet().status() != terrama2::core::DataSet::ACTIVE)
  {
    throw InactiveDataSetError() << terrama2::ErrorDescription(
                                         tr("Trying to collect an inactive dataset."));
  }

  //If can get lock creates a thread the collects the dataset
  if(!mutex_.try_lock())
  {
    throw UnabletoGetLockError() << terrama2::ErrorDescription(
                                         tr("Unable to get lock."));
  }


  //***************************************************
  //nothing wrong, prepare and collect

  //"default" thread are not joinable,
  //if there was one and we got a lock it has ended than join
  if(collectingThread_.joinable())
    collectingThread_.join();

  //JANO: Reabilitar thread na colleta
  //start a new thread
  collectingThread_ = std::thread(&Collector::collectAsThread, this, datasetTimer);
}
