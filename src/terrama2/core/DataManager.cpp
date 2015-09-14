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
#include "DataProvider.hpp"
#include "DataProviderDAO.hpp"
#include "DataSet.hpp"
#include "DataSetDAO.hpp"
#include "Exception.hpp"


// STL
#include <cstdint>
#include <mutex>


struct terrama2::core::DataManager::Impl
{
  std::map<uint64_t, DataProviderPtr> providers_; //!< A map from data-provider-id to data-provider.
  std::map<uint64_t, DataSetPtr> datasets_;       //!< A map from data-set-id to dataset.
  bool dataLoaded_;                               //!< A boolean that defines if the data has already been loaded.
  mutable std::mutex mutex_;                      //!< A mutex to syncronize all operations.
};

void terrama2::core::DataManager::load()
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  // If the data has already been loaded there is nothing to be done.
  if(pimpl_->dataLoaded_)
  {
    return;
  }

  try
  {

    // Loads all providers and fills the map
    DataProviderDAO dataProviderDAO;
    std::vector<DataProviderPtr> vecProviders = dataProviderDAO.list();

    foreach (auto provider, vecProviders)
    {
      pimpl_->providers_[provider->id()] = provider;
    }

    // Loads all datasets and fills the map
    DataSetDAO datasetDAO;
    std::vector<terrama2::core::DataSetPtr> vecDataSets = datasetDAO.list();

    foreach (auto dataSet, vecDataSets)
    {
      pimpl_->datasets_[dataSet->id()] = dataSet;
    }

    pimpl_->dataLoaded_ = true;

    // Emits a signal in order to notify the application that the data manager has been loaded.
    emit dataManagerLoaded();

  }
  catch(...)
  {
    //TODO: Execption handling
  }
}

void terrama2::core::DataManager::unload()
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  // If the data has already been loaded there is nothing to be done.
  if(pimpl_->dataLoaded_)
  {
    return;
  }

  // Clears all data
  pimpl_->providers_.clear();
  pimpl_->datasets_.clear();

  pimpl_->dataLoaded_ = false;

  // Emits a signal in order to notify the application that the data manager has been unloaded.
  emit dataManagerUnloaded();

}

void terrama2::core::DataManager::add(terrama2::core::DataProviderPtr dataProvider)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  try
  {
    DataProviderDAO dataProviderDAO;
    dataProviderDAO.save(dataProvider);
  }
  catch(...)
  {
    //TODO: Execption handling
  }
}

void terrama2::core::DataManager::add(terrama2::core::DataSetPtr dataset)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  try
  {
    DataSetDAO datasetDAO;
    datasetDAO.save(dataset);
  }
  catch(...)
  {
    //TODO: Execption handling
  }
}

void terrama2::core::DataManager::update(terrama2::core::DataProviderPtr dataProvider)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  try
  {
    DataProviderDAO dataProviderDAO;
    dataProviderDAO.update(dataProvider);
  }
  catch(...)
  {
    //TODO: Execption handling
  }
}

void terrama2::core::DataManager::update(terrama2::core::DataSetPtr dataset)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  try
  {
    DataSetDAO datasetDAO;
    datasetDAO.update(dataset);
  }
  catch(...)
  {
    //TODO: Execption handling
  }
}

void terrama2::core::DataManager::removeDataProvider(const uint64_t& id)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  try
  {
    DataProviderPtr dataProvider = pimpl_->providers_[id];

    DataProviderDAO dataProviderDAO;
    dataProviderDAO.remove(dataProvider);

    // Removes all related datasets from the map
    foreach (auto dataSet, dataProvider->dataSetList())
    {
      auto it = pimpl_->providers_.find(dataSet->id());
      pimpl_->providers_.erase(it);
    }

    // Remove the data provider from the map
    auto it = pimpl_->providers_.find(id);
    pimpl_->providers_.erase(it);
  }
  catch(const DataSetInUseException& e)
  {
    throw e;
  }
  catch(const terrama2::Exception& /*e*/)
  {
    // PAULO-TODO: throw new terrama2::Exception() << terrama2::ErrorDescription(tr("Could not remove the data provider."));
  }
  catch(const std::exception& /*e*/)
  {
    // PAULO-TODO: throw new terrama2::Exception() << terrama2::ErrorDescription(tr("Could not remove the data provider."));
  }
}

void terrama2::core::DataManager::removeDataSet(const uint64_t& id)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  try
  {
    DataSetDAO datasetDAO;
    datasetDAO.remove(id);
  }
  catch(...)
  {
    //TODO: Execption handling
  }
}

terrama2::core::DataProviderPtr terrama2::core::DataManager::findDataProvider(const uint64_t& id) const
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  terrama2::core::DataProviderPtr dataProvider;
  try
  {
    DataProviderDAO dataProviderDAO;
    dataProvider = dataProviderDAO.find(id);
  }
  catch(...)
  {
    //TODO: Execption handling
  }

  return dataProvider;
}

terrama2::core::DataSetPtr terrama2::core::DataManager::findDataSet(const uint64_t& id) const
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  terrama2::core::DataSetPtr dataset;
  try
  {
    DataSetDAO datasetDAO;
    dataset = datasetDAO.find(id);
  }
  catch(...)
  {
    //TODO: Execption handling
  }

  return dataset;
}

std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataManager::listDataProvider() const
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  std::vector<terrama2::core::DataProviderPtr> vecProviders;
  try
  {
    DataProviderDAO dataProviderDAO;
    vecProviders = dataProviderDAO.list();
  }
  catch(...)
  {
    //TODO: Execption handling
  }

  return vecProviders;
}

std::vector<terrama2::core::DataSetPtr> terrama2::core::DataManager::listDataSet() const
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  std::vector<terrama2::core::DataSetPtr> vecDataSets;
  try
  {
    DataSetDAO datasetDAO;
    vecDataSets = datasetDAO.list();
  }
  catch(...)
  {
    //TODO: Execption handling
  }

  return vecDataSets;
}

std::vector<terrama2::core::DataSetPtr> terrama2::core::DataManager::listDataSet(const uint64_t &dataProviderId) const
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  std::vector<terrama2::core::DataSetPtr> vecDataSets;
  try
  {
    DataSetDAO datasetDAO;
    vecDataSets = datasetDAO.list();
  }
  catch(...)
  {
    //TODO: Execption handling
  }

  return vecDataSets;
}
