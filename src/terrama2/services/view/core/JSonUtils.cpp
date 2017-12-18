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
#include "serialization/Serialization.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// Terralib
#include <terralib/se/Style.h>

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QTemporaryFile>


QJsonObject terrama2::services::view::core::toJson(ViewPtr view)
{
  QJsonObject obj;
  obj.insert("class", QString("View"));
  obj.insert("name", QString(view->viewName.c_str()));
  obj.insert("id", static_cast<int32_t>(view->id));
  obj.insert("project_id", static_cast<int32_t>(view->projectId));
  obj.insert("service_instance_id", static_cast<int32_t>(view->serviceInstanceId));
  obj.insert("active", view->active);
  obj.insert("imageName", QString(view->imageName.c_str()));
  obj.insert("imageType", static_cast<int32_t>(view->imageType));
  obj.insert("imageResolutionWidth", static_cast<int32_t>(view->imageResolutionWidth));
  obj.insert("imageResolutionHeight", static_cast<int32_t>(view->imageResolutionHeight));
  obj.insert("srid", static_cast<int32_t>(view->srid));
  obj.insert("schedule", terrama2::core::toJson(view->schedule));

  obj.insert("dataseries_id", static_cast<int32_t>(view->dataSeriesID));

  obj.insert("legend", toJson(*view->legend.get()));

  return obj;
}


terrama2::services::view::core::ViewPtr terrama2::services::view::core::fromViewJson(QJsonObject json)
{
  if(json["class"].toString() != "View")
  {
    QString errMsg = QObject::tr("Invalid View JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!json.contains("name")
     || !json.contains("id")
     || !json.contains("project_id")
     || !json.contains("service_instance_id")
     || !json.contains("active")
     || !json.contains("dataseries_id")
     || !json.contains("legend")
     || !json.contains("schedule"))
  {
    QString errMsg = QObject::tr("Invalid View JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
  terrama2::services::view::core::ViewPtr viewPtr(view);

  view->viewName = json["name"].toString().toStdString();
  view->id = static_cast<uint32_t>(json["id"].toInt());
  view->projectId = static_cast<uint32_t>(json["project_id"].toInt());
  view->serviceInstanceId = static_cast<uint32_t>(json["service_instance_id"].toInt());
  view->active = json["active"].toBool();

  uint32_t dataseriesID = static_cast<uint32_t>(json["dataseries_id"].toInt());

  view->dataSeriesID = dataseriesID;

  if(!json["legend"].isNull())
  {
    view->legend.reset(fromLegendJson(json["legend"].toObject()));
  }
  else
  {
    if(view->legend)
      view->legend.release();
  }

  if(json.contains("properties"))
  {
    auto properties = json["properties"].toObject();
    for(auto it = properties.begin(); it != properties.end(); ++it)
    {
      view->properties.emplace(it.key().toStdString(), it.value().toString().toStdString());
    }
  }

  view->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());

  view->imageName = json["imageName"].toString().toStdString();
  view->imageType = te::map::ImageType(json["imageType"].toInt());
  view->imageResolutionWidth = static_cast<uint32_t>(json["imageResolutionWidth"].toInt());
  view->imageResolutionHeight = static_cast<uint32_t>(json["imageResolutionHeight"].toInt());
  view->srid = static_cast<uint32_t>(json["srid"].toInt());

  return viewPtr;
}

terrama2::services::view::core::View::Legend* terrama2::services::view::core::fromLegendJson(QJsonObject json)
{
  if(json["class"].toString() != "ViewStyleLegend")
  {
    QString errMsg = QObject::tr("Invalid View Legend JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!json.contains("type")
     || !json.contains("metadata")
     || !json.contains("colors"))
  {
    QString errMsg = QObject::tr("Invalid View Legend JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  View::Legend* legend = new View::Legend();

  legend->classify = View::Legend::ClassifyType(json["type"].toInt());

  auto metadataObj= json["metadata"].toObject();

  if(metadataObj.isEmpty())
  {
    QString errMsg = QObject::tr("Invalid View Legend JSON object: Empty metadada!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  for(auto it = metadataObj.begin(); it != metadataObj.end(); ++it)
  {
    legend->metadata.emplace(it.key().toStdString(), it.value().toString().toStdString());
  }

  for(auto color : json["colors"].toArray())
  {
    auto obj = color.toObject();

    View::Legend::Rule c;

    c.title = obj["title"].toString().toStdString();
    c.value = obj["value"].toString().toStdString();

    std::string colorOpacity = obj["color"].toString().toStdString();

    if(colorOpacity.size() == 9)
    {
      c.color = colorOpacity.substr(0, colorOpacity.size()-2);

      std::string hexOpacity = colorOpacity.substr(colorOpacity.size()-2);

      auto hex = std::strtoul(hexOpacity.c_str(), nullptr, 16);

      c.opacity = std::to_string(hex/255.0);
    }
    else
    {
      c.color = colorOpacity;
    }


    c.isDefault = obj["isDefault"].toBool();

    legend->rules.push_back(c);
  }

  return legend;
}

QJsonObject terrama2::services::view::core::toJson(View::Legend legend)
{
  QJsonObject obj;
  obj.insert("class", QString("ViewStyleLegend"));
  obj.insert("type", static_cast<int32_t>(legend.classify));

  QJsonObject metadataObj;

  for(const auto& metadata : legend.metadata)
  {
    metadataObj.insert(QString::fromStdString(metadata.first), QString::fromStdString(metadata.second));
  }

  obj.insert("metadata", metadataObj);

  QJsonArray rules;

  for(auto& rule : legend.rules)
  {
    rules.push_back(toJson(rule));
  }

  obj.insert("colors", rules);

  return obj;
}

QJsonObject terrama2::services::view::core::toJson(View::Legend::Rule rule)
{
  QJsonObject obj;

  obj.insert("title", QString::fromStdString(rule.title));
  obj.insert("value", QString::fromStdString(rule.value));

  auto longOpacity = std::stol(rule.opacity);

  std::stringstream stream;
  stream << std::hex << longOpacity;

  obj.insert("color", QString::fromStdString(rule.color));
  obj.insert("opacity", QString::fromStdString(stream.str()));
  obj.insert("isDefault", rule.isDefault);

  return obj;
}
