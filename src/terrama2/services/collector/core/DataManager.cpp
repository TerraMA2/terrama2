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
  \file terrama2/services/collector/core/Collector.hpp

  \brief Model class for the collector configuration.

  \author Jano Simas
*/

#include "DataManager.hpp"
#include "Collector.hpp"
#include "Exception.hpp"
#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/utility/Logger.hpp"

// STL
#include <mutex>

// Qt
#include <QJsonValue>
#include <QJsonArray>

terrama2::services::collector::core::CollectorPtr terrama2::services::collector::core::DataManager::findCollector(CollectorId id) const
{
  return collectors_.at(id);
}

void terrama2::services::collector::core::DataManager::add(terrama2::services::collector::core::CollectorPtr collector)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(collector->id == terrama2::core::InvalidId())
    {
      QString errMsg = QObject::tr("Can not add a data provider with an invalid id.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    collectors_[collector->id] = collector;
  }

  emit collectorAdded(collector);
}

void terrama2::services::collector::core::DataManager::update(terrama2::services::collector::core::CollectorPtr collector)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    blockSignals(true);
    removeCollector(collector->id);
    add(collector);
    blockSignals(false);
  }

  emit collectorUpdated(collector);
}

void terrama2::services::collector::core::DataManager::removeCollector(CollectorId collectorId)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto itPr = collectors_.find(collectorId);
    if(itPr == collectors_.end())
    {
      QString errMsg = QObject::tr("DataProvider not registered.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    collectors_.erase(itPr);
  }

  emit collectorRemoved(collectorId);
}

void terrama2::services::collector::core::DataManager::addFromJSON(const QJsonObject& obj)
{
  try
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    terrama2::core::DataManager::DataManager::addFromJSON(obj);

    auto collectors = obj["Collectors"].toArray();
    for(auto json : collectors)
    {
      auto dataPtr = terrama2::services::collector::core::fromCollectorJson(json.toObject());
      add(dataPtr);
    }
  }
  catch(terrama2::Exception& /*e*/)
  {
    // loggend on throw...
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString().c_str();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknow error...");
  }
}
