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

  if(!json.contains("id")
     || !json.contains("project_id")
     || !json.contains("service_instance_id")
     || !json.contains("active")
     || !json.contains("resolutionWidth")
     || !json.contains("resolutionHeight")
     || !json.contains("schedule")
     || !json.contains("srid")
     || !json.contains("data_series_list")
     || !json.contains("filters_per_data_series")
     || !json.contains("styles_per_data_series"))
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
  view->srid = static_cast<uint32_t>(json["srid"].toInt());

  view->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());

  {
    auto datasetSeriesArray = json["data_series_list"].toArray();
    auto it = datasetSeriesArray.begin();
    for(; it != datasetSeriesArray.end(); ++it)
    {
      auto obj = (*it).toObject();
      view->dataSeriesList.push_back(static_cast<uint32_t>(obj["dataset_series_id"].toInt()));
    }
  }

  {
    auto datasetSeriesArray = json["filters_per_data_series"].toArray();
    auto it = datasetSeriesArray.begin();
    for(; it != datasetSeriesArray.end(); ++it)
    {
      auto obj = (*it).toObject();
      view->filtersPerDataSeries.emplace(static_cast<uint32_t>(obj["dataset_series_id"].toInt()), terrama2::core::fromFilterJson(json["dataset_series_filter"].toObject()));
    }
  }

  {
    auto datasetSeriesArray = json["styles_per_data_series"].toArray();
    auto it = datasetSeriesArray.begin();
    for(; it != datasetSeriesArray.end(); ++it)
    {
      auto obj = (*it).toObject();
      view->stylesPerDataSeries.emplace(static_cast<uint32_t>(obj["dataset_series_id"].toInt()), fromViewStyleJson(json["dataset_series_view_style"].toObject()));
    }
  }

  return viewPtr;
}


QJsonObject terrama2::services::view::core::toJson(ViewPtr view)
{
  QJsonObject obj;
  obj.insert("class", QString("View"));
  obj.insert("id", static_cast<int32_t>(view->id));
  obj.insert("project_id", static_cast<int32_t>(view->projectId));
  obj.insert("service_instance_id", static_cast<int32_t>(view->serviceInstanceId));
  obj.insert("active", view->active);
  obj.insert("resolutionWidth", static_cast<int32_t>(view->resolutionWidth));
  obj.insert("resolutionHeight", static_cast<int32_t>(view->resolutionHeight));
  obj.insert("schedule", terrama2::core::toJson(view->schedule));
  obj.insert("srid", static_cast<int32_t>(view->srid));

  {
    QJsonArray array;
    for(auto it : view->dataSeriesList)
    {
      QJsonObject datasetSeries;
      datasetSeries.insert("dataset_series_id", static_cast<int32_t>(it));
      array.push_back(datasetSeries);
    }
    obj.insert("data_series_list", array);
  }

  {
    QJsonArray array;
    for(auto it : view->filtersPerDataSeries)
    {
      QJsonObject datasetSeriesAndFilter;
      datasetSeriesAndFilter.insert("dataset_series_id", static_cast<int32_t>(it.first));
      datasetSeriesAndFilter.insert("dataset_series_filter", terrama2::core::toJson(it.second));
      array.push_back(datasetSeriesAndFilter);
    }
    obj.insert("filters_per_data_series", array);
  }

  {
    QJsonArray array;
    for(auto it : view->stylesPerDataSeries)
    {
      QJsonObject datasetSeriesAndStyle;
      datasetSeriesAndStyle.insert("dataset_series_id", static_cast<int32_t>(it.first));
      datasetSeriesAndStyle.insert("dataset_series_view_style", toJson(it.second));

      array.push_back(datasetSeriesAndStyle);
    }
    obj.insert("view_style_per_data_series", array);
  }

  return obj;
}


QJsonObject terrama2::services::view::core::toJson(ViewStyle viewStyle)
{
  QJsonObject obj;
  obj.insert("class", QString("ViewStyle"));

  obj.insert("view_style_color", QString::fromStdString(viewStyle.color));
  obj.insert("view_style_opacity", QString::fromStdString(viewStyle.opacity));
  obj.insert("view_style_width", QString::fromStdString(viewStyle.width));
  obj.insert("view_style_dasharray",QString::fromStdString(viewStyle.dasharray));
  obj.insert("view_style_linecap", QString::fromStdString(viewStyle.linecap));
  obj.insert("view_style_linejoin",QString::fromStdString(viewStyle.linejoin));
  obj.insert("view_style_size", QString::fromStdString(viewStyle.size));
  obj.insert("view_style_rotation", QString::fromStdString(viewStyle.rotation));

  return obj;
}

terrama2::services::view::core::ViewStyle terrama2::services::view::core::fromViewStyleJson(QJsonObject json)
{
  if(json["class"].toString() != "ViewStyle")
  {
    QString errMsg = QObject::tr("Invalid View JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!json.contains("view_style_color")
     || !json.contains("view_style_opacity")
     || !json.contains("view_style_width")
     || !json.contains("view_style_dasharray")
     || !json.contains("view_style_linecap")
     || !json.contains("view_style_linejoin")
     || !json.contains("view_style_size")
     || !json.contains("view_style_rotation"))
  {
    QString errMsg = QObject::tr("Invalid View Style JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  ViewStyle viewStyle;

  viewStyle.color = json["view_style_color"].toString().toStdString();
  viewStyle.opacity = json["view_style_opacity"].toString().toStdString();
  viewStyle.width = json["view_style_width"].toString().toStdString();
  viewStyle.dasharray = json["view_style_dasharray"].toString().toStdString();
  viewStyle.linecap = json["view_style_dasharray"].toString().toStdString();
  viewStyle.linejoin = json["view_style_linejoin"].toString().toStdString();
  viewStyle.size = json["view_style_size"].toString().toStdString();
  viewStyle.rotation = json["view_style_rotation"].toString().toStdString();

  return viewStyle;
}
