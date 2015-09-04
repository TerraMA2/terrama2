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
  \file terrama2/ws/collector/core/DataProvider.cpp

  \brief DataProvider...

  \author Paulo R. M. Oliveira
*/

#include "DataProvider.hpp"
#include "Dataset.hpp"
#include "Data.hpp"

terrama2::ws::collector::core::DataProvider::DataProvider():
  dataset_(0)
{

}


terrama2::ws::collector::core::DataProvider::~DataProvider()
{
  close();
}


bool terrama2::ws::collector::core::DataProvider::isOpen() const
{
  return isOpen_;
}


void terrama2::ws::collector::core::DataProvider::collect(terrama2::ws::collector::core::Dataset* dataset)
{
  if(mutex_.tryLock())
  {
    dataset_ = dataset;
    start();
  }
}

void terrama2::ws::collector::core::DataProvider::run()
{

  auto dataLst = dataset_->getDataList();
  for(auto &data : dataLst)
  {
    //temporary store file
    std::string uri = getData(data->getMask());

    //let data store in the proper place
    data->store(uri);
  }

  mutex_.unlock();
}

std::string terrama2::ws::collector::core::DataProvider::getData(const std::string& dataMask)
{
  //TODO: arquire the data and store in a temporary place
  // for storage
  return "/data/uri";
}

bool terrama2::ws::collector::core::DataProvider::isAquiring()
{
  //test if is locked
  bool canLock = mutex_.tryLock();

  //if trylock did lock, unlock
  if(canLock)
    mutex_.unlock();

  return !canLock;
};

bool terrama2::ws::collector::core::DataProvider::open()
{
  isOpen_ = true;
  return isOpen_;
}


bool terrama2::ws::collector::core::DataProvider::close()
{
  isOpen_ = false;
  return isOpen_;
}
