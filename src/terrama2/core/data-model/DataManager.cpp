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
#include "../typedef.hpp"

// TerraMA2 Logger
#include "../utility/Logger.hpp"

// STL
#include <algorithm>
#include <mutex>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

struct terrama2::core::DataManager::Impl
{
  std::map<DataProviderId, DataProviderPtr> providers; //!< A map from data-provider-id to data-provider.
  std::map<DataSeriesId, DataSeriesPtr> dataseries;       //!< A map from data-set-id to dataseries.
  std::recursive_mutex mtx;                             //!< A mutex to syncronize all operations.
};

void terrama2::core::DataManager::add(DataProviderPtr provider)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);

    if(provider->name.empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a data provider with empty name."));

    if(provider->id == terrama2::core::InvalidId())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a data provider with an invalid id."));

    pimpl_->providers[provider->id] = provider;
  }

  emit dataProviderAdded(provider);
}

void terrama2::core::DataManager::add(DataSeriesPtr dataseries)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);

    if(dataseries->name.empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a data provider with empty name."));

    if(dataseries->id == terrama2::core::InvalidId())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a data provider with an invalid id."));

    auto itPr = pimpl_->providers.find(dataseries->dataProviderId);

    if(itPr == pimpl_->providers.end())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add a dataseries with a non-registered data provider."));

    auto itDs = pimpl_->dataseries.find(dataseries->id);
    if(itDs != pimpl_->dataseries.end())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("DataSeries already registered. Is this an update?"));

    pimpl_->dataseries[dataseries->id] = dataseries;
  }

  emit dataSeriesAdded(dataseries);
}

void terrama2::core::DataManager::update(terrama2::core::DataProviderPtr provider)
{
  {
    std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);
    blockSignals(true);
    removeDataProvider(provider->id, true);
    add(provider);
    blockSignals(false);
  }

  emit dataProviderUpdated(provider);
}

void terrama2::core::DataManager::update(terrama2::core::DataSeriesPtr dataseries, const bool shallowSave)
{
  {
    std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);
    blockSignals(true);
    removeDataSeries(dataseries->id);
    add(dataseries);
    blockSignals(false);
  }

  emit dataSeriesUpdated(dataseries);
}

void terrama2::core::DataManager::removeDataProvider(const DataProviderId id, const bool shallowRemove)
{
  {
    std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);
    auto itPr = pimpl_->providers.find(id);
    if(itPr == pimpl_->providers.end())
    {
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("DataProvider not registered."));
    }

    pimpl_->providers.erase(itPr);

    if(!shallowRemove)
    {
      for(const auto& pair : pimpl_->dataseries)
      {
        const auto& dataseries = pair.second;
        if(dataseries->dataProviderId == id)
          removeDataSeries(dataseries->id);
      }
    }
  }

  emit dataProviderRemoved(id);
}

void terrama2::core::DataManager::removeDataSeries(const DataSeriesId id)
{
  {
    std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);
    const auto& it = pimpl_->dataseries.find(id);
    if(it == pimpl_->dataseries.end())
    {
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("DataSeries not registered."));
    }

    pimpl_->dataseries.erase(it);
  }

  emit dataSeriesRemoved(id);
}

terrama2::core::DataProviderPtr terrama2::core::DataManager::findDataProvider(const std::string& name) const
{
  std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);

  const auto& it = std::find_if(pimpl_->providers.cbegin(), pimpl_->providers.cend(), [name](std::pair<DataProviderId, DataProviderPtr> provider){ return provider.second->name == name; });
  if(it == pimpl_->providers.cend())
  {
    throw terrama2::InvalidArgumentException() <<
          ErrorDescription(QObject::tr("DataProvider not registered."));
  }

  return it->second;
}

terrama2::core::DataProviderPtr terrama2::core::DataManager::findDataProvider(const DataProviderId id) const
{
  std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);

  const auto& it = pimpl_->providers.find(id);
  if(it == pimpl_->providers.cend())
  {
    throw terrama2::InvalidArgumentException() <<
          ErrorDescription(QObject::tr("DataProvider not registered."));
  }

  return it->second;
}

terrama2::core::DataSeriesPtr terrama2::core::DataManager::findDataSeries(const std::string& name) const
{
  std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);

  const auto& it = std::find_if(pimpl_->dataseries.cbegin(), pimpl_->dataseries.cend(), [name](std::pair<DataSeriesId, DataSeriesPtr> series){ return series.second->name == name; });
  if(it == pimpl_->dataseries.cend())
  {
    throw terrama2::InvalidArgumentException() <<
          ErrorDescription(QObject::tr("DataSeries not registered."));
  }

  return it->second;
}

terrama2::core::DataSeriesPtr terrama2::core::DataManager::findDataSeries(const DataSeriesId id) const
{
  std::lock_guard<std::recursive_mutex> lock(pimpl_->mtx);

  const auto& it = pimpl_->dataseries.find(id);
  if(it == pimpl_->dataseries.cend())
  {
    throw terrama2::InvalidArgumentException() <<
          ErrorDescription(QObject::tr("DataSeries not registered."));
  }

  return it->second;
}

terrama2::core::DataManager::DataManager()
  : pimpl_(new Impl)
{
  qRegisterMetaType<DataProvider>("DataProvider");
  qRegisterMetaType<DataSeries>("DataSeries");
  qRegisterMetaType<uint64_t>("uint64_t");
}

terrama2::core::DataManager::~DataManager()
{
  delete pimpl_;
}
