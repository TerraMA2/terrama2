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

// TerraMA2
#include "DataManager.hpp"
#include "ApplicationController.hpp"
#include "DataProvider.hpp"
#include "DataProviderDAO.hpp"
#include "DataSet.hpp"
#include "DataSetDAO.hpp"
#include "Exception.hpp"

// STL
#include <mutex>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>


struct terrama2::core::DataManager::Impl
{
  std::map<uint64_t, DataProviderPtr> providers_; //!< A map from data-provider-id to data-provider.
  std::map<uint64_t, DataSetPtr> datasets_;       //!< A map from data-set-id to dataset.
  bool dataLoaded_;                               //!< A boolean that defines if the data has already been loaded.
  std::mutex mutex_;                              //!< A mutex to syncronize all operations.
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

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  // Loads all providers and fills the map
  std::vector<DataProviderPtr> vecProviders = DataProviderDAO::list(*transactor.get());

  foreach (auto provider, vecProviders)
  {
    pimpl_->providers_[provider->id()] = provider;
  }

  // Loads all datasets and fills the map
  std::vector<terrama2::core::DataSetPtr> vecDataSets = DataSetDAO::list(*transactor.get());

  foreach (auto dataSet, vecDataSets)
  {
    pimpl_->datasets_[dataSet->id()] = dataSet;
  }

  pimpl_->dataLoaded_ = true;

  // Emits a signal in order to notify the application that the data manager has been loaded.
  emit dataManagerLoaded();
}

void terrama2::core::DataManager::unload()
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

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

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataProviderDAO::save(dataProvider, *transactor.get());

  transactor->commit();

  pimpl_->providers_[dataProvider->id()] = dataProvider;

  emit dataProviderAdded(dataProvider);

}

void terrama2::core::DataManager::add(terrama2::core::DataSetPtr dataset)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataSetDAO::save(dataset, *transactor.get());

  transactor->commit();

  pimpl_->datasets_[dataset->id()] = dataset;

  emit dataSetAdded(dataset);
}

void terrama2::core::DataManager::update(terrama2::core::DataProviderPtr dataProvider)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataProviderDAO::update(dataProvider, *transactor.get());

  transactor->commit();

  foreach (auto dataSet, dataProvider->dataSets())
  {
    pimpl_->datasets_[dataSet->id()] = dataSet;
  }

  pimpl_->providers_[dataProvider->id()] = dataProvider;

  emit dataProviderUpdated(dataProvider);
}

void terrama2::core::DataManager::update(terrama2::core::DataSetPtr dataset)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataSetDAO::update(dataset, *transactor.get());

  transactor->commit();

  pimpl_->datasets_[dataset->id()] = dataset;

  emit dataSetUpdated(dataset);
}

void terrama2::core::DataManager::removeDataProvider(const uint64_t& id)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  if(id == 0)
    throw InvalidDataProviderIdError() << ErrorDescription(QObject::tr("Can not remove a data provider with identifier: 0."));

  DataProviderPtr dataProvider = pimpl_->providers_[id];

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataProviderDAO::remove(dataProvider, *transactor.get());

  transactor->commit();

  // Removes all related datasets from the map
  foreach (auto dataSet, dataProvider->dataSets())
  {
    auto it = pimpl_->datasets_.find(id);
    if(it !=  pimpl_->datasets_.end())
    {
      pimpl_->datasets_.erase(it);
    }
  }

  // Remove the data provider from the map
  auto it = pimpl_->providers_.find(id);
  if(it != pimpl_->providers_.end())
  {
    pimpl_->providers_.erase(it);
  }

  emit dataProviderRemoved(dataProvider);

}

void terrama2::core::DataManager::removeDataSet(const uint64_t& id)
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  if(id == 0)
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataSetDAO::remove(id, *transactor.get());

  transactor->commit();

  // Removes dataset from the map
  DataSetPtr dataSet;
  auto it = pimpl_->datasets_.find(id);
  if(it !=  pimpl_->datasets_.end())
  {
    dataSet = it->second;
    pimpl_->datasets_.erase(it);
  }

  emit dataSetRemoved(dataSet);
}

terrama2::core::DataProviderPtr terrama2::core::DataManager::findDataProvider(const uint64_t& id) const
{
  DataProviderPtr dataProvider;

  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  auto it = pimpl_->providers_.find(id);
  if(it !=  pimpl_->providers_.end())
  {
    dataProvider = it->second;
  }

  return dataProvider;
}

terrama2::core::DataSetPtr terrama2::core::DataManager::findDataSet(const uint64_t& id) const
{
  DataSetPtr dataset;

  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  auto it = pimpl_->datasets_.find(id);
  if(it !=  pimpl_->datasets_.end())
  {
    dataset = it->second;
  }

  return dataset;
}

std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataManager::providers() const
{
  std::vector<terrama2::core::DataProviderPtr> vecProviders;

  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  for (auto it = pimpl_->providers_.begin(); it != pimpl_->providers_.end(); ++it)
  {
    vecProviders.push_back(it->second);
  }

  return vecProviders;
}

std::vector<terrama2::core::DataSetPtr> terrama2::core::DataManager::dataSets() const
{
  // Only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  std::vector<terrama2::core::DataSetPtr> vecDataSets;

  for (auto it = pimpl_->datasets_.begin(); it != pimpl_->datasets_.end(); ++it)
  {
    vecDataSets.push_back(it->second);
  }

  return vecDataSets;
}

terrama2::core::DataManager::DataManager()
{
  pimpl_ = new Impl();
}

terrama2::core::DataManager::~DataManager()
{
  delete pimpl_;
  pimpl_ = nullptr;
}
