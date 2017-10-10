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
 * \file interpolator/core/DataManager.cpp
 *
 * \author Frederico Augusto Bedê
*/

#include "DataManager.hpp"
#include "Interpolator.hpp"
#include "InterpolatorFactories.h"
#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/utility/Logger.hpp"

// STL
#include <mutex>

// Qt
#include <QJsonValue>
#include <QJsonArray>

terrama2::services::interpolator::core::InterpolatorParamsPtr terrama2::services::interpolator::core::DataManager::findInterpolatorParams(InterpolatorId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = interpolatorsParams_.find(id);
  if(it == interpolatorsParams_.cend())
  {
    QString errMsg = QObject::tr("Interpolator not registered.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

bool terrama2::services::interpolator::core::DataManager::hasInterpolator(InterpolatorId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = interpolatorsParams_.find(id);
  return it != interpolatorsParams_.cend();
}

void terrama2::services::interpolator::core::DataManager::add(terrama2::services::interpolator::core::InterpolatorParamsPtr params)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(params->id_ == terrama2::core::InvalidId())
    {
      QString errMsg = QObject::tr("Can not add a data provider with an invalid id.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    params->dataManager_ = this;

    TERRAMA2_LOG_DEBUG() << tr("Interpolator parameters added");
    interpolatorsParams_[params->id_] = params;
  }

  emit interpolatorAdded(params);
}

void terrama2::services::interpolator::core::DataManager::update(terrama2::services::interpolator::core::InterpolatorParamsPtr params)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    blockSignals(true);
    removeInterpolator(params->id_);
    add(params);
    blockSignals(false);
  }

  emit interpolatorUpdated(params);
}

void terrama2::services::interpolator::core::DataManager::removeInterpolator(InterpolatorId interpolatorId)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto itPr = interpolatorsParams_.find(interpolatorId);
    if(itPr == interpolatorsParams_.end())
    {
      QString errMsg = QObject::tr("DataProvider not registered.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    interpolatorsParams_.erase(itPr);
  }

  emit interpolatorRemoved(interpolatorId);
}

terrama2::services::interpolator::core::DataManager::DataManager()
{

}

terrama2::services::interpolator::core::DataManager::~DataManager()
{

}

void terrama2::services::interpolator::core::DataManager::addJSon(const QJsonObject& obj)
{
  try
  {
    terrama2::core::DataManager::DataManager::addJSon(obj);

    auto interpolators = obj["Interpolators"].toArray();
    for(auto json : interpolators)
    {
      InterpolatorParamsPtr dataPtr(terrama2::services::interpolator::core::fromInterpolatorJson(json.toObject(), this));

      if(hasInterpolator(dataPtr->id_))
        update(dataPtr);
      else
        add(dataPtr);
    }
  }
  catch(const terrama2::Exception& /*e*/)
  {
    // logged on throw...
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown error...");
  }
}

void terrama2::services::interpolator::core::DataManager::removeJSon(const QJsonObject& obj)
{
  try
  {
    auto interpolators = obj["Interpolators"].toArray();
    for(auto json : interpolators)
    {
      auto dataId = json.toInt();
      removeInterpolator(dataId);
    }

    terrama2::core::DataManager::DataManager::removeJSon(obj);
  }
  catch(const terrama2::Exception& /*e*/)
  {
    // loggend on throw...
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown error...");
  }
}
