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
#include "../../../core/Exception.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

// TerraLib
#include <terralib/geometry/WKTReader.h>
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
       && json.contains("schedule")
       && json.contains("service_instance_id")))
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

  analysis->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());
  analysis->active = json["active"].toBool();

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
  for(auto analysisDataSeries : analysis->analysisDataSeriesList)
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

  obj.insert("schedule", terrama2::core::toJson(analysis->schedule));
  obj.insert("active", analysis->active);


  return obj;
}

QJsonObject terrama2::services::analysis::core::toJson(OutputGridPtr outputGrid)
{
  QJsonObject obj;

  if(!outputGrid)
    return obj;

  obj.insert("class", QString("OutputGrid"));
  obj.insert("analysis_id", static_cast<qint32>(outputGrid->analysisId));
  obj.insert("interpolation_method", static_cast<qint32>(outputGrid->interpolationMethod));
  obj.insert("interpolation_dummy", QJsonValue(outputGrid->interpolationDummy));
  obj.insert("resolution_type", static_cast<qint32>(outputGrid->resolutionType));
  obj.insert("resolution_data_series_id", static_cast<qint32>(outputGrid->resolutionDataSeriesId));
  obj.insert("resolution_x",  QJsonValue(outputGrid->interpolationDummy));
  obj.insert("resolution_y",  QJsonValue(outputGrid->interpolationDummy));
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


terrama2::services::analysis::core::OutputGridPtr terrama2::services::analysis::core::fromOutputGrid(const QJsonObject& json)
{
  if(json["class"].toString() != "OutputGrid")
  {
    QString errMsg(QObject::tr("Invalid OutputGrid JSON object."));
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
    QString errMsg(QObject::tr("Invalid OutputGrid JSON object."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  OutputGrid* outputGrid = new OutputGrid;
  OutputGridPtr outputGridPtr(outputGrid);


  outputGrid->analysisId = json["analysis_id"].toInt();
  outputGrid->interpolationMethod = ToInterpolationMethod(json["interpolation_method"].toInt());
  outputGrid->interpolationDummy = json["script_language"].toDouble();
  outputGrid->resolutionType =  ToResolutionType(json["resolution_type"].toInt());
  outputGrid->resolutionDataSeriesId = json["resolution_data_series_id"].toInt();
  outputGrid->resolutionX = json["resolution_x"].toDouble();
  outputGrid->resolutionY = json["resolution_y"].toDouble();
  outputGrid->srid = json["active"].toInt();
  outputGrid->interestAreaDataSeriesId = json["area_of_interest_data_series_id"].toInt();
  outputGrid->interestAreaType = ToInterestAreaType(json["area_of_interest_type"].toInt());
  outputGrid->interestAreaBox.reset(te::gm::WKTReader::read(json["area_of_interest_box"].toString().toStdString().c_str()));

  return outputGridPtr;
}


