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
  \file src/terrama2/services/collector/core/JSonUtils.cpp

  \brief

  \author Jano Simas
*/

#include "../../../core/Exception.hpp"
#include "../../../core/utility/JSonUtils.hpp"

#include "JSonUtils.hpp"

// Terralib
#include <terralib/geometry/WKTReader.h>

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::services::collector::core::CollectorPtr terrama2::services::collector::core::fromCollectorJson(QJsonObject json)
{
  if(json["class"].toString() != "Collector")
  {
    throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));
  }

  if(!(json.contains("id") && json.contains("project_id") && json.contains("service_instance_id") && json.contains("input_data_series") &&
       json.contains("output_data_series") && json.contains("input_output_map") && json.contains("schedule") && json.contains("intersection") &&
       json.contains("active")))
    throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  terrama2::services::collector::core::Collector* collector = new terrama2::services::collector::core::Collector();
  terrama2::services::collector::core::CollectorPtr collectorPtr(collector);

  collector->id = json["id"].toInt();
  collector->projectId = json["project_id"].toInt();

  collector->inputDataSeries = json["input_data_series"].toInt();
  collector->outputDataSeries = json["output_data_series"].toInt();

  auto inOutArray = json["input_output_map"].toArray();
  auto it = inOutArray.begin();
  for(; it != inOutArray.end(); ++it)
  {
    auto obj = (*it).toObject();
    collector->inputOutputMap.emplace(obj["input"].toInt(), obj["output"].toInt());
  }

  collector->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());
  collector->intersection = terrama2::services::collector::core::fromIntersectionJson(json["intersection"].toObject());
  collector->active = json["active"].toBool();

  return collectorPtr;
}

terrama2::services::collector::core::IntersectionPtr terrama2::services::collector::core::fromIntersectionJson(QJsonObject json)
{
  //FIXME: implement fromIntersectionJson
  return nullptr;
}

QJsonObject terrama2::services::collector::core::toJson(CollectorPtr collector)
{
  QJsonObject obj;
  obj.insert("class", QString("Collector"));
  obj.insert("id", static_cast<qint64>(collector->id));
  obj.insert("project_id", static_cast<qint64>(collector->projectId));
  obj.insert("service_instance_id", static_cast<qint64>(collector->serviceInstanceId));
  obj.insert("input_data_series", static_cast<qint64>(collector->inputDataSeries));
  obj.insert("output_data_series", static_cast<qint64>(collector->outputDataSeries));
  obj.insert("schedule", terrama2::core::toJson(collector->schedule));
  obj.insert("intersection", terrama2::services::collector::core::toJson(collector->intersection));
  obj.insert("active", collector->active);

  QJsonArray array;
  for(auto it : collector->inputOutputMap)
  {
    QJsonObject dataset;
    dataset.insert("input", static_cast<qint64>(it.first));
    dataset.insert("output", static_cast<qint64>(it.second));
    array.push_back(dataset);
  }
  obj.insert("input_output_map", array);

  return obj;
}

QJsonObject terrama2::services::collector::core::toJson(IntersectionPtr intersection)
{
  //FIXME: implement toJson(IntersectionPtr intersection)
  return QJsonObject();
}
