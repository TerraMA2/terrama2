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
#include "../Typedef.hpp"

// TerraMA2 Logger
#include "../utility/Logger.hpp"
#include "../utility/JSonUtils.hpp"

// STL
#include <algorithm>
#include <memory>

//Qt
#include <QJsonArray>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

std::unique_lock<std::recursive_mutex> terrama2::core::DataManager::getLock()
{
  std::unique_lock<std::recursive_mutex> lock(mtx_);
  return std::move(lock);
}

void terrama2::core::DataManager::add(DataProviderPtr provider)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(provider->name.empty())
    {
      QString errMsg = QObject::tr("Can not add a data provider with empty name.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    if(provider->id == terrama2::core::InvalidId())
    {
      QString errMsg = QObject::tr("Can not add a data provider with an invalid id.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    providers_.emplace(provider->id, provider);
  }

  emit dataProviderAdded(provider);
}

void terrama2::core::DataManager::add(DataSeriesPtr dataseries)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(dataseries->name.empty())
    {
      QString errMsg = QObject::tr("Can not add a data series with empty name.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    if(dataseries->id == terrama2::core::InvalidId())
    {
      QString errMsg = QObject::tr("Can not add a data series with invalid identifier.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    try
    {
      providers_.at(dataseries->dataProviderId);
    }
    catch (const std::out_of_range&)
    {
      QString errMsg = QObject::tr("Can not add a dataseries with a non-registered data provider: %1.").arg(dataseries->dataProviderId);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    try
    {
      dataseries_.at(dataseries->id);

      QString errMsg = QObject::tr("DataSeries already registered.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }
    catch (const std::out_of_range&)
    {

    }

    dataseries_.emplace(dataseries->id, dataseries);
  }

  emit dataSeriesAdded(dataseries);
}

void terrama2::core::DataManager::update(terrama2::core::DataProviderPtr provider)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
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
    std::lock_guard<std::recursive_mutex> lock(mtx_);
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
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto itPr = providers_.find(id);
    if(itPr == providers_.end())
    {
      QString errMsg = QObject::tr("Could not find a data provider with the given id: %1.").arg(id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    providers_.erase(itPr);

    if(!shallowRemove)
    {
      for(const auto& pair : dataseries_)
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
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    const auto& it = dataseries_.find(id);
    if(it == dataseries_.end())
    {
      QString errMsg = QObject::tr("Could not find a data series with the given id: %1.").arg(id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    dataseries_.erase(it);
  }

  emit dataSeriesRemoved(id);
}

terrama2::core::DataProviderPtr terrama2::core::DataManager::findDataProvider(const std::string& name) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = std::find_if(providers_.cbegin(), providers_.cend(), [name](std::pair<DataProviderId, DataProviderPtr> provider) { return provider.second->name == name; });
  if(it == providers_.cend())
  {
    QString errMsg = QObject::tr("Could not find a data provider with the given name: %1.").arg(QString::fromStdString(name));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

bool terrama2::core::DataManager::hasDataProvider(DataProviderId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = providers_.find(id);
  return it != providers_.cend();
}

terrama2::core::DataProviderPtr terrama2::core::DataManager::findDataProvider(const DataProviderId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = providers_.find(id);
  if(it == providers_.cend())
  {
    QString errMsg = QObject::tr("Could not find a data provider with the given id: %1.").arg(id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

bool terrama2::core::DataManager::hasDataSeries(DataSeriesId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = dataseries_.find(id);
  return it != dataseries_.cend();
}

terrama2::core::DataSeriesPtr terrama2::core::DataManager::findDataSeries(const std::string& name) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);
//TODO: create a map name->id to fasten the cases where this function is called to many times?
  const auto& it = std::find_if(dataseries_.cbegin(), dataseries_.cend(), [name] (std::pair<DataSeriesId, DataSeriesPtr> series)
                                                                          { return series.second->name == name; });
  if(it == dataseries_.cend())
  {
    QString errMsg = QObject::tr("Could not find a data series with the given name: %1.").arg(QString::fromStdString(name));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

terrama2::core::DataSeriesPtr terrama2::core::DataManager::findDataSeries(const DataSeriesId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = dataseries_.find(id);
  if(it == dataseries_.cend())
  {
    QString errMsg = QObject::tr("Could not find a data series with the given id: %1.").arg(id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

terrama2::core::DataManager::DataManager()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  qRegisterMetaType<DataSeriesPtr>("DataSeriesPtr");
  qRegisterMetaType<uint32_t>("uint32_t");
}

void terrama2::core::DataManager::addJSon(const QJsonObject& obj)
{
  auto dataProviders = obj["DataProviders"].toArray();
  for(auto json : dataProviders)
  {
    auto dataPtr = terrama2::core::fromDataProviderJson(json.toObject());
    if(hasDataProvider(dataPtr->id))
      update(dataPtr);
    else
      add(dataPtr);
  }

  auto dataSeries = obj["DataSeries"].toArray();
  for(auto json : dataSeries)
  {
    auto dataPtr = terrama2::core::fromDataSeriesJson(json.toObject());
    if(hasDataSeries(dataPtr->id))
      update(dataPtr);
    else
      add(dataPtr);
  }
}

void terrama2::core::DataManager::removeJSon(const QJsonObject& obj)
{
  auto dataSeries = obj["DataSeries"].toArray();
  for(auto json : dataSeries)
  {
    auto dataId = json.toInt();
    removeDataSeries(dataId);
  }

  auto dataProviders = obj["DataProviders"].toArray();
  for(auto json : dataProviders)
  {
    auto dataId = json.toInt();
    removeDataProvider(dataId);
  }
}
