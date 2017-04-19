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
  \file src/terrama2/services/alert/core/JSonUtils.cpp

  \brief

  \author Jano Simas
          Vinicius Campanha
*/

#include "JSonUtils.hpp"
#include "Report.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::services::alert::core::AlertPtr terrama2::services::alert::core::fromAlertJson(QJsonObject json, terrama2::core::DataManager* dataManager)
{
  if(json["class"].toString() != "Alert")
  {
    QString errMsg = QObject::tr("Invalid Alert JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("project_id")
       && json.contains("service_instance_id")
       && json.contains("data_series_id")
       && json.contains("risk_attribute")
       && json.contains("active")
       && json.contains("name")
       && json.contains("description")
       && json.contains("schedule")
//       && json.contains("filter")
       && json.contains("additional_data")
       && json.contains("risk")
       && json.contains("notifications")))
  {
    QString errMsg = QObject::tr("Invalid Alert JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }


  terrama2::services::alert::core::Alert* alert = new terrama2::services::alert::core::Alert();
  terrama2::services::alert::core::AlertPtr alertPtr(alert);

  alert->id = static_cast<uint32_t>(json["id"].toInt());
  alert->projectId = static_cast<uint32_t>(json["project_id"].toInt());
  alert->serviceInstanceId = static_cast<uint32_t>(json["service_instance_id"].toInt());
  alert->dataSeriesId = static_cast<uint32_t>(json["data_series_id"].toInt());
  alert->riskAttribute = json["risk_attribute"].toString().toStdString();
  alert->active = json["active"].toBool();
  alert->name = json["name"].toString().toStdString();
  alert->description = json["description"].toString().toStdString();
  //TODO: review alert filter
  alert->filter = terrama2::core::fromFilterJson(json["filter"].toObject(), dataManager);
  alert->filter.lastValues = std::make_shared<size_t>(6);

  auto addDataArray = json["additional_data"].toArray();
  for(const auto& value : addDataArray)
  {
    auto obj = value.toObject();
    auto id = static_cast<uint32_t>(obj["dataseries_id"].toInt());
    auto datasetid = static_cast<uint32_t>(obj["dataset_id"].toInt());
    auto referrerAttribute = obj["referrer_attribute"].toString().toStdString();
    auto referredAttribute = obj["referred_attribute"].toString().toStdString();

    std::vector<std::string> attributes;
    auto attributesArray = obj["attributes"].toArray();
    for(const auto& tempAttribute : attributesArray)
      attributes.push_back(tempAttribute.toString().toStdString());

    alert->additionalDataVector.push_back({id, datasetid, referrerAttribute, referredAttribute, attributes});
  }

  auto reportMetadata = json["report_metadata"].toObject();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::TITLE] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::TITLE)].toString().toStdString();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::AUTHOR] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::AUTHOR)].toString().toStdString();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::ABSTRACT] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::ABSTRACT)].toString().toStdString();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::DESCRIPTION] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::DESCRIPTION)].toString().toStdString();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::CONTACT] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::CONTACT)].toString().toStdString();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::COPYRIGHT] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::COPYRIGHT)].toString().toStdString();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::TIMESTAMP_FORMAT] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::TIMESTAMP_FORMAT)].toString().toStdString();
  alert->reportMetadata[terrama2::services::alert::core::ReportTags::LOGO_PATH] = reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::LOGO_PATH)].toString().toStdString();

  alert->risk = terrama2::core::fromRiskJson(json["risk"].toObject());

  auto recipientsArray = json["notifications"].toArray();
  for(const auto& tempRecipient : recipientsArray)
  {
    auto obj = tempRecipient.toObject();
    Notification recipient;
    recipient.includeReport = obj["include_report"].toString().toStdString();
    recipient.notifyOnChange = obj["notify_on_change"].toBool();
    recipient.simplifiedReport = obj["simplified_report"].toBool();
    recipient.notifyOnRiskLevel = static_cast<uint32_t>(obj["notify_on_risk_level"].toInt());

    for(const auto& target : obj["recipients"].toArray())
      recipient.targets.push_back(target.toString().toStdString());

    alert->notifications.push_back(recipient);
  }

  return alertPtr;
}

QJsonObject terrama2::services::alert::core::toJson(AlertPtr alert)
{
  QJsonObject obj;
  obj.insert("class", QString("Alert"));
  obj.insert("id", static_cast<int>(alert->id));
  obj.insert("project_id", static_cast<int>(alert->projectId));
  obj.insert("service_instance_id", static_cast<int>(alert->serviceInstanceId));
  obj.insert("active", alert->active);
  obj.insert("name", QString::fromStdString(alert->name));
  obj.insert("description", QString::fromStdString(alert->description));

  obj.insert("risk", toJson(alert->risk));
  obj.insert("schedule", toJson(alert->schedule));
  obj.insert("filter", toJson(alert->filter));

  QJsonArray additionalDataArray;
  for(const auto& data : alert->additionalDataVector)
  {
    QJsonObject tempObj;
    tempObj.insert("dataseries_id", static_cast<int>(data.dataSeriesId));
    obj.insert("dataset_id", static_cast<int>(data.dataSetId));
    QJsonArray attributesArray;
    for(const auto& attribute : data.attributes)
      attributesArray.append(QString::fromStdString(attribute));
    obj.insert("attributes", attributesArray);

    additionalDataArray.append(obj);
  }
  obj.insert("additional_data",additionalDataArray);

  QJsonObject reportMetadata;
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::TITLE)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::TITLE));
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::AUTHOR)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::AUTHOR));
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::ABSTRACT)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::ABSTRACT));
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::DESCRIPTION)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::DESCRIPTION));
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::CONTACT)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::CONTACT));
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::COPYRIGHT)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::COPYRIGHT));
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::TIMESTAMP_FORMAT)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::TIMESTAMP_FORMAT));
  reportMetadata[QString::fromStdString(terrama2::services::alert::core::ReportTags::LOGO_PATH)] = QString::fromStdString(alert->reportMetadata.at(terrama2::services::alert::core::ReportTags::LOGO_PATH));
  obj.insert("report_metadata", reportMetadata);

  QJsonArray recipientsArray;
  for(const auto& recipient : alert->notifications)
  {
    QJsonObject obj;
    obj["include_report"] = QString::fromStdString(recipient.includeReport);
    obj["notify_on_change"] = recipient.notifyOnChange;
    obj["notify_on_risk_level"] = static_cast<int>(recipient.notifyOnRiskLevel);
    obj["simplified_report"] = recipient.simplifiedReport;

    QJsonArray targets;
    for(const auto& target : recipient.targets)
      targets.append(QString::fromStdString(target));

    obj["targets"] = targets;

    recipientsArray.push_back(obj);
  }

  obj.insert("recipients", recipientsArray);

  return obj;
}
