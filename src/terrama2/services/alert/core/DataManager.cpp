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
  \file terrama2/services/alert/core/Alert.hpp

  \brief Model class for the alert configuration.

  \author Jano Simas
*/

#include "DataManager.hpp"
#include "Alert.hpp"
#include "Exception.hpp"
#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/utility/Logger.hpp"

// STL
#include <mutex>

// Qt
#include <QJsonValue>
#include <QJsonArray>

terrama2::services::alert::core::AlertPtr terrama2::services::alert::core::DataManager::findAlert(AlertId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = alerts_.find(id);
  if(it == alerts_.cend())
  {
    QString errMsg = QObject::tr("Alert not registered.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

bool terrama2::services::alert::core::DataManager::hasAlert(AlertId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = alerts_.find(id);
  return it != alerts_.cend();
}

void terrama2::services::alert::core::DataManager::add(terrama2::services::alert::core::AlertPtr alert)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(alert->id == terrama2::core::InvalidId())
    {
      QString errMsg = QObject::tr("Can not add a data provider with an invalid id.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    TERRAMA2_LOG_DEBUG() << tr("Alert added");
    alerts_[alert->id] = alert;
  }

  emit alertAdded(alert);
}

void terrama2::services::alert::core::DataManager::update(terrama2::services::alert::core::AlertPtr alert)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    blockSignals(true);
    removeAlert(alert->id);
    add(alert);
    blockSignals(false);
  }

  emit alertUpdated(alert);
}

void terrama2::services::alert::core::DataManager::removeAlert(AlertId alertId)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto itPr = alerts_.find(alertId);
    if(itPr == alerts_.end())
    {
      QString errMsg = QObject::tr("DataProvider not registered.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    alerts_.erase(itPr);
  }

  emit alertRemoved(alertId);
}

void terrama2::services::alert::core::DataManager::addJSon(const QJsonObject& obj)
{
  try
  {
    terrama2::core::DataManager::DataManager::addJSon(obj);

    auto alerts = obj["Alerts"].toArray();
    for(auto json : alerts)
    {
      auto dataPtr = terrama2::services::alert::core::fromAlertJson(json.toObject(), this);
      if(hasAlert(dataPtr->id))
        update(dataPtr);
      else
        add(dataPtr);
    }
  }
  catch(const terrama2::Exception& /*e*/)
  {
    // logged on throw...
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown error...");
  }
}

void terrama2::services::alert::core::DataManager::removeJSon(const QJsonObject& obj)
{
  try
  {
    auto alerts = obj["Alerts"].toArray();
    for(auto json : alerts)
    {
      auto dataId = json.toInt();
      removeAlert(dataId);
    }

    terrama2::core::DataManager::DataManager::removeJSon(obj);
  }
  catch(const terrama2::Exception& /*e*/)
  {
    // loggend on throw...
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown error...");
  }
}
