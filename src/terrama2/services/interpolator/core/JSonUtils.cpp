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

#include "JSonUtils.hpp"
//#include "Intersection.hpp"
//#include "../../../core/Exception.hpp"
//#include "../../../core/data-model/DataManager.hpp"
//#include "../../../core/utility/JSonUtils.hpp"
//#include "../../../core/utility/Logger.hpp"

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::fromInterpolatorJson(QJsonObject json, terrama2::core::DataManager* dataManager)
{
  Interpolator* res = 0;

  return res;
//  if(json["class"].toString() != "Collector")
//  {
//    QString errMsg = QObject::tr("Invalid Collector JSON object.");
//    TERRAMA2_LOG_ERROR() << errMsg;
//    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
//  }

//  if(!(json.contains("id")
//   && json.contains("project_id")
//    && json.contains("service_instance_id")
//     && json.contains("input_data_series")
//      && json.contains("output_data_series")
//       && json.contains("input_output_map")
//         && json.contains("active")))
//  {
//    QString errMsg = QObject::tr("Invalid Collector JSON object.");
//    TERRAMA2_LOG_ERROR() << errMsg;
//    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
//  }

//  terrama2::services::collector::core::Collector* collector = new terrama2::services::collector::core::Collector();
//  terrama2::services::collector::core::CollectorPtr collectorPtr(collector);

//  collector->id = static_cast<uint32_t>(json["id"].toInt());
//  collector->projectId = static_cast<uint32_t>(json["project_id"].toInt());
//  collector->serviceInstanceId = static_cast<uint32_t>(json["service_instance_id"].toInt());

//  collector->inputDataSeries = static_cast<uint32_t>(json["input_data_series"].toInt());
//  collector->outputDataSeries = static_cast<uint32_t>(json["output_data_series"].toInt());

//  auto inOutArray = json["input_output_map"].toArray();
//  auto it = inOutArray.begin();
//  for(; it != inOutArray.end(); ++it)
//  {
//    auto obj = (*it).toObject();
//    collector->inputOutputMap.emplace(obj["input"].toInt(), obj["output"].toInt());
//  }

//  if(json.contains("schedule") && !json["schedule"].isNull())
//  {
//    collector->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());
//  }
//  QJsonObject json
//  collector->filter = terrama2::core::fromFilterJson(json["filter"].toObject(), dataManager);
//  collector->intersection = terrama2::services::collector::core::fromIntersectionJson(json["intersection"].toObject());
//  collector->active = json["active"].toBool();

//  return collectorPtr;
}

//terrama2::services::collector::core::IntersectionPtr terrama2::services::collector::core::fromIntersectionJson(QJsonObject json)
//{
//  if(json.empty())
//    return nullptr;

//  if(! (json.contains("collector_id") && json.contains("attribute_map")))
//  {
//    QString errMsg = QObject::tr("Invalid Intersection JSON object.");
//    TERRAMA2_LOG_ERROR() << errMsg;
//    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
//  }

//  Intersection* intersection = new Intersection();
//  IntersectionPtr intersectionPtr(intersection);

//  intersection->collectorId = json["collector_id"].toInt();

//  QJsonObject attributeMapJson = json["attribute_map"].toObject();
//  std::map<DataSeriesId, std::vector<IntersectionAttribute> > attributeMap;
//  for(auto it = attributeMapJson.begin(); it != attributeMapJson.end(); ++it)
//  {
//    QJsonArray attrListJson = it.value().toArray();

//    std::vector<IntersectionAttribute> vecAttributes;
//    for(int index = 0; index < attrListJson.size(); ++index)
//    {
//      QJsonObject value = attrListJson[index].toObject();
//      IntersectionAttribute intersectionAttribute;
//      intersectionAttribute.attribute = value["attribute"].toString().toStdString();
//      intersectionAttribute.alias = value["alias"].toString().toStdString();
//      vecAttributes.push_back(intersectionAttribute);
//    }

//    attributeMap[it.key().toInt()] = vecAttributes;
//  }
//  intersection->attributeMap = attributeMap;



//  return intersectionPtr;
//}

QJsonObject terrama2::services::interpolator::core::toJson(Interpolator* interpolator)
{
  QJsonObject obj;
//  obj.insert("class", QString("Collector"));
//  obj.insert("id", static_cast<int32_t>(collector->id));
//  obj.insert("project_id", static_cast<int32_t>(collector->projectId));
//  obj.insert("service_instance_id", static_cast<int32_t>(collector->serviceInstanceId));
//  obj.insert("input_data_series", static_cast<int32_t>(collector->inputDataSeries));
//  obj.insert("output_data_series", static_cast<int32_t>(collector->outputDataSeries));
//  obj.insert("schedule", terrama2::core::toJson(collector->schedule));
//  obj.insert("intersection", terrama2::services::collector::core::toJson(collector->intersection));
//  obj.insert("active", collector->active);

//  QJsonArray array;
//  for(auto it : collector->inputOutputMap)
//  {
//    QJsonObject dataset;
//    dataset.insert("input", static_cast<int32_t>(it.first));
//    dataset.insert("output", static_cast<int32_t>(it.second));
//    array.push_back(dataset);
//  }
//  obj.insert("input_output_map", array);

  return obj;
}

//QJsonObject terrama2::services::collector::core::toJson(IntersectionPtr intersection)
//{
//  QJsonObject json;

//  json["class"] = QString("Intersection");

//  json["collector_id"] = static_cast<qint32>(intersection->collectorId);

//  QJsonObject attributeMapJson;
//  for(auto it = intersection->attributeMap.begin(); it != intersection->attributeMap.end(); ++it)
//  {
//    QJsonArray attrList;
//    for(auto& intersectionAttribute : it->second)
//    {
//      QJsonObject value;
//      value["attribute"] = (QString(intersectionAttribute.attribute.c_str()));
//      value["alias"] = (QString(intersectionAttribute.alias.c_str()));
//      attrList.push_back(value);
//    }
//    attributeMapJson[QString(it->first)] = attrList;
//  }
//  json["attribute_map"] = attributeMapJson;

//  return json;
//}
