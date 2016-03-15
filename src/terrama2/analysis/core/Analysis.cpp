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
  \file terrama2/analysis/core/Analysis.cpp

  \brief Model class for the analysis configuration.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Analysis.hpp"
#include "../../core/DataSet.hpp"
#include "Utils.hpp"
#include "../../Exception.hpp"

// QT
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>


void terrama2::analysis::core::Analysis::setId(uint64_t id)
{
	id_ = id;
}

uint64_t terrama2::analysis::core::Analysis::id() const
{
	return id_;
}

void terrama2::analysis::core::Analysis::setAdditionalDataList(const std::vector<terrama2::core::DataSet>& additionalDataList)
{
  additionalDataList_ = additionalDataList;
}

std::vector<terrama2::core::DataSet> terrama2::analysis::core::Analysis::additionalDataList() const
{
  return additionalDataList_;
}

void terrama2::analysis::core::Analysis::setMonitoredObject(const terrama2::core::DataSet& monitoredObject)
{
	monitoredObject_ = monitoredObject;
}

terrama2::core::DataSet terrama2::analysis::core::Analysis::monitoredObject() const
{
  return monitoredObject_;
}

void terrama2::analysis::core::Analysis::setScriptLanguage(const ScriptLanguage scriptLanguage)
{
	scriptLanguage_ = scriptLanguage;
}

void terrama2::analysis::core::Analysis::setDCP(const terrama2::core::DataSet& pcd, Influence influence)
{
  pcd_ = pcd;
  mapInfluence_[pcd.id()] = influence;
}

terrama2::core::DataSet terrama2::analysis::core::Analysis::DCP() const
{
  return pcd_;
}

terrama2::analysis::core::Analysis::ScriptLanguage terrama2::analysis::core::Analysis::scriptLanguage() const
{
	return scriptLanguage_;
}

void terrama2::analysis::core::Analysis::setScript(const std::string& script)
{
	script_ = script;
}

std::string terrama2::analysis::core::Analysis::script() const
{
	return script_;
}

void terrama2::analysis::core::Analysis::setDescription(const std::string& description)
{
	description_ = description;
}

std::string terrama2::analysis::core::Analysis::description() const
{
	return description_;
}

void terrama2::analysis::core::Analysis::setType(const Type type)
{
	type_ = type;
}

terrama2::analysis::core::Analysis::Type terrama2::analysis::core::Analysis::type() const
{
	return type_;
}

terrama2::analysis::core::Analysis::Influence terrama2::analysis::core::Analysis::influence(uint64_t datasetId) const
{
  auto it = mapInfluence_.find(datasetId);

  if(it == mapInfluence_.end())
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Could not find an influence configuration for this dataset."));
  return it->second;
}

void terrama2::analysis::core::Analysis::setInfluence(uint64_t datasetId, Influence influence)
{
  mapInfluence_[datasetId] = influence;
}

terrama2::analysis::core::Analysis terrama2::analysis::core::Analysis::FromJson(const QJsonObject& json)
{
  if(!(json.contains("id")
     && json.contains("staticDataList")
     && json.contains("monitoredObject")
     && json.contains("scriptLanguage")
     && json.contains("script")
     && json.contains("description")
     && json.contains("type")))
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  Analysis analysis;
  analysis.setId(json["id"].toInt());
  analysis.setMonitoredObject(terrama2::core::DataSet::FromJson(json["monitoredObject"].toObject()));
  analysis.setScriptLanguage(ToScriptLanguage(json["scriptLanguage"].toInt()));
  analysis.setScript(json["script"].toString().toStdString());
  analysis.setDescription(json["description"].toString().toStdString());

  QJsonArray staticDataListJson =  json["staticDataList"].toArray();
  std::vector<terrama2::core::DataSet> staticDataList;
  for (const QJsonValue & value: staticDataListJson)
  {
    auto dataset = terrama2::core::DataSet::FromJson(value.toObject());
    staticDataList.push_back(dataset);
  }
  analysis.setAdditionalDataList(staticDataList);

  analysis.setType(ToType(json["type"].toInt()));

  return analysis;
}

QJsonObject terrama2::analysis::core::Analysis::toJson() const
{
  QJsonObject json;

  json["id"] = QJsonValue((int)id_);

  QJsonArray additionMapListJson;
  for(auto additionalMap : additionalDataList_)
  {
    additionMapListJson.append(additionalMap.toJson());
  }
  json["additionalMapList"] = additionMapListJson;

  json["monitoredObject"] = monitoredObject_.toJson();
  json["scriptLanguage"] = QJsonValue((int)scriptLanguage_);
  json["script"] = QString(script_.c_str());
  json["description"] = QString(description_.c_str());
  json["type"] = QJsonValue((int)type_);

  return json;
}

