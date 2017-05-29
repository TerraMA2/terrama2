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
  \file src/terrama2/services/analysis/core/JSonUtils.cpp

  \brief Utility functions to encode and decode analysis objects to/from JSON.

  \author Paulo R. M. Oliveira
*/

#include "JSonUtils.hpp"

#include "Utils.hpp"
#include "../../../../core/Exception.hpp"
#include "../../../../core/utility/JSonUtils.hpp"
#include "../../../../core/utility/Utils.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/TimeUtils.hpp"

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

// TerraLib
#include <terralib/geometry/Utils.h>

terrama2::services::analysis::core::AnalysisPtr terrama2::services::analysis::core::fromAnalysisJson(const QJsonObject& json)
{
  if(json["class"].toString() != "Analysis")
  {
    QString errMsg(QObject::tr("Invalid Analysis JSON object."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("project_id")
       && json.contains("script_language")
       && json.contains("script")
       && json.contains("type")
       && json.contains("name")
       && json.contains("description")
       && json.contains("active")
       && json.contains("output_dataseries_id")
       && json.contains("metadata")
       && json.contains("analysis_dataseries_list")
       && json.contains("service_instance_id")
       && json.contains("output_grid")))
  {
    QString errMsg(QObject::tr("Invalid Analysis JSON object."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  Analysis* analysis = new Analysis;
  AnalysisPtr analysisPtr(analysis);


  analysis->id = json["id"].toInt();
  analysis->projectId = json["project_id"].toInt();
  analysis->scriptLanguage = ToScriptLanguage(json["script_language"].toInt());
  analysis->script = json["script"].toString().toStdString();
  analysis->type = ToAnalysisType(json["type"].toInt());
  analysis->name = json["name"].toString().toStdString();
  analysis->description = json["description"].toString().toStdString();
  analysis->active = json["active"].toBool();
  analysis->outputDataSeriesId = json["output_dataseries_id"].toInt();
  analysis->outputDataSetId = json["output_dataset_id"].toInt();
  analysis->serviceInstanceId = json["service_instance_id"].toInt();


  QJsonObject metadataJson = json["metadata"].toObject();
  std::map<std::string, std::string> metadata;
  for(auto it = metadataJson.begin(); it != metadataJson.end(); ++it)
  {
    metadata[it.key().toStdString()] = it.value().toString().toStdString();
  }
  analysis->metadata = metadata;

  auto analysisDataSeriesArray = json["analysis_dataseries_list"].toArray();
  for (int i = 0; i < analysisDataSeriesArray.size(); ++i)
  {
    auto analysisDataSeriesJson = analysisDataSeriesArray[i].toObject();
    if(analysisDataSeriesJson["class"].toString() != "AnalysisDataSeries")
    {
      QString errMsg(QObject::tr("Invalid AnalysisDataSeries JSON object."));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
    }

    if(!(analysisDataSeriesJson.contains("id") && analysisDataSeriesJson.contains("data_series_id") && analysisDataSeriesJson.contains("type") &&
         analysisDataSeriesJson.contains("alias") && analysisDataSeriesJson.contains("metadata")))
    {
      QString errMsg(QObject::tr("Invalid AnalysisDataSeries JSON object."));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
    }


    AnalysisDataSeries analysisDataSeries;

    analysisDataSeries.id = analysisDataSeriesJson["id"].toInt();
    analysisDataSeries.dataSeriesId = analysisDataSeriesJson["data_series_id"].toInt();
    analysisDataSeries.type = ToAnalysisDataSeriesType(analysisDataSeriesJson["type"].toInt());
    analysisDataSeries.alias = analysisDataSeriesJson["alias"].toString().toStdString();

    QJsonObject metadataJson = analysisDataSeriesJson["metadata"].toObject();
    std::map<std::string, std::string> metadata;
    for(auto it = metadataJson.begin(); it != metadataJson.end(); ++it)
    {
      metadata[it.key().toStdString()] = it.value().toString().toStdString();
    }
    analysisDataSeries.metadata = metadata;

    analysis->analysisDataSeriesList.push_back(analysisDataSeries);

  }

  analysis->active = json["active"].toBool();
  analysis->outputGridPtr = fromAnalysisOutputGrid(json["output_grid"].toObject());

  if(json.contains("schedule") && !json["schedule"].isNull())
    analysis->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());

  if(json.contains("reprocessing_historical_data") && !json["reprocessing_historical_data"].isNull())
    analysis->reprocessingHistoricalData = fromAnalysisReprocessingHistoricalData(json["reprocessing_historical_data"].toObject());

  return analysisPtr;
}

QJsonObject terrama2::services::analysis::core::toJson(AnalysisPtr analysis)
{
  QJsonObject obj;

  if(!analysis)
    return obj;

  obj.insert("class", QString("Analysis"));
  obj.insert("id", static_cast<qint32>(analysis->id));
  obj.insert("project_id", static_cast<qint32>(analysis->projectId));
  obj.insert("script", QString(analysis->script.c_str()));
  obj.insert("script_language", static_cast<qint32>(analysis->scriptLanguage));
  obj.insert("type", static_cast<qint32>(analysis->type));
  obj.insert("name", QString(analysis->name.c_str()));
  obj.insert("description", QString(analysis->description.c_str()));
  obj.insert("output_dataseries_id", static_cast<qint32>(analysis->outputDataSeriesId));
  obj.insert("output_dataset_id", static_cast<qint32>(analysis->outputDataSetId));
  obj.insert("service_instance_id", static_cast<qint32>(analysis->serviceInstanceId));

  // Analysis metadata
  QJsonObject metadataJson;
  for(auto it = analysis->metadata.begin(); it != analysis->metadata.end(); ++it)
  {
    metadataJson[QString(it->first.c_str())] = QString(it->second.c_str());
  }
  obj.insert("metadata", metadataJson);

  // Analysis DataSeries
  QJsonArray analysisDataSeriesList;
  for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    QJsonObject analysisDataSeriesObj;
    analysisDataSeriesObj.insert("class", QString("AnalysisDataSeries"));
    analysisDataSeriesObj.insert("id", static_cast<qint32>(analysisDataSeries.id));
    analysisDataSeriesObj.insert("data_series_id", static_cast<qint32>(analysisDataSeries.dataSeriesId));
    analysisDataSeriesObj.insert("type", static_cast<qint32>(analysisDataSeries.type));

    // Analysis metadata
    QJsonObject metadataAnalysisDataSeriesJson;
    for(auto it = analysisDataSeries.metadata.begin(); it != analysisDataSeries.metadata.end(); ++it)
    {
      metadataAnalysisDataSeriesJson[QString(it->first.c_str())] = QString(it->second.c_str());
    }
    analysisDataSeriesObj.insert("metadata", metadataAnalysisDataSeriesJson);
    analysisDataSeriesObj.insert("alias", QString::fromStdString(analysisDataSeries.alias));

    analysisDataSeriesList.append(analysisDataSeriesObj);
  }
  obj.insert("analysis_dataseries_list", analysisDataSeriesList);


  obj.insert("schedule", analysis->schedule.valid() ? terrama2::core::toJson(analysis->schedule) : QJsonObject());
  obj.insert("active", analysis->active);
  obj.insert("output_grid", toJson(analysis->outputGridPtr));
  obj.insert("reprocessing_historical_data", toJson(analysis->reprocessingHistoricalData));


  return obj;
}

QJsonObject terrama2::services::analysis::core::toJson(AnalysisOutputGridPtr outputGrid)
{
  QJsonObject obj;

  if(!outputGrid)
    return obj;

  obj.insert("class", QString("AnalysisOutputGrid"));
  obj.insert("analysis_id", static_cast<qint32>(outputGrid->analysisId));
  obj.insert("interpolation_method", static_cast<qint32>(outputGrid->interpolationMethod));
  obj.insert("interpolation_dummy", QJsonValue(outputGrid->interpolationDummy));
  obj.insert("resolution_type", static_cast<qint32>(outputGrid->resolutionType));
  obj.insert("resolution_data_series_id", static_cast<qint32>(outputGrid->resolutionDataSeriesId));
  obj.insert("resolution_x",  QJsonValue(outputGrid->resolutionX));
  obj.insert("resolution_y",  QJsonValue(outputGrid->resolutionY));
  obj.insert("srid", static_cast<qint32>(outputGrid->resolutionType));
  obj.insert("area_of_interest_data_series_id", static_cast<qint32>(outputGrid->interestAreaDataSeriesId));
  obj.insert("area_of_interest_type", static_cast<qint32>(outputGrid->interestAreaType));
  std::string strBox;
  if(outputGrid->interestAreaBox)
  {
    strBox = outputGrid->interestAreaBox->toString();
  }

  obj.insert("area_of_interest_box", QString::fromStdString(strBox));


  return obj;
}


terrama2::services::analysis::core::AnalysisOutputGridPtr terrama2::services::analysis::core::fromAnalysisOutputGrid(const QJsonObject& json)
{
  if(json.isEmpty())
  {
    return std::make_shared<terrama2::services::analysis::core::AnalysisOutputGrid>();
  }

  if(json["class"].toString() != "AnalysisOutputGrid")
  {
    QString errMsg(QObject::tr("Invalid AnalysisOutputGrid JSON object."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("analysis_id")
       && json.contains("interpolation_method")
       && json.contains("interpolation_dummy")
       && json.contains("resolution_type")
       && json.contains("resolution_data_series_id")
       && json.contains("resolution_x")
       && json.contains("resolution_y")
       && json.contains("srid")
       && json.contains("area_of_interest_data_series_id")
       && json.contains("area_of_interest_type")
       && json.contains("area_of_interest_box")))
  {
    QString errMsg(QObject::tr("Invalid AnalysisOutputGrid JSON object."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  AnalysisOutputGrid* outputGrid = new AnalysisOutputGrid;
  AnalysisOutputGridPtr outputGridPtr(outputGrid);


  outputGrid->analysisId = json["analysis_id"].toInt();
  outputGrid->interpolationMethod = ToInterpolationMethod(json["interpolation_method"].toInt());
  outputGrid->interpolationDummy = json["script_language"].toDouble();
  outputGrid->resolutionType =  ToResolutionType(json["resolution_type"].toInt());
  if(!json["resolution_data_series_id"].isNull())
    outputGrid->resolutionDataSeriesId = json["resolution_data_series_id"].toInt();
  if(!json["resolution_x"].isNull())
    outputGrid->resolutionX = json["resolution_x"].toDouble();

  if(!json["resolution_y"].isNull())
    outputGrid->resolutionY = json["resolution_y"].toDouble();
  if(!json["srid"].isNull())
    outputGrid->srid = json["srid"].toInt();
  if(!json["area_of_interest_data_series_id"].isNull())
    outputGrid->interestAreaDataSeriesId = json["area_of_interest_data_series_id"].toInt();
  outputGrid->interestAreaType = ToInterestAreaType(json["area_of_interest_type"].toInt());
  if(!json["area_of_interest_box"].isNull())
  {
    std::string ewkt = json["area_of_interest_box"].toString().toStdString();
    outputGrid->interestAreaBox = terrama2::core::ewktToGeom(ewkt);
  }

  return outputGridPtr;
}

terrama2::services::analysis::core::ReprocessingHistoricalDataPtr terrama2::services::analysis::core::fromAnalysisReprocessingHistoricalData(
    const QJsonObject& json)
{
  if(json.isEmpty())
  {
    return terrama2::services::analysis::core::ReprocessingHistoricalDataPtr();
  }

  if(json["class"].toString() != "ReprocessingHistoricalData")
  {
    QString errMsg(QObject::tr("Invalid ReprocessingHistoricalData JSON object."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("start_date")
       && json.contains("end_date")))
  {
    QString errMsg(QObject::tr("Invalid ReprocessingHistoricalData JSON object."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  ReprocessingHistoricalData* reprocessingHistoricalData = new ReprocessingHistoricalData;
  ReprocessingHistoricalDataPtr reprocessingHistoricalDataPtr(reprocessingHistoricalData);

  if(!json.value("start_date").isNull())
  {
    std::string startDate = json["start_date"].toString().toStdString();
    reprocessingHistoricalData->startDate = terrama2::core::TimeUtils::stringToTimestamp(startDate, terrama2::core::TimeUtils::webgui_timefacet);
  }

  if(!json.value("end_date").isNull())
  {
    std::string endDate = json["end_date"].toString().toStdString();
    reprocessingHistoricalData->endDate = terrama2::core::TimeUtils::stringToTimestamp(endDate, terrama2::core::TimeUtils::webgui_timefacet);
  }

  return reprocessingHistoricalDataPtr;
}

QJsonObject terrama2::services::analysis::core::toJson(terrama2::services::analysis::core::ReprocessingHistoricalDataPtr
                                                       reprocessingHistoricalDataPtr)
{
  QJsonObject obj;

  if(!reprocessingHistoricalDataPtr)
    return obj;

  obj.insert("class", QString("ReprocessingHistoricalData"));
  if(reprocessingHistoricalDataPtr->startDate.get())
  {
    std::string startDate = terrama2::core::TimeUtils::boostLocalTimeToString(reprocessingHistoricalDataPtr->startDate->getTimeInstantTZ(), terrama2::core::TimeUtils::webgui_timefacet);
    obj.insert("start_date", QString::fromStdString(startDate));
  }
  if(reprocessingHistoricalDataPtr->endDate.get())
  {
    std::string endDate = terrama2::core::TimeUtils::boostLocalTimeToString(reprocessingHistoricalDataPtr->endDate->getTimeInstantTZ(), terrama2::core::TimeUtils::webgui_timefacet);
    obj.insert("end_date", QString::fromStdString(endDate));
  }

  return obj;
}

QJsonObject terrama2::services::analysis::core::toJson(terrama2::services::analysis::core::ValidateResult result)
{
  QJsonObject obj;
  obj.insert("id", static_cast<qint32>(result.analysisId));
  obj.insert("valid", result.valid);

  QJsonArray messages;
  for(const std::string& message : result.messages)
  {
    messages.append(QJsonValue(QString::fromStdString(message)));
  }
  obj.insert("messages", messages);

  return obj;
}
