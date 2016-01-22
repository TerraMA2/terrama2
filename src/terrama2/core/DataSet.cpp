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
  \file terrama2/core/DataSet.cpp

  \brief Models the information of given dataset dataset from a data provider that should be collected by TerraMA2.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/


// TerraMA2
#include "DataSet.hpp"
#include "Utils.hpp"
#include "../Exception.hpp"

// Qt
#include <QObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>

terrama2::core::DataSet::DataSet(const std::string& name, Kind kind, uint64_t id, uint64_t providerId)
  : name_(name),
    kind_(kind),
    id_(id),
    provider_(providerId),
    status_(INACTIVE),
    dataFrequency_(0, 0, 0),
    schedule_(0, 0, 0),
    scheduleRetry_(0, 0, 0),
    scheduleTimeout_(0, 0, 0)
{
}

terrama2::core::DataSet::~DataSet()
{
}

uint64_t terrama2::core::DataSet::id() const
{
  return id_;
}

void terrama2::core::DataSet::setId(uint64_t id)
{
  id_ = id;

  for(auto& item : datasetItems_)
    item.setDataSet(id);

  intersection_.setDataSet(id);
}

const std::string&
terrama2::core::DataSet::name() const
{
  return name_;
}

void terrama2::core::DataSet::setName(const std::string& name)
{
  name_ = name;
}

const std::string&
terrama2::core::DataSet::description() const
{
  return description_;
}

void terrama2::core::DataSet::setDescription(const std::string& d)
{
  description_ = d;
}

terrama2::core::DataSet::Kind terrama2::core::DataSet::kind() const
{
  return kind_;
}

void terrama2::core::DataSet::setKind(const Kind k)
{
  kind_ = k;
}

terrama2::core::DataSet::Status terrama2::core::DataSet::status() const
{
  return status_;
}

void terrama2::core::DataSet::setStatus(const Status s)
{
  status_ = s;
}

uint64_t terrama2::core::DataSet::provider() const
{
  return provider_;
}

void
terrama2::core::DataSet::setProvider(uint64_t id)
{
  provider_ = id;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::dataFrequency() const
{
  return dataFrequency_;
}

void terrama2::core::DataSet::setDataFrequency(const te::dt::TimeDuration& t)
{
  dataFrequency_ = t;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::schedule() const
{
  return schedule_;
}

void terrama2::core::DataSet::setSchedule(const te::dt::TimeDuration& t)
{
  schedule_ = t;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::scheduleRetry() const
{
  return scheduleRetry_;
}

void terrama2::core::DataSet::setScheduleRetry(const te::dt::TimeDuration& t)
{
  scheduleRetry_ = t;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::scheduleTimeout() const
{
  return scheduleTimeout_;
}

void terrama2::core::DataSet::setScheduleTimeout(const te::dt::TimeDuration& t)
{
  scheduleTimeout_ = t;
}

const std::map<std::string, std::string>&
terrama2::core::DataSet::metadata() const
{
  return metadata_;
}

void terrama2::core::DataSet::setMetadata(const std::map<std::string, std::string>& m)
{
  metadata_ = m;
}

std::vector<terrama2::core::DataSetItem>&
terrama2::core::DataSet::dataSetItems()
{
  return datasetItems_;
}

const std::vector<terrama2::core::DataSetItem> &terrama2::core::DataSet::dataSetItems() const
{
  return datasetItems_;
}

void
terrama2::core::DataSet::add(DataSetItem& d)
{
  datasetItems_.push_back(d);
  d.setDataSet(id());
}

void terrama2::core::DataSet::removeDataSetItem(uint64_t id)
{
  datasetItems_.erase(std::remove_if(datasetItems_.begin(),
                                     datasetItems_.end(),
                                     [&id](const DataSetItem& item){ return (item.id() == id) ? true : false; }),
                      datasetItems_.end());
}

void terrama2::core::DataSet::setIntersection(const Intersection& intersection)
{
  intersection_ = intersection;
  intersection_.setDataSet(id_);
}

terrama2::core::Intersection terrama2::core::DataSet::intersection() const
{
  return intersection_;
}

void terrama2::core::DataSet::update(DataSetItem& dataSetItem)
{

  for(unsigned int i = 0; i < datasetItems_.size(); i++)
  {
    if(datasetItems_[i].id() == dataSetItem.id())
    {
      datasetItems_[i] = dataSetItem;
    }
  }
}

terrama2::core::DataSet terrama2::core::DataSet::FromJson(QJsonObject json)
{
  if(!(json.contains("name")
     && json.contains("kind")
     && json.contains("id")
     && json.contains("provider")
     && json.contains("description")
     && json.contains("status")
     && json.contains("dataFrequency")
     && json.contains("schedule")
     && json.contains("scheduleRetry")
     && json.contains("scheduleTimeout")
     && json.contains("metadata")
     && json.contains("datasetItems")
     && json.contains("intersection")))
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  DataSet dataset;
  dataset.setName(json["name"].toString().toStdString());
  dataset.setKind(ToDataSetKind(json["kind"].toInt()));
  dataset.setId(json["id"].toInt());
  dataset.setProvider(json["provider"].toInt());
  dataset.setDescription(json["description"].toString().toStdString());
  dataset.setStatus(ToDataSetStatus(json["status"].toBool()));

  boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string(json["dataFrequency"].toString().toStdString()));
  boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string(json["schedule"].toString().toStdString()));
  boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string(json["scheduleRetry"].toString().toStdString()));
  boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string(json["scheduleTimeout"].toString().toStdString()));

  dataset.setDataFrequency(te::dt::TimeDuration(dataFrequency));
  dataset.setSchedule(te::dt::TimeDuration(schedule));
  dataset.setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
  dataset.setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

  QJsonObject metadataJson = json["metadata"].toObject();
  std::map<std::string, std::string> metadata;
  for(auto it = metadataJson.begin(); it != metadataJson.end(); ++it)
  {
    metadata[it.key().toStdString()] = it.value().toString().toStdString();
  }
  dataset.setMetadata(metadata);

  QJsonArray datasetItemsJson =  json["datasetItems"].toArray();
  for (const QJsonValue & value: datasetItemsJson)
  {
    auto datasetItem = DataSetItem::FromJson(value.toObject());
    dataset.add(datasetItem);
  }

  dataset.setIntersection(Intersection::FromJson(json["intersection"].toObject()));

  return dataset;
}

QJsonObject terrama2::core::DataSet::toJson()
{
  QJsonObject json;

  json["name"] = QString(name_.c_str());
  json["kind"] = QJsonValue((int)kind_);
  json["id"] = QJsonValue((int)id_);
  json["provider"] = QJsonValue((int)provider_);
  json["description"] = QString(description_.c_str());
  json["status"] = QJsonValue(ToBool(status_));
  json["dataFrequency"] = QString(dataFrequency_.toString().c_str());
  json["schedule"] = QString(schedule_.toString().c_str());
  json["scheduleRetry"] = QString(scheduleRetry_.toString().c_str());
  json["scheduleTimeout"] = QString(scheduleTimeout_.toString().c_str());

  QJsonObject metadataJson;
  for(auto it = metadata_.begin(); it != metadata_.end(); ++it)
  {
    metadataJson[QString(it->first.c_str())] = QString(it->second.c_str());
  }
  json["metadata"] = metadataJson;

  QJsonArray itemsJson;
  for(auto item : datasetItems_)
  {
    itemsJson.append(item.toJson());
  }
  json["datasetItems"] = itemsJson;
  json["intersection"] = intersection_.toJson();

  return json;
}

bool terrama2::core::DataSet::operator==(const DataSet& rhs)
{
  if(name_ != rhs.name_)
    return false;
  if(kind_ != rhs.kind_)
    return false;
  if(id_ != rhs.id_)
    return false;
  if(provider_ != rhs.provider_)
    return false;
  if(description_ != rhs.description_)
    return false;
  if(status_ != rhs.status_)
    return false;
  if(dataFrequency_ != rhs.dataFrequency_)
    return false;
  if(schedule_ != rhs.schedule_)
    return false;
  if(scheduleRetry_ != rhs.scheduleRetry_)
    return false;
  if(scheduleTimeout_ != rhs.scheduleTimeout_)
    return false;
  if(metadata_ != rhs.metadata_)
    return false;
  if(datasetItems_.size() != rhs.datasetItems_.size())
    return false;
  else
  {
    for (int i = 0; i < datasetItems_.size(); ++i)
    {
      if(datasetItems_[i] != rhs.datasetItems_[i])
        return false;
    }
  }
  if(intersection_ != rhs.intersection_)
    return false;

  return true;
}

bool terrama2::core::DataSet::operator!=(const DataSet& rhs)
{
  return !(*this == rhs);
}
