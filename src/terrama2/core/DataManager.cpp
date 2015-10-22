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
  std::map<uint64_t, DataProvider> providers; //!< A map from data-provider-id to data-provider.
  std::map<uint64_t, DataSet> datasets;       //!< A map from data-set-id to dataset.
  bool dataLoaded;                               //!< A boolean that defines if the data has already been loaded.
  std::mutex mtx;                                //!< A mutex to syncronize all operations.
};

void terrama2::core::DataManager::load()
{

// Inside a block so the lock is released before emitting the signal
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

// retrieve all data providers from database
    std::vector<DataProvider> providers = DataProviderDAO::loadAll(*transactor);

// index all data providers and theirs datasets
    for(auto& provider : providers)
    {

      pimpl_->providers[provider.id()] = provider;

      const std::vector<DataSet>& datasets = provider.datasets();

      for(auto& dataset : datasets)
        pimpl_->datasets[dataset.id()] = dataset;
    }

    pimpl_->dataLoaded = true;
  }

// emits a signal in order to notify the application that the data manager has been loaded.
  emit dataManagerLoaded();
}

void terrama2::core::DataManager::unload() noexcept
{
// Inside a block so the lock is released before emitting the signal
  {

    // only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    // clears references to data providers and their data
    pimpl_->datasets.clear();
    pimpl_->providers.clear();
    pimpl_->dataLoaded = false;

  }

  // emits a signal in order to notify the application that the data manager has been unloaded.
  emit dataManagerUnloaded();
}

void terrama2::core::DataManager::add(DataProvider& provider, const bool shallowSave)
{
  // Inside a block so the lock is released before emitting the signal
  {
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(provider.id() != 0)
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not add a data provider with an identifier different than 0."));

    if(provider.name().empty())
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not add a data provider with empty name."));

    try
    {
      std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

      transactor->begin();

      DataProviderDAO::save(provider, *transactor, shallowSave);

      transactor->commit();

      pimpl_->providers[provider.id()] = provider;
    }
    catch(const terrama2::Exception&)
    {
      throw;
    }
    catch(const std::exception& e)
    {
      throw DataAccessError() << ErrorDescription(e.what());
    }
    catch(...)
    {
      throw DataAccessError() <<
            ErrorDescription(QObject::tr("Unexpected error adding a data provider and registering it."));
    }
  }

  emit dataProviderAdded(provider);
}

void terrama2::core::DataManager::add(DataSet& dataset, const bool shallowSave)
{
// Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(dataset.id() != 0)
      throw InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not add a dataset with identifier different than 0."));

    if(dataset.name().empty())
      throw terrama2::InvalidArgumentError() << ErrorDescription(QObject::tr("Can not add a dataset with empty name."));

    if(dataset.provider() == 0)
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not add a dataset with an invalid data provider."));

    auto it = pimpl_->providers.find(dataset.provider());

    if(it == pimpl_->providers.end())
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not add a dataset with a non-registered data provider."));

    std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

    try
    {
      transactor->begin();

      DataSetDAO::save(dataset, *transactor, shallowSave);

      transactor->commit();

      it->second.add(dataset);
      pimpl_->datasets[dataset.id()] = dataset;
    }
    catch(const terrama2::Exception&)
    {
      throw;
    }
    catch(const std::exception& e)
    {
      throw DataAccessError() << ErrorDescription(e.what());
    }
    catch(...)
    {
      throw DataAccessError() << ErrorDescription(QObject::tr("Unexpected error adding a dataset and registering it."));
    }
  }

  emit dataSetAdded(dataset);
}

void terrama2::core::DataManager::update(DataProvider& provider, const bool shallowSave)
{
// Inside a block so the lock is released before emitting the signal
  {

    // only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(provider.id() == 0)
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not update a data provider with identifier: 0."));

    if(provider.name().empty())
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not update a data provider with empty name."));

    try
    {
      auto it = pimpl_->providers.find(provider.id());

      if(it == pimpl_->providers.end())
      {
        throw terrama2::InvalidArgumentError() <<
              ErrorDescription(QObject::tr("Can not update a provider not registered in the data manager."));
      }

      std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

      transactor->begin();

      DataProviderDAO::update(provider, *transactor, shallowSave);

      transactor->commit();

              foreach(auto dataset, provider.datasets())
        {
          pimpl_->datasets[dataset.id()] = dataset;
        }

      pimpl_->providers[provider.id()] = provider;
    }
    catch(const terrama2::Exception&)
    {
      throw;
    }
    catch(const std::exception& e)
    {
      throw DataAccessError() << ErrorDescription(e.what());
    }
    catch(...)
    {
      throw DataAccessError() << ErrorDescription(QObject::tr("Unexpected error updating a data provider."));
    }
  }

  emit dataProviderUpdated(provider);
}

void terrama2::core::DataManager::update(DataSet& dataset, const bool shallowSave)
{
// Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(dataset.id() == 0)
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

    if(dataset.name().empty())
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not update a dataset with empty name."));

    if(dataset.provider() == 0)
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not update a dataset with an invalid data provider."));

    try
    {
      auto itDp = pimpl_->providers.find(dataset.provider());

      if(itDp == pimpl_->providers.end())
        throw terrama2::InvalidArgumentError() <<
              ErrorDescription(QObject::tr("Can not update a nonexistent data provider."));

      auto itDs = pimpl_->datasets.find(dataset.id());

      if(itDs == pimpl_->datasets.end())
        throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not update a nonexistent dataset."));

      std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

      transactor->begin();

      DataSetDAO::update(dataset, *transactor, shallowSave);

      transactor->commit();

      pimpl_->datasets[dataset.id()] = dataset;


    }
    catch(const terrama2::Exception&)
    {
      throw;
    }
    catch(const std::exception& e)
    {
      throw DataAccessError() << ErrorDescription(e.what());
    }
    catch(...)
    {
      throw DataAccessError() << ErrorDescription(QObject::tr("Unexpected error updating a dataset."));
    }
  }

  emit dataSetUpdated(dataset);
}

void terrama2::core::DataManager::removeDataProvider(const uint64_t id)
{
  DataProvider dataProvider;
// Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(id == 0)
      throw terrama2::InvalidArgumentError() <<
            ErrorDescription(QObject::tr("Can not remove a data provider with identifier: 0."));

    try
    {
      auto itDp = pimpl_->providers.find(id);

      if(itDp != pimpl_->providers.end())
      {
        dataProvider = itDp->second;

        std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

        transactor->begin();

        DataProviderDAO::remove(id, *transactor.get());

        transactor->commit();

        // removes all related datasets from the map
        for(auto dataSet: dataProvider.datasets())
        {
          auto itDs = pimpl_->datasets.find(id);
          if(itDs != pimpl_->datasets.end())
          {
            pimpl_->datasets.erase(itDs);
          }
        }

        if(itDp != pimpl_->providers.end())
        {
          pimpl_->providers.erase(itDp);
        }

      }
      else
        throw terrama2::InvalidArgumentError() <<
              ErrorDescription(QObject::tr("Can not remove a nonexistent data provider."));

    }
    catch(const terrama2::Exception&)
    {
      throw;
    }
    catch(const std::exception& e)
    {
      throw DataAccessError() << ErrorDescription(e.what());
    }
    catch(...)
    {
      throw DataAccessError() << ErrorDescription(QObject::tr("Unexpected error removing a data provider."));
    }
  }

  emit dataProviderRemoved(dataProvider);

}

void terrama2::core::DataManager::removeDataSet(const uint64_t id)
{
  DataSet dataset;
// Inside a block so the lock is released before emitting the signal
  {
// only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(id == 0)
    {
      throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not remove a dataset with identifier: 0."));
    }

    std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

    try
    {
      auto it = pimpl_->datasets.find(id);
      if(it == pimpl_->datasets.end())
      {
        throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not remove a nonexistent dataset."));
      }

      transactor->begin();

      DataSetDAO::remove(id, *transactor.get());

      transactor->commit();

// removes dataset from the map


      if(it != pimpl_->datasets.end())
      {
        dataset = it->second;
        pimpl_->datasets.erase(it);

        auto it = pimpl_->providers.find(dataset.provider());

        if(it != pimpl_->providers.end())
        {
          auto dataProvider = it->second;
          dataProvider.removeDataSet(dataset.id());
        }
        else
        {
          throw terrama2::InvalidArgumentError() <<
                ErrorDescription(QObject::tr("Can not remove a dataset with invalid data provider."));
        }
      }
    }
    catch(const terrama2::Exception&)
    {
      throw;
    }
    catch(const std::exception& e)
    {
      throw DataAccessError() << ErrorDescription(e.what());
    }
    catch(...)
    {
      throw DataAccessError() << ErrorDescription(QObject::tr("Unexpected error removing a data provider."));
    }
  }

  emit dataSetRemoved(dataset);
}

terrama2::core::DataProvider
terrama2::core::DataManager::findDataProvider(const uint64_t id) const
{
  if(id == 0)
  {
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not find a data provider with identifier: 0."));
  }

// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->providers.find(id);
  
  if(it !=  pimpl_->providers.end())
    return it->second;

  return DataProvider();
}

terrama2::core::DataProvider
terrama2::core::DataManager::findDataProvider(const std::string& name) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->providers.begin();
  
  while(it !=  pimpl_->providers.end())
  {
    if(it->second.name() == name)
      return it->second;

    ++it;
  }

  return DataProvider();
}

terrama2::core::DataSet
terrama2::core::DataManager::findDataSet(const std::string& name) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->datasets.begin();
  
  while(it !=  pimpl_->datasets.end())
  {
    if(it->second.name() == name)
      return it->second;

    ++it;
  }

  return DataSet();
}

terrama2::core::DataSet terrama2::core::DataManager::findDataSet(const uint64_t id) const
{
  if(id == 0)
  {
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not find a data provider with identifier: 0."));
  }

// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);


  auto it = pimpl_->datasets.find(id);
  
  if(it !=  pimpl_->datasets.end())
    return it->second;
    
  return DataSet();
}

std::vector<terrama2::core::DataProvider> terrama2::core::DataManager::providers() const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);
  
  std::vector<DataProvider> providers;
  
  std::transform(pimpl_->providers.begin(), pimpl_->providers.end(), std::back_inserter(providers),
                 [](const std::map<uint64_t, DataProvider>::value_type& v) { return v.second; } );

  return providers;
}

std::vector<terrama2::core::DataSet> terrama2::core::DataManager::dataSets() const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  std::vector<DataSet> datasets;
  
  std::for_each(pimpl_->datasets.begin(), pimpl_->datasets.end(),
                [&datasets](const std::map<uint64_t, DataSet>::value_type& v){ datasets.push_back(v.second); });
  
  return datasets;
}

terrama2::core::DataManager::DataManager()
  : pimpl_(new Impl)
{
  pimpl_->dataLoaded = false;
}

terrama2::core::DataManager::~DataManager()
{
  delete pimpl_;
}

bool terrama2::core::DataManager::isDatasetNameValid(const std::string& name) const
{
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->datasets.begin();

  while(it !=  pimpl_->datasets.end())
  {
    if(it->second.name() == name)
      return false;

    ++it;
  }

  return true;
}

bool terrama2::core::DataManager::isDataProviderNameValid(const std::string& name) const
{
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->providers.begin();

  while(it !=  pimpl_->providers.end())
  {
    if(it->second.name() == name)
      return false;

    ++it;
  }

  return true;
}
