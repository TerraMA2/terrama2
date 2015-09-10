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
  \file terrama2/core/DataManager.hpp

  \brief Manages metadata about data providers and its datasets.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#include "DataManager.hpp"
#include "ApplicationController.hpp"
#include "DataProvider.hpp"
#include "DataProviderDAO.hpp"
#include "DataSet.hpp"
#include "DataSetDAO.hpp"

// STL
#include <cstdint>


terrama2::core::DataManager::DataManager()
{

}

terrama2::core::DataManager::~DataManager()
{

}

terrama2::core::DataManager::DataManager(const terrama2::core::DataManager & rhs)
{
  //PAULO-TODO: implement
}

terrama2::core::DataManager& terrama2::core::DataManager::operator=(const terrama2::core::DataManager & rhs)
{
  //PAULO-TODO: implement
}

void terrama2::core::DataManager::load()
{
  //PAULO-TODO: implement
}

void terrama2::core::DataManager::unload()
{
  //PAULO-TODO: implement
}

void terrama2::core::DataManager::add(terrama2::core::DataProviderPtr dataProvider)
{
  try
  {
    DataProviderDAO dataProviderDAO(ApplicationController::getInstance().getDataSource());
    dataProviderDAO.save(dataProvider);
  }
  catch(...)
  {
    //TODO: expection handling
  }
}

void terrama2::core::DataManager::add(terrama2::core::DataSetPtr dataset)
{

  try
  {
    DataSetDAO datasetDAO(ApplicationController::getInstance().getDataSource());
    datasetDAO.save(dataset);
  }
  catch(...)
  {
    //TODO: expection handling
  }
}

void terrama2::core::DataManager::update(terrama2::core::DataProviderPtr dataProvider)
{
  try
  {
    DataProviderDAO dataProviderDAO(ApplicationController::getInstance().getDataSource());
    dataProviderDAO.update(dataProvider);
  }
  catch(...)
  {
    //TODO: expection handling
  }
}

void terrama2::core::DataManager::update(terrama2::core::DataSetPtr dataset)
{
  try
  {
    DataSetDAO datasetDAO(ApplicationController::getInstance().getDataSource());
    datasetDAO.update(dataset);
  }
  catch(...)
  {
    //TODO: expection handling
  }
}

void terrama2::core::DataManager::removeDataProvider(const uint64_t& id)
{
  try
  {
    DataProviderDAO dataProviderDAO(ApplicationController::getInstance().getDataSource());
    dataProviderDAO.remove(id);
  }
  catch(...)
  {
    //TODO: expection handling
  }
}

void terrama2::core::DataManager::removeDataSet(const uint64_t& id)
{
  try
  {
    DataSetDAO datasetDAO(ApplicationController::getInstance().getDataSource());
    datasetDAO.remove(id);
  }
  catch(...)
  {
    //TODO: expection handling
  }
}

terrama2::core::DataProviderPtr terrama2::core::DataManager::findDataProvider(const uint64_t& id) const
{
  terrama2::core::DataProviderPtr dataProvider;
  try
  {
    DataProviderDAO dataProviderDAO(ApplicationController::getInstance().getDataSource());
    dataProvider = dataProviderDAO.find(id);
  }
  catch(...)
  {
    //TODO: expection handling
  }

  return dataProvider;
}

terrama2::core::DataSetPtr terrama2::core::DataManager::findDataSet(const uint64_t& id) const
{
  terrama2::core::DataSetPtr dataset;
  try
  {
    DataSetDAO datasetDAO(ApplicationController::getInstance().getDataSource());
    dataset = datasetDAO.find(id);
  }
  catch(...)
  {
    //TODO: expection handling
  }

  return dataset;
}


std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataManager::listDataProvider() const
{
  std::vector<terrama2::core::DataProviderPtr> vecProviders;
  try
  {
    DataProviderDAO dataProviderDAO(ApplicationController::getInstance().getDataSource());
    vecProviders = dataProviderDAO.list();
  }
  catch(...)
  {
    //TODO: expection handling
  }

  return vecProviders;
}

std::vector<terrama2::core::DataSetPtr> terrama2::core::DataManager::listDataSet() const
{
  std::vector<terrama2::core::DataSetPtr> vecDataSets;
  try
  {
    DataSetDAO datasetDAO(ApplicationController::getInstance().getDataSource());
    vecDataSets = datasetDAO.list();
  }
  catch(...)
  {
    //TODO: expection handling
  }

  return vecDataSets;
}

terrama2::core::DataManager &terrama2::core::DataManager::instance()
{
  static DataManager dataManagerInstance;  // The singleton instance.

  return dataManagerInstance;
}
