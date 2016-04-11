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
  \file terrama2/core/utility/JSonUtils.hpp

  \brief

  \author Jano Simas
*/
#include "../data-model/DataProvider.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../data-model/DataSet.hpp"
#include "../data-model/DataSetDcp.hpp"
#include "../data-model/DataSetGrid.hpp"
#include "../data-model/DataSetOccurrence.hpp"
#include "../Exception.hpp"

#include "JSonUtils.hpp"

//Terralib
#include <terralib/geometry/WKTReader.h>

//Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::core::DataProviderPtr terrama2::core::fromDataProviderJson(QJsonObject json)
{
  if(json["class"].toString() != "DataProvider")
  {
    throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));
  }

  if(!(json.contains("id")
      && json.contains("project_id")
      && json.contains("name")
      && json.contains("description")
      && json.contains("intent")
      && json.contains("uri")
      && json.contains("active")))
     throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));

   terrama2::core::DataProvider* provider = new terrama2::core::DataProvider();
   terrama2::core::DataProviderPtr providerPtr(provider);

   provider->id = json["id"].toInt();
   provider->projectId = json["project_id"].toInt();
   provider->name = json["name"].toString().toStdString();
   provider->description = json["description"].toString().toStdString();
   provider->intent = static_cast<terrama2::core::DataProvider::DataProviderIntent>(json["intent"].toInt());
   provider->uri = json["uri"].toString().toStdString();
   provider->active = json["active"].toBool();

   return providerPtr;
}

terrama2::core::DataSeriesPtr terrama2::core::fromDataSeriesJson(QJsonObject json, SemanticsManager* semanticsManager)
{
  if(json["class"].toString() != "DataSeries")
  {
    throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));
  }

  if(!(json.contains("id")
      && json.contains("data_provider_id")
      && json.contains("semantics")
      && json.contains("name")
      && json.contains("description")))
     throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));

   terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
   terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

   dataSeries->id = json["id"].toInt();
   dataSeries->dataProviderId = json["data_provider_id"].toInt();
   dataSeries->semantics = semanticsManager->getSemantics(json["semantics"].toString().toStdString());
   dataSeries->name = json["name"].toString().toStdString();
   dataSeries->description = json["description"].toString().toStdString();

   QJsonArray dataSetArray = json["datasets"].toArray();

   std::function<terrama2::core::DataSetPtr(QJsonObject)> createDataSet = nullptr;
   switch (dataSeries->semantics.macroType)
   {
   case DataSeriesSemantics::DCP:
     createDataSet = fromDataSetDcpJson;
     break;
   case DataSeriesSemantics::OCCURRENCE:
     createDataSet = fromDataSetOccurrenceJson;
     break;
   case DataSeriesSemantics::GRID:
     createDataSet = fromDataSetGridJson;
     break;
   default:
     throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object.\n Unknown DataSet type."));
     break;
   }

   for(auto json : dataSetArray)
   {
     if(json.isObject())
      dataSeries->datasetList.push_back(createDataSet(json.toObject()));
    else
      throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));
   }

   return dataSeriesPtr;
}

void terrama2::core::addBaseDataSetData(QJsonObject json, terrama2::core::DataSet* dataSet)
{
  if(json["class"].toString() != "DataSet")
  {
    throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));
  }

  if(!(json.contains("id")
      && json.contains("data_series_id")
      && json.contains("active")
      && json.contains("format")))
     throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  dataSet->id = json["id"].toInt();
  dataSet->dataSeriesId = json["data_series_id"].toInt();
  dataSet->active = json["active"].toBool();

  auto formatObj= json["format"].toObject();
  for(auto it = formatObj.begin(); it != formatObj.end(); ++it)
  {
    dataSet->format.emplace(it.key().toStdString(), it.value().toString().toStdString());
  }

}

terrama2::core::DataSetPtr terrama2::core::fromDataSetDcpJson(QJsonObject json)
{
  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  terrama2::core::DataSetDcpPtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

  if(!json.contains("position"))
     throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  std::string wkt = json["position"].toString().toStdString();
  te::gm::Geometry* geom = te::gm::WKTReader::read(wkt.c_str());
  te::gm::Point* point = dynamic_cast<te::gm::Point*>(geom);

  if(!point)
  {
    throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));
  }

  dataSet->position = point;

  return dataSetPtr;
}

terrama2::core::DataSetPtr terrama2::core::fromDataSetOccurrenceJson(QJsonObject json)
{
  terrama2::core::DataSetOccurrence* dataSet = new terrama2::core::DataSetOccurrence();
  terrama2::core::DataSetOccurrencePtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

  return dataSetPtr;
}

terrama2::core::DataSetPtr terrama2::core::fromDataSetGridJson(QJsonObject json)
{
  terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
  terrama2::core::DataSetGridPtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

  return dataSetPtr;
}

terrama2::core::Schedule terrama2::core::fromScheduleJson(QJsonObject json)
{
  if(json["class"].toString() != "Schedule")
  {
    throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));
  }

  if(!(json.contains("id")
      && json.contains("frequency")
      && json.contains("frequency_unit")
      && json.contains("schedule")
      && json.contains("schedule_retry")
      && json.contains("schedule_retry_unit")
      && json.contains("schedule_timeout")
      && json.contains("schedule_timeout_unit")))
     throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  terrama2::core::Schedule schedule;
  schedule.id = json["id"].toInt();
  schedule.frequency = json["frequency"].toInt();
  schedule.frequencyUnit = json["frequency_unit"].toString().toStdString();
  //TODO: schedule time duration
  // schedule.schedule = json["schedule"].toInt();
  schedule.scheduleRetry = json["schedule_retry"].toInt();
  schedule.scheduleRetryUnit = json["schedule_retry_unit"].toString().toStdString();
  schedule.scheduleTimeout = json["schedule_timeout"].toInt();
  schedule.scheduleTimeoutUnit = json["schedule_timeout_unit"].toString().toStdString();

  return schedule;
}
