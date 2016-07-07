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
  \file src/terrama2/services/view/core/JSonUtils.cpp

  \brief Methods to convertion between a View and JSon object

  \author Vinicius Campanha
*/

// TerraMA2
#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// Terralib
#include <terralib/geometry/WKTReader.h>

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::services::view::core::ViewPtr terrama2::services::view::core::fromViewJson(QJsonObject json)
{
  if(json["class"].toString() != "View")
  {
    QString errMsg = QObject::tr("Invalid View JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  // VINICIUS:
  if(!json.contains("id")
     || !json.contains("project_id")
     || !json.contains("service_instance_id")
     || !json.contains("active")
     || !json.contains("resolutionWidth")
     || !json.contains("resolutionHeight")
     || !json.contains("schedule")
     || !json.contains("filter")
     || !json.contains("dataset_series_list"))
  {
    QString errMsg = QObject::tr("Invalid View JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
  terrama2::services::view::core::ViewPtr viewPtr(view);

  view->id = static_cast<uint32_t>(json["id"].toInt());
  view->projectId = static_cast<uint32_t>(json["project_id"].toInt());
  view->serviceInstanceId = static_cast<uint32_t>(json["service_instance_id"].toInt());
  view->active = json["active"].toBool();
  view->resolutionWidth = static_cast<uint32_t>(json["resolutionWidth"].toInt());
  view->resolutionHeight = static_cast<uint32_t>(json["resolutionHeight"].toInt());

  view->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());
  view->filter = terrama2::core::fromFilterJson(json["filter"].toObject());

  auto datasetSeriesArray = json["dataset_series_list"].toArray();
  auto it = datasetSeriesArray.begin();
  for(; it != datasetSeriesArray.end(); ++it)
  {
    auto obj = (*it).toObject();
    view->dataSeriesList.push_back(static_cast<uint32_t>(obj["dataset_series_id"].toInt()));
  }

  return viewPtr;
}


QJsonObject terrama2::services::view::core::toJson(ViewPtr view)
{
  QJsonObject obj;
  obj.insert("class", QString("View"));
  obj.insert("id", static_cast<qint64>(view->id));
  obj.insert("project_id", static_cast<qint64>(view->projectId));
  obj.insert("service_instance_id", static_cast<qint64>(view->serviceInstanceId));
  obj.insert("active", view->active);
  obj.insert("resolutionWidth", static_cast<qint64>(view->resolutionWidth));
  obj.insert("resolutionHeight", static_cast<qint64>(view->resolutionHeight));
  obj.insert("schedule", terrama2::core::toJson(view->schedule));
  obj.insert("filter", terrama2::core::toJson(view->filter));

  QJsonArray array;
  for(auto it : view->dataSeriesList)
  {
    QJsonObject datasetSeries;
    datasetSeries.insert("dataset_series_id", static_cast<qint64>(it));
    array.push_back(datasetSeries);
  }
  obj.insert("dataset_series_list", array);

  return obj;
}

