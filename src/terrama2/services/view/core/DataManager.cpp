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
  \file terrama2/services/view/core/DataManager.cpp

  \brief Model class for the view configuration.

  \author Vinicius Campanha
*/

// TerraMA2
#include "DataManager.hpp"
#include "View.hpp"
#include "Exception.hpp"
#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/utility/Logger.hpp"

// STL
#include <mutex>

// Qt
#include <QJsonValue>
#include <QJsonArray>

terrama2::services::view::core::ViewPtr terrama2::services::view::core::DataManager::findView(ViewId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = view_.find(id);
  if(it == view_.cend())
  {
    QString errMsg = QObject::tr("View not registered.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

bool terrama2::services::view::core::DataManager::hasView(ViewId id) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = view_.find(id);
  return it != view_.cend();
}

void terrama2::services::view::core::DataManager::add(terrama2::services::view::core::ViewPtr view)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(view->id == terrama2::core::InvalidId())
    {
      QString errMsg = QObject::tr("Can not add a data provider with an invalid id.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    TERRAMA2_LOG_DEBUG() << tr("View %1 added").arg(view->id);
    view_[view->id] = view;
  }

  emit viewAdded(view);
}

void terrama2::services::view::core::DataManager::update(terrama2::services::view::core::ViewPtr view)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    blockSignals(true);
    removeView(view->id);
    add(view);
    blockSignals(false);
  }

  emit viewUpdated(view);
}

void terrama2::services::view::core::DataManager::removeView(ViewId viewId)
{
  DataSeriesId dataSeriesId;

  ViewPtr viewPtr;
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto itPr = view_.find(viewId);
    if(itPr == view_.end())
    {
      QString errMsg = QObject::tr("View not registered.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }
    viewPtr = itPr->second;
    dataSeriesId = itPr->second->dataSeriesID;
    view_.erase(itPr);
  }

  emit viewRemoved(viewPtr, dataSeriesId);
}

void terrama2::services::view::core::DataManager::addJSon(const QJsonObject& obj)
{
  try
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    terrama2::core::DataManager::DataManager::addJSon(obj);

    auto view = obj["Views"].toArray();
    for(auto json : view)
    {
      auto dataPtr = terrama2::services::view::core::fromViewJson(json.toObject());
      if(hasView(dataPtr->id))
        update(dataPtr);
      else
        add(dataPtr);
    }
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

void terrama2::services::view::core::DataManager::removeJSon(const QJsonObject& obj)
{
  try
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto view = obj["Views"].toArray();
    for(auto json : view)
    {
      auto dataId = json.toInt();
      removeView(dataId);
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
