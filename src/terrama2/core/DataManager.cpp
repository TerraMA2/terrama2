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
#include "DataSet.hpp"
#include "dao/DataProviderDAO.hpp"
#include "dao/DataSetDAO.hpp"
#include "Exception.hpp"

// TerraMA2 Logger
#include "Logger.hpp"

// STL
#include <algorithm>
#include <mutex>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>


struct terrama2::core::DataManager::Impl
{
  std::map<uint64_t, DataProvider> providers; //!< A map from data-provider-id to data-provider.
  std::map<uint64_t, DataSet> datasets;       //!< A map from data-set-id to dataset.
  bool dataLoaded;                            //!< A boolean that defines if the data has already been loaded.
  std::mutex mtx;                             //!< A mutex to syncronize all operations.
  bool memory;                                //!< Defines if the DataManager will store data only in memory.
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

    pimpl_->memory = false;

    assert(pimpl_->providers.empty());
    assert(pimpl_->datasets.empty());

// otherwise, we must search for and load all metadata information
    std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

// retrieve all data providers from database
    std::vector<DataProvider> providers = dao::DataProviderDAO::loadAll(*transactor);

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


void terrama2::core::DataManager::load(std::vector<DataProvider> providers)
{

// Inside a block so the lock is released before emitting the signal
  {

// only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

// if the data has already been loaded there is nothing to be done.
    if(pimpl_->dataLoaded)
      return;

    pimpl_->memory = true;

    assert(pimpl_->providers.empty());
    assert(pimpl_->datasets.empty());

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

    if(!pimpl_->memory)
    {
      if(provider.id() != 0)
        throw terrama2::InvalidArgumentException() <<
              ErrorDescription(QObject::tr("Can not add a data provider with an identifier different than 0."));
    }

    if(provider.name().empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a data provider with empty name."));

    try
    {
      std::auto_ptr<te::da::DataSourceTransactor> transactor;

      if(!pimpl_->memory)
      {
        transactor = ApplicationController::getInstance().getTransactor();

        transactor->begin();

        dao::DataProviderDAO::save(provider, *transactor, shallowSave);

        transactor->commit();
      }


      if(!shallowSave)
      {
        for(auto& dataset: provider.datasets())
        {
          if(!pimpl_->memory)
            dao::DataSetDAO::save(dataset, *transactor, shallowSave);

          pimpl_->datasets[dataset.id()] = dataset;
        }
      }

      pimpl_->providers[provider.id()] = provider;
    }
    catch(const terrama2::Exception& e)
    {
      if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
        TERRAMA2_LOG_ERROR() << message->toStdString();
      throw;
    }
    catch(const std::exception& e)
    {
      const char* message = e.what();
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
    catch(...)
    {
      QString message = QObject::tr("Unexpected error adding a data provider and registering it.");
      throw DataAccessException() <<
            ErrorDescription(message);
    }
  }

  if(!shallowSave)
  {
    for (auto& dataset : provider.datasets())
    {
      emit dataSetAdded(dataset);
    }
  }

  emit dataProviderAdded(provider);
}

void terrama2::core::DataManager::add(DataSet& dataset, const bool shallowSave)
{

  DataProvider provider;

// Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(!pimpl_->memory)
    {
      if(dataset.id() != 0)
        throw InvalidArgumentException() <<
              ErrorDescription(QObject::tr("Can not add a dataset with identifier different than 0."));
    }

    if(dataset.name().empty())
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Can not add a dataset with empty name."));

    if(dataset.provider() == 0)
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a dataset with an invalid data provider."));

    auto it = pimpl_->providers.find(dataset.provider());

    if(it == pimpl_->providers.end())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a dataset with a non-registered data provider."));

    provider = it->second;

    try
    {
      if(!pimpl_->memory)
      {
        std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();
        transactor->begin();

        dao::DataSetDAO::save(dataset, *transactor, shallowSave);

        transactor->commit();
      }

      it->second.add(dataset);
      pimpl_->datasets[dataset.id()] = dataset;
    }
    catch(const terrama2::Exception& e)
    {
      if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
        TERRAMA2_LOG_ERROR() << message->toStdString();
      throw;
    }
    catch(const std::exception& e)
    {
      const char* message = e.what();
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
    catch(...)
    {
      QString message = QObject::tr("Unexpected error adding a dataset and registering it.");
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
  }

  emit dataProviderUpdated(provider);
  emit dataSetAdded(dataset);
}

void terrama2::core::DataManager::update(DataProvider& provider, const bool shallowSave)
{

  std::vector<DataSet> added, updated;
  std::vector<uint64_t> removed;

// Inside a block so the lock is released before emitting the signal
  {

    // only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(provider.id() == 0)
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a data provider with identifier: 0."));

    if(provider.name().empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a data provider with empty name."));

    try
    {
      auto it = pimpl_->providers.find(provider.id());

      if(it == pimpl_->providers.end())
      {
        throw terrama2::InvalidArgumentException() <<
              ErrorDescription(QObject::tr("Can not update a provider not registered in the data manager."));
      }

      auto transactor = ApplicationController::getInstance().getTransactor();
      if(!pimpl_->memory)
      {
        transactor->begin();

        dao::DataProviderDAO::update(provider, *transactor, shallowSave);
      }

      if(!shallowSave)
      {
        std::vector<uint64_t> ids = dao::DataProviderDAO::getDatasetsIds(provider.id(), *transactor);

        for(auto& dataset: provider.datasets())
        {
          // Id exists just need to call update
          auto it = find (ids.begin(), ids.end(), dataset.id());
          if (it != ids.end())
          {
            // Remove from the list, so what is left in this vector are the datasets to remove
            ids.erase(it);

            if(!pimpl_->memory)
              dao::DataSetDAO::update(dataset, *transactor, shallowSave);

            pimpl_->datasets[dataset.id()] = dataset;
            updated.push_back(dataset);
          }

          // Id is 0 for new items
          if(dataset.id() == 0)
          {
            if(!pimpl_->memory)
              dao::DataSetDAO::save(dataset, *transactor, shallowSave);
            added.push_back(dataset);
          }
        }

        // What is left in vector are the the removed datasets.
        for(auto datasetId : ids)
        {
          if(!pimpl_->memory)
            dao::DataSetDAO::remove(datasetId, *transactor);
          removed.push_back(datasetId);

          // removes dataset from the map

          auto it = pimpl_->datasets.find(datasetId);
          if(it == pimpl_->datasets.end())
          {
            throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove a nonexistent dataset."));
          }

          if(it != pimpl_->datasets.end())
          {
            pimpl_->datasets.erase(it);
          }
        }
      }

      if(!pimpl_->memory)
        transactor->commit();

      pimpl_->providers[provider.id()] = provider;
    }
    catch(const terrama2::Exception& e)
    {
      if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
        TERRAMA2_LOG_ERROR() << message->toStdString();
      throw;
    }
    catch(const std::exception& e)
    {
      const char* message = e.what();
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
    catch(...)
    {
      QString message = QObject::tr("Unexpected error updating a data provider.");
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
  }

  if(!shallowSave)
  {
    for(auto& dataset: added)
    {
      dataSetAdded(dataset);
    }

    for(auto& dataset: updated)
    {
      dataSetUpdated(dataset);
    }

    for(auto& id: removed)
    {
      dataSetRemoved(id);
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
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

    if(dataset.name().empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a dataset with empty name."));

    if(dataset.provider() == 0)
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a dataset with an invalid data provider."));

    try
    {
      auto itDp = pimpl_->providers.find(dataset.provider());

      if(itDp == pimpl_->providers.end())
        throw terrama2::InvalidArgumentException() <<
              ErrorDescription(QObject::tr("Can not update a nonexistent data provider."));

      auto itDs = pimpl_->datasets.find(dataset.id());

      if(itDs == pimpl_->datasets.end())
        throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not update a nonexistent dataset."));

      if(!pimpl_->memory)
      {
        std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

        transactor->begin();

        dao::DataSetDAO::update(dataset, *transactor, shallowSave);

        transactor->commit();
      }

      pimpl_->datasets[dataset.id()] = dataset;


    }
    catch(const terrama2::Exception& e)
    {
      if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
        TERRAMA2_LOG_ERROR() << message->toStdString();
      throw;
    }
    catch(const std::exception& e)
    {
      const char* message = e.what();
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
    catch(...)
    {
      QString message = QObject::tr("Unexpected error updating a dataset.");
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
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
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not remove a data provider with identifier: 0."));

    try
    {
      auto itDp = pimpl_->providers.find(id);

      if(itDp != pimpl_->providers.end())
      {
        dataProvider = itDp->second;

        if(!pimpl_->memory)
        {
          std::auto_ptr<te::da::DataSourceTransactor> transactor = ApplicationController::getInstance().getTransactor();

          transactor->begin();

          dao::DataProviderDAO::remove(id, *transactor.get());

          transactor->commit();
        }

        // removes all related datasets from the map
        for(auto dataSet: dataProvider.datasets())
        {
          auto itDs = pimpl_->datasets.find(dataSet.id());
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
        throw terrama2::InvalidArgumentException() <<
              ErrorDescription(QObject::tr("Can not remove a nonexistent data provider."));

    }
    catch(const terrama2::Exception& e)
    {
      if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
        TERRAMA2_LOG_ERROR() << message->toStdString();
      throw;
    }
    catch(const std::exception& e)
    {
      const char* message = e.what();
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(e.what());
    }
    catch(...)
    {
      QString message = QObject::tr("Unexpected error removing a data provider.");
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
  }

  emit dataProviderRemoved(dataProvider);

}

void terrama2::core::DataManager::removeDataSet(const uint64_t id)
{
  DataSet dataset;
  DataProvider provider;

// Inside a block so the lock is released before emitting the signal
  {
// only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(id == 0)
    {
      throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove a dataset with identifier: 0."));
    }


    try
    {
      auto it = pimpl_->datasets.find(id);
      if(it == pimpl_->datasets.end())
      {
        throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove a nonexistent dataset."));
      }

      if(!pimpl_->memory)
      {
        auto transactor = ApplicationController::getInstance().getTransactor();
        transactor->begin();

        dao::DataSetDAO::remove(id, *transactor.get());

        transactor->commit();
      }

// removes dataset from the map


      if(it != pimpl_->datasets.end())
      {
        dataset = it->second;
        pimpl_->datasets.erase(it);

        auto it = pimpl_->providers.find(dataset.provider());

        if(it != pimpl_->providers.end())
        {
          provider = it->second;
          provider.removeDataSet(dataset.id());
        }
        else
        {
          throw terrama2::InvalidArgumentException() <<
                ErrorDescription(QObject::tr("Can not remove a dataset with invalid data provider."));
        }
      }
    }
    catch(const terrama2::Exception& e)
    {
      if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
        TERRAMA2_LOG_ERROR() << message->toStdString();
      throw;
    }
    catch(const std::exception& e)
    {
      const char* message = e.what();
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
    catch(...)
    {
      QString message = QObject::tr("Unexpected error removing a data provider.");
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
  }

  emit dataProviderUpdated(provider);
  emit dataSetRemoved(dataset.id());
}

terrama2::core::DataProvider
terrama2::core::DataManager::findDataProvider(const uint64_t id) const
{
  if(id == 0)
  {
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not find a data provider with identifier: 0."));
  }

// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->providers.find(id);

  if(it !=  pimpl_->providers.end())
    return it->second;

  QString err_msg(QObject::tr("Could not find a data provider with id:: %1"));
  err_msg = err_msg.arg(id);
  throw InvalidArgumentException() << ErrorDescription(err_msg);
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

  QString err_msg(QObject::tr("Could not find a data provider with name:: %1"));
  err_msg = err_msg.arg(name.c_str());
  throw InvalidArgumentException() << ErrorDescription(err_msg);
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

  QString err_msg(QObject::tr("Could not find a dataset with name:: %1"));
  err_msg = err_msg.arg(name.c_str());
  throw InvalidArgumentException() << ErrorDescription(err_msg);
}

terrama2::core::DataSet terrama2::core::DataManager::findDataSet(const uint64_t id) const
{
  if(id == 0)
  {
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not find a data provider with identifier: 0."));
  }

// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);


  auto it = pimpl_->datasets.find(id);

  if(it !=  pimpl_->datasets.end())
    return it->second;

  QString err_msg(QObject::tr("Could not find a dataset with id:: %1"));
  err_msg = err_msg.arg(id);
  throw InvalidArgumentException() << ErrorDescription(err_msg);
}

std::vector<terrama2::core::DataProvider> terrama2::core::DataManager::providers(terrama2::core::DataProvider::Origin origin) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  std::vector<DataProvider> providers;

  for(auto it = pimpl_->providers.begin(); it != pimpl_->providers.end(); ++it)
  {
    if(it->second.origin() == origin)
      providers.push_back(it->second);
  }

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
  qRegisterMetaType<DataProvider>("DataProvider");
  qRegisterMetaType<DataSet>("DataSet");
  qRegisterMetaType<uint64_t>("uint64_t");
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
