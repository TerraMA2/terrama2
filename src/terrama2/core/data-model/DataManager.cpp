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
#include "DataProvider.hpp"
#include "DataSeries.hpp"
#include "DataSet.hpp"
#include "../Exception.hpp"

// TerraMA2 Logger
#include "../utility/Logger.hpp"

// STL
#include <algorithm>
#include <mutex>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>


struct terrama2::core::DataManager::Impl
{
  std::map<uint64_t, DataProvider> providers; //!< A map from data-provider-id to data-provider.
  std::map<uint64_t, DataSeries> dataseries;       //!< A map from data-set-id to dataseries.
  bool dataLoaded;                            //!< A boolean that defines if the data has already been loaded.
  std::mutex mtx;                             //!< A mutex to syncronize all operations.
};

void terrama2::core::DataManager::load(bool memory)
{

// Inside a block so the lock is released before emitting the signal
  {
// only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

// if the data has already been loaded there is nothing to be done.
    if(pimpl_->dataLoaded)
      return;

    assert(pimpl_->providers.empty());
    assert(pimpl_->dataseries.empty());

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
    pimpl_->dataseries.clear();
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

    if(provider.name.empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a data provider with empty name."));

    try
    {
      std::shared_ptr<te::da::DataSourceTransactor> transactor;

      if(!shallowSave)
      {
        for(auto& dataseries: provider.dataseries)
        {
          pimpl_->dataseries[dataseries.id] = dataseries;
        }
      }

      pimpl_->providers[provider.id] = provider;
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

  emit dataProviderAdded(provider);

  if(!shallowSave)
  {
    for (auto& dataserie : provider.dataseries)
    {
      emit dataSeriesAdded(dataserie);
    }
  }
}

void terrama2::core::DataManager::add(DataSeries& dataseries, const bool shallowSave)
{

  DataProvider provider;

// Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(dataseries.name.empty())
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Can not add a dataseries with empty name."));

    if(dataseries.dataProviderId == 0)
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a dataseries with an invalid data provider."));

    auto it = pimpl_->providers.find(dataseries.dataProviderId);

    if(it == pimpl_->providers.end())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a dataseries with a non-registered data provider."));

    provider = it->second;

    try
    {
      it->second.dataseries.push_back(dataseries);
      pimpl_->dataseries[dataseries.id] = dataseries;
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
      QString message = QObject::tr("Unexpected error adding a dataseries and registering it.");
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
  }

  emit dataProviderUpdated(provider);
  emit dataSeriesAdded(dataseries);
}

void terrama2::core::DataManager::update(DataProvider& provider, const bool shallowSave)
{

  std::vector<DataSeries> added, updated;
  std::vector<DataSeriesId> removed;

// Inside a block so the lock is released before emitting the signal
  {

    // only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(provider.id == 0)
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a data provider with identifier: 0."));

    if(provider.name.empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a data provider with empty name."));

    try
    {
      auto it = pimpl_->providers.find(provider.id);

      if(it == pimpl_->providers.end())
      {
        throw terrama2::InvalidArgumentException() <<
              ErrorDescription(QObject::tr("Can not update a provider not registered in the data manager."));
      }

      //FIXME: check this
      /*
      auto transactor = ApplicationController::getInstance().getTransactor();
      if(!shallowSave)
      {
        std::vector<uint64_t> ids = dao::DataProviderDAO::getDatasetsIds(provider.id, *transactor);

        for(auto& dataseries: provider.dataseries)
        {
          // Id exists just need to call update
          auto it = find (ids.begin(), ids.end(), dataseries.id);
          if (it != ids.end())
          {
            // Remove from the list, so what is left in this vector are the dataseries to remove
            ids.erase(it);

            pimpl_->dataseries[dataseries.id] = dataseries;
            updated.push_back(dataseries);
          }

          // Id is 0 for new items
          if(dataseries.id == 0)
          {
            added.push_back(dataseries);
          }
        }

        // What is left in vector are the the removed dataseries.
        for(auto dataSeriesId : ids)
        {
          removed.push_back(dataSeriesId);

          // removes dataSeries from the map

          auto it = pimpl_->dataseries.find(dataSeriesId);
          if(it == pimpl_->dataseries.end())
          {
            throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove a nonexistent dataseries."));
          }

          if(it != pimpl_->dataseries.end())
          {
            pimpl_->dataseries.erase(it);
          }
        }
      }
      */

      pimpl_->providers[provider.id] = provider;
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
    for(auto& dataseries: added)
    {
      dataSeriesAdded(dataseries);
    }

    for(auto& dataseries: updated)
    {
      dataSeriesUpdated(dataseries);
    }

    for(auto& id: removed)
    {
      dataSeriesRemoved(id);
    }
  }
  emit dataProviderUpdated(provider);
}

void terrama2::core::DataManager::update(DataSeries& dataseries, const bool shallowSave)
{
// Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(dataseries.id == 0)
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a dataseries with identifier: 0."));

    if(dataseries.name.empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a dataseries with empty name."));

    if(dataseries.dataProviderId == 0)
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not update a dataseries with an invalid data provider."));

    try
    {
      auto itDp = pimpl_->providers.find(dataseries.dataProviderId);

      if(itDp == pimpl_->providers.end())
        throw terrama2::InvalidArgumentException() <<
              ErrorDescription(QObject::tr("Can not update a nonexistent data provider."));

      auto itDs = pimpl_->dataseries.find(dataseries.id);

      if(itDs == pimpl_->dataseries.end())
        throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not update a nonexistent dataseries."));

      pimpl_->dataseries[dataseries.id] = dataseries;
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
      QString message = QObject::tr("Unexpected error updating a dataseries.");
      TERRAMA2_LOG_ERROR() << message;
      throw DataAccessException() << ErrorDescription(message);
    }
  }

  emit dataSeriesUpdated(dataseries);
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

        // removes all related dataseries from the map
        for(auto dataseries: dataProvider.dataseries)
        {
          auto itDs = pimpl_->dataseries.find(dataseries.id);
          if(itDs != pimpl_->dataseries.end())
          {
            pimpl_->dataseries.erase(itDs);
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

void terrama2::core::DataManager::removeDataSeries(const DataSeriesId id)
{
  DataSeries dataseries;
  DataProvider provider;

// Inside a block so the lock is released before emitting the signal
  {
// only one thread at time can access the data
    std::lock_guard<std::mutex> lock(pimpl_->mtx);

    if(id == 0)
    {
      throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove a dataseries with identifier: 0."));
    }


    try
    {
      auto it = pimpl_->dataseries.find(id);
      if(it == pimpl_->dataseries.end())
      {
        throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove a nonexistent dataseries."));
      }

// removes dataseries from the map
      if(it != pimpl_->dataseries.end())
      {
        dataseries = it->second;
        pimpl_->dataseries.erase(it);

        auto it = pimpl_->providers.find(dataseries.dataProviderId);

        if(it != pimpl_->providers.end())
        {
          provider = it->second;
          auto pos = std::find(provider.dataseries.begin(), provider.dataseries.end(), dataseries);
          provider.dataseries.erase(pos);
        }
        else
        {
          throw terrama2::InvalidArgumentException() <<
                ErrorDescription(QObject::tr("Can not remove a dataseries with invalid data provider."));
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
  emit dataSeriesRemoved(dataseries.id);
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
    if(it->second.name == name)
      return it->second;

    ++it;
  }

  QString err_msg(QObject::tr("Could not find a data provider with name:: %1"));
  err_msg = err_msg.arg(name.c_str());
  throw InvalidArgumentException() << ErrorDescription(err_msg);
}

terrama2::core::DataSeries terrama2::core::DataManager::findDataSeries(const std::string& name) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->dataseries.begin();

  while(it !=  pimpl_->dataseries.end())
  {
    if(it->second.name == name)
      return it->second;

    ++it;
  }

  QString err_msg(QObject::tr("Could not find a dataseries with name:: %1"));
  err_msg = err_msg.arg(name.c_str());
  throw InvalidArgumentException() << ErrorDescription(err_msg);
}

terrama2::core::DataSeries terrama2::core::DataManager::findDataSeries(const uint64_t id) const
{
  if(id == 0)
  {
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not find a data provider with identifier: 0."));
  }

// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);


  auto it = pimpl_->dataseries.find(id);

  if(it !=  pimpl_->dataseries.end())
    return it->second;

  QString err_msg(QObject::tr("Could not find a dataseries with id:: %1"));
  err_msg = err_msg.arg(id);
  throw InvalidArgumentException() << ErrorDescription(err_msg);
}

std::vector<terrama2::core::DataProvider> terrama2::core::DataManager::providers(const terrama2::core::DataProvider::DataProviderIntent intent) const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  std::vector<DataProvider> providers;

  for(auto it = pimpl_->providers.begin(); it != pimpl_->providers.end(); ++it)
  {
    if(it->second.intent == intent)
      providers.push_back(it->second);
  }

  return providers;
}

std::vector<terrama2::core::DataSeries> terrama2::core::DataManager::dataSeries() const
{
// only one thread at time can access the data
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  std::vector<DataSeries> dataseries;

  std::for_each(pimpl_->dataseries.begin(), pimpl_->dataseries.end(),
                [&dataseries](const std::map<uint64_t, DataSeries>::value_type& v){ dataseries.push_back(v.second); });

  return dataseries;
}

terrama2::core::DataManager::DataManager()
  : pimpl_(new Impl)
{
  pimpl_->dataLoaded = false;
  qRegisterMetaType<DataProvider>("DataProvider");
  qRegisterMetaType<DataSeries>("DataSeries");
  qRegisterMetaType<uint64_t>("uint64_t");
}

terrama2::core::DataManager::~DataManager()
{
  delete pimpl_;
}

bool terrama2::core::DataManager::isDataseriesNameValid(const std::string& name) const
{
  std::lock_guard<std::mutex> lock(pimpl_->mtx);

  auto it = pimpl_->dataseries.begin();

  while(it !=  pimpl_->dataseries.end())
  {
    if(it->second.name == name)
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
    if(it->second.name == name)
      return false;

    ++it;
  }

  return true;
}
