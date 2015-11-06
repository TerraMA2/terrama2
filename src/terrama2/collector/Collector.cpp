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

#include "DataRetriever.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "Storager.hpp"
#include "Factory.hpp"
#include "Parser.hpp"

#include "../core/DataSet.hpp"

//Boost
#include <boost/log/trivial.hpp>

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>

terrama2::collector::Collector::Collector(const core::DataProvider &dataProvider, QObject *parent)
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
  /*
  //already locked by Collector::collect, lock_guard just to release when finished
  std::lock_guard<std::mutex> lock(mutex_, std::adopt_lock);

  uint64_t dataSetId = datasetTimer->dataSet();

  if(dataSet.dataSetItems().empty())
  {
    //TODO: LOG empty dataset
    return;
  }

  //aquire all data
  for(auto& dataSetItem : dataSet.dataSetItems())
  {

    try
    {
      ParserPtr     parser = Factory::getParser(dataSetItem);
      StoragerPtr   storager = Factory::getStorager(dataSetItem);
      DataFilterPtr filter(new DataFilter(dataSetItem));

      //TODO: conditions to collect Data?
      //retrieve remote data to local temp file
      std::string uri = retrieveData(filter);

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

//      data->import(localUri);
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
  */
}

void terrama2::collector::Collector::collectAsThreadST(const terrama2::core::DataProvider &dataProvider, const std::list<terrama2::core::DataSet> &dataSetList)
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

        //      data->import(localUri);
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

void terrama2::collector::Collector::collect(const DataSetTimerPtr datasetTimer)
{
//  if(datasetTimer->dataSet().status() != terrama2::core::DataSet::ACTIVE)
//  {
//    throw InactiveDataSetError() << terrama2::ErrorDescription(
//                                         tr("Trying to collect an inactive dataset."));
//  }

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
