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
#include "../utility/SemanticsManager.hpp"
#include "../utility/Logger.hpp"
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
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("project_id")
       && json.contains("name")
       && json.contains("description")
       && json.contains("intent")
       && json.contains("uri")
       && json.contains("active")
       && json.contains("data_provider_type")))
  {
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::core::DataProvider* provider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr providerPtr(provider);

  provider->id = json["id"].toInt();
  provider->projectId = json["project_id"].toInt();
  provider->name = json["name"].toString().toStdString();
  provider->description = json["description"].toString().toStdString();
  provider->intent = static_cast<terrama2::core::DataProvider::DataProviderIntent>(json["intent"].toInt());
  provider->uri = json["uri"].toString().toStdString();
  provider->active = json["active"].toBool();
  provider->dataProviderType = json["data_provider_type"].toString().toStdString();

  return providerPtr;
}

terrama2::core::DataSeriesPtr terrama2::core::fromDataSeriesJson(QJsonObject json)
{
  if(json["class"].toString() != "DataSeries")
  {
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("data_provider_id")
       && json.contains("semantics")
       && json.contains("name")
       && json.contains("description")))
  {
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

  dataSeries->id = json["id"].toInt();
  dataSeries->dataProviderId = json["data_provider_id"].toInt();
  dataSeries->semantics = SemanticsManager::getInstance().getSemantics(json["semantics"].toString().toStdString());
  dataSeries->name = json["name"].toString().toStdString();
  dataSeries->description = json["description"].toString().toStdString();

  QJsonArray dataSetArray = json["datasets"].toArray();

  std::function<terrama2::core::DataSetPtr(QJsonObject)> createDataSet = nullptr;
  switch(dataSeries->semantics.dataSeriesType)
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
    case DataSeriesSemantics::STATIC:
      createDataSet = fromDataSetGridJson;
      break;
    default:
    {
      QString errMsg = QObject::tr("Invalid JSON object.\n Unknown DataSet type.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
      break;
    }
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
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("data_series_id")
       && json.contains("active")
       && json.contains("format")))
  {
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

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
  {
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  std::string wkt = json["position"].toString().toStdString();
  auto geom = std::shared_ptr<te::gm::Geometry>(te::gm::WKTReader::read(wkt.c_str()));
  auto point = std::dynamic_pointer_cast<te::gm::Point>(geom);

  if(!point.get())
  {
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  dataSet->position = point;

  return dataSetPtr;
}


terrama2::core::DataSetPtr terrama2::core::fromDataSetJson(QJsonObject json)
{
  terrama2::core::DataSet* dataSet = new terrama2::core::DataSet();
  terrama2::core::DataSetPtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

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
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
      && json.contains("frequency")
      && json.contains("frequency_unit")
      && json.contains("schedule")
      && json.contains("schedule_unit")
      && json.contains("schedule_retry")
      && json.contains("schedule_retry_unit")
      && json.contains("schedule_timeout")
      && json.contains("schedule_timeout_unit")))
  {
    QString errMsg = QObject::tr("Invalid JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }


  terrama2::core::Schedule schedule;
  schedule.id = json["id"].toInt();
  schedule.frequency = json["frequency"].toInt();
  schedule.frequencyUnit = json["frequency_unit"].toString().toStdString();
  schedule.schedule = json["schedule"].toInt();
  schedule.scheduleUnit = json["schedule_unit"].toString().toStdString();
  schedule.scheduleRetry = json["schedule_retry"].toInt();
  schedule.scheduleRetryUnit = json["schedule_retry_unit"].toString().toStdString();
  schedule.scheduleTimeout = json["schedule_timeout"].toInt();
  schedule.scheduleTimeoutUnit = json["schedule_timeout_unit"].toString().toStdString();

  return schedule;
}

QJsonObject terrama2::core::toJson(DataProviderPtr dataProviderPtr)
{
  QJsonObject obj;
  obj.insert("class", QString("DataProvider"));
  obj.insert("id", static_cast<qint64>(dataProviderPtr->id));
  obj.insert("project_id", static_cast<qint64>(dataProviderPtr->projectId));
  obj.insert("name", QString::fromStdString(dataProviderPtr->name));
  obj.insert("description", QString::fromStdString(dataProviderPtr->description));
  obj.insert("intent", static_cast<int>(dataProviderPtr->intent));
  obj.insert("uri", QString::fromStdString(dataProviderPtr->uri));
  obj.insert("active", dataProviderPtr->active);
  obj.insert("data_provider_type", QString::fromStdString(dataProviderPtr->dataProviderType));

  return obj;
}

QJsonObject terrama2::core::toJson(DataSeriesPtr dataSeriesPtr)
{
  QJsonObject obj;
  obj.insert("class", QString("DataSeries"));
  obj.insert("id", static_cast<qint64>(dataSeriesPtr->id));
  obj.insert("data_provider_id", static_cast<qint64>(dataSeriesPtr->dataProviderId));
  obj.insert("semantics", QString::fromStdString(dataSeriesPtr->semantics.name));
  obj.insert("name", QString::fromStdString(dataSeriesPtr->name));
  obj.insert("description", QString::fromStdString(dataSeriesPtr->description));

  QJsonArray array;
  for(auto dataSet : dataSeriesPtr->datasetList)
    array.push_back(terrama2::core::toJson(dataSet, dataSeriesPtr->semantics));
  obj.insert("datasets", array);

  return obj;
}

QJsonObject terrama2::core::toJson(DataSetPtr dataSetPtr, DataSeriesSemantics semantics)
{
  QJsonObject obj;
  obj.insert("class", QString("DataSet"));
  obj.insert("id", static_cast<qint64>(dataSetPtr->id));
  obj.insert("data_series_id", static_cast<qint64>(dataSetPtr->dataSeriesId));
  obj.insert("data_series_id", static_cast<qint64>(dataSetPtr->dataSeriesId));
  obj.insert("active", dataSetPtr->active);
  QJsonObject format;
  for(auto it = dataSetPtr->format.cbegin(); it != dataSetPtr->format.cend(); ++it)
  {
    format.insert(QString::fromStdString(it->first), QString::fromStdString(it->second));
  }
  obj.insert("format", format);

  switch(semantics.dataSeriesType)
  {
    case terrama2::core::DataSeriesSemantics::DCP :
    {
      auto dataSet = std::dynamic_pointer_cast<const DataSetDcp>(dataSetPtr);
      terrama2::core::addToJson(obj, dataSet);
      break;
    }
    case terrama2::core::DataSeriesSemantics::OCCURRENCE :
    {
      auto dataSet = std::dynamic_pointer_cast<const DataSetOccurrence>(dataSetPtr);
      terrama2::core::addToJson(obj, dataSet);
      break;
    }
    case terrama2::core::DataSeriesSemantics::GRID :
    {
      auto dataSet = std::dynamic_pointer_cast<const DataSetGrid>(dataSetPtr);
      terrama2::core::addToJson(obj, dataSet);
      break;
    }
    default:
      /* code */
      break;
  }

  return obj;
}

void terrama2::core::addToJson(QJsonObject& obj, DataSetDcpPtr dataSetPtr)
{
  obj.insert("position", QString::fromStdString(dataSetPtr->position->toString()));
}

void terrama2::core::addToJson(QJsonObject& obj, DataSetOccurrencePtr dataSetPtr)
{

}
void terrama2::core::addToJson(QJsonObject& obj, DataSetGridPtr dataSetPtr)
{

}

QJsonObject terrama2::core::toJson(Schedule schedule)
{
  QJsonObject obj;
  obj.insert("class", QString("Schedule"));
  obj.insert("id", static_cast<qint64>(schedule.id));
  obj.insert("frequency",static_cast<qint64>(schedule.frequency));
  obj.insert("frequency_unit", QString::fromStdString(schedule.frequencyUnit));

  obj.insert("schedule",static_cast<qint64>(schedule.schedule));
  obj.insert("schedule_unit",QString::fromStdString(schedule.scheduleUnit));
  obj.insert("schedule_retry",static_cast<qint64>(schedule.scheduleRetry));
  obj.insert("schedule_retry_unit", QString::fromStdString(schedule.scheduleRetryUnit));
  obj.insert("schedule_timeout",static_cast<qint64>(schedule.scheduleTimeout));
  obj.insert("schedule_timeout_unit", QString::fromStdString(schedule.scheduleTimeoutUnit));

  return obj;
}
