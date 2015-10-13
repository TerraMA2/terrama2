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
#include <algorithm>
#include <mutex>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>


struct terrama2::core::DataManager::Impl
{
  std::map<uint64_t, DataProviderPtr> providers; //!< A map from data-provider-id to data-provider.
  std::map<uint64_t, DataSet*> datasets;       //!< A map from data-set-id to dataset.
  bool dataLoaded;                               //!< A boolean that defines if the data has already been loaded.
  std::mutex mtx;                                //!< A mutex to syncronize all operations.
};

void terrama2::core::DataManager::load()
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

// if the data has already been loaded there is nothing to be done.
  if(pimpl_->dataLoaded)
    return;

  assert(pimpl_->providers.empty());
  assert(pimpl_->datasets.empty());

// otherwise, we must search for and load all metadata information
  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

// retrieve all data providers and keep a reference to them
  std::vector<std::unique_ptr<DataProvider> > providers = DataProviderDAO::load(*transactor);

  foreach(const std::unique_ptr<DataProvider>& provider, providers)
  {
// index provider
    pimpl_->providers[provider->id()] = std::move(provider);

// index all datasets
    const std::vector<terrama2::core::DataSetPtr>& datasets = provider->dataSets();

    foreach (const DataSetPtr& dataset, datasets)
      pimpl_->datasets[dataset->id()] = dataset;
  }

  pimpl_->dataLoaded = true;

// emits a signal in order to notify the application that the data manager has been loaded.
  emit dataManagerLoaded();
}

void terrama2::core::DataManager::unload() noexcept
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

// clears references to data providers and their data
  pimpl_->providers.clear();
  pimpl_->datasets.clear();

  pimpl_->dataLoaded = false;

// emits a signal in order to notify the application that the data manager has been unloaded.
  emit dataManagerUnloaded();
}

void terrama2::core::DataManager::add(DataProviderPtr provider, const bool shallowSave)
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  if(provider == nullptr)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not register a NULL data provider."));

  if(provider->id() != 0)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not add the data provider with an identifier different than 0."));

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataProviderDAO::save(provider, *transactor, shallowSave);

  transactor->commit();

  pimpl_->providers[provider->id()] = provider;

  emit dataProviderAdded(provider);
}

void terrama2::core::DataManager::add(DataSetPtr dataset, const bool shallowSave)
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  if(dataset == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not register a NULL dataset."));

  if(dataset->id() != 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not add a dataset with identifier different than 0."));

  DataProviderPtr provider = dataset->dataProvider();

  if(provider == nullptr)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not add a dataset with an invalid data provider."));

  auto it = pimpl_->providers.find(provider->id());

  if(it ==  pimpl_->providers.end())
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not add a dataset with a non-existent data provider."));

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataSetDAO::save(dataset, *transactor, shallowSave);

  transactor->commit();

  pimpl_->datasets[dataset->id()] = dataset;

  emit dataSetAdded(dataset);
  emit dataProviderUpdated(provider);
}

void terrama2::core::DataManager::update(DataProviderPtr provider, const bool shallowSave)
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  if(provider == nullptr)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a NULL data provider."));

  if(provider->id() == 0)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a data provider with identifier: 0."));

  auto it = pimpl_->providers.find(provider->id());
  
  if(it ==  pimpl_->providers.end())
  {
    throw terrama2::InvalidParameterError() <<
          ErrorDescription(QObject::tr("Can not update a provider not registered in the data manager."));
  }

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataProviderDAO::update(provider, *transactor, shallowSave);

  transactor->commit();

  foreach(auto dataset, provider->dataSets())
  {
    pimpl_->datasets[dataset->id()] = dataset;
  }

  pimpl_->providers[provider->id()] = provider;

  emit dataProviderUpdated(provider);
}

void terrama2::core::DataManager::update(DataSetPtr dataset, const bool shallowSave)
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  if(dataset == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a NULL dataset."));

  if(dataset->id() == 0)
  {
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));
  }

  if(dataset->dataProvider() == nullptr)
  {
    throw terrama2::InvalidParameterError() <<
          ErrorDescription(QObject::tr("Can not add a dataset with an invalid data provider."));
  }

  auto itDp = pimpl_->providers.find(dataset->dataProvider()->id());

  if(itDp == pimpl_->providers.end())
  {
    throw terrama2::InvalidParameterError() <<
          ErrorDescription(QObject::tr("Can not update a nonexistent data provider."));
  }

  auto itDs = pimpl_->datasets.find(dataset->id());
  
  if(itDs ==  pimpl_->datasets.end())
  {
    throw InvalidParameterError() <<
          ErrorDescription(QObject::tr("Can not update a nonexistent dataset."));
  }

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataSetDAO::update(dataset, *transactor, shallowSave);

  transactor->commit();

  pimpl_->datasets[dataset->id()] = dataset;

  emit dataSetUpdated(dataset);
}

void terrama2::core::DataManager::removeDataProvider(const uint64_t id)
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  if(id == 0)
  {
    throw terrama2::InvalidParameterError() <<
          ErrorDescription(QObject::tr("Can not remove a data provider with identifier: 0."));
  }

  DataProviderPtr dataProvider;

  auto itDp = pimpl_->providers.find(id);
  if(itDp !=  pimpl_->providers.end())
  {
    dataProvider = itDp->second;
  }

  if(!dataProvider.get())
  {
    throw terrama2::InvalidParameterError() <<
          ErrorDescription(QObject::tr("Can not remove a nonexistent data provider."));
  }

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

  transactor->begin();

  DataProviderDAO::remove(id, *transactor.get());

  transactor->commit();

  // Removes all related datasets from the map
  foreach (auto dataSet, dataProvider->dataSets())
  {
    auto itDs = pimpl_->datasets.find(id);
    if(itDs !=  pimpl_->datasets.end())
    {
      pimpl_->datasets.erase(itDs);
    }

    emit dataSetRemoved(dataSet);
  }

  if(itDp != pimpl_->providers.end())
  {
    pimpl_->providers.erase(itDp);
  }

  emit dataProviderRemoved(dataProvider);

}

void terrama2::core::DataManager::removeDataSet(const uint64_t id)
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  if(id == 0)
  {
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not remove a dataset with identifier: 0."));
  }

  std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();


  auto it = pimpl_->datasets.find(id);
  if(it ==  pimpl_->datasets.end())
  {
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not remove a nonexistent dataset."));
  }

  transactor->begin();

  DataSetDAO::remove(id, *transactor.get());

  transactor->commit();

// removes dataset from the map

  DataSetPtr dataSet;
  if(it !=  pimpl_->datasets.end())
  {
    dataSet = it->second;
    pimpl_->datasets.erase(it);

    auto dataProvider = dataSet->dataProvider();
    auto dataSets = dataProvider->dataSets();
    std::remove(dataSets.begin(), dataSets.end(), dataSet);
    dataProvider->setDataSets(dataSets);

    emit dataProviderUpdated(dataProvider);

  }

  emit dataSetRemoved(dataSet);
}

terrama2::core::DataProviderPtr
terrama2::core::DataManager::findDataProvider(const uint64_t id) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->providers.find(id);
  
  if(it !=  pimpl_->providers.end())
    return it->second;

  return nullptr;
}

terrama2::core::DataProviderPtr
terrama2::core::DataManager::findDataProvider(const std::string& name) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->providers.begin();
  
  while(it !=  pimpl_->providers.end())
  {
    if(it->second->name() == name)
      return it->second;

    ++it;
  }

  return nullptr;
}

terrama2::core::DataSetPtr
terrama2::core::DataManager::findDataSet(const std::string& name) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->datasets.begin();
  
  while(it !=  pimpl_->datasets.end())
  {
    if(it->second->name() == name)
      return it->second;

    ++it;
  }

  return nullptr;
}

terrama2::core::DataSetPtr terrama2::core::DataManager::findDataSet(const uint64_t id) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->datasets.find(id);
  
  if(it !=  pimpl_->datasets.end())
    return it->second;
    
  return nullptr;
}

std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataManager::providers() const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);
  
  std::vector<DataProviderPtr> providers;
  
  std::transform(pimpl_->providers.begin(), pimpl_->providers.end(), std::back_inserter(providers),
                 [](const std::map<uint64_t, DataProviderPtr>::value_type& v) { return v.second; } );

  return providers;
}

std::vector<terrama2::core::DataSetPtr> terrama2::core::DataManager::dataSets() const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  std::vector<DataSetPtr> datasets;
  
  std::for_each(pimpl_->datasets.begin(), pimpl_->datasets.end(),
                [&datasets](const std::map<uint64_t, DataSetPtr>::value_type& v){ datasets.push_back(v.second); });
  
  return datasets;
}

terrama2::core::DataManager::DataManager()
  : pimpl_(new Impl)
{

}

terrama2::core::DataManager::~DataManager()
{
  delete pimpl_;
}
