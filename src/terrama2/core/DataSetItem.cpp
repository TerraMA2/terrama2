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
  \file terrama2/core/DataSetItem.cpp

  \brief Metadata about a dataset item.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "DataSetItem.hpp"
#include "Utils.hpp"
#include "../Exception.hpp"

// Qt
#include <QObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>


terrama2::core::DataSetItem::DataSetItem(Kind k, uint64_t id, uint64_t datasetId)
  : kind_(k),
    id_(id),
    dataset_(datasetId),
    status_(INACTIVE),
    timezone_("00:00"),
    srid_(0)
{
}

terrama2::core::DataSetItem::~DataSetItem()
{
}

uint64_t terrama2::core::DataSetItem::id() const
{
  return id_;
}

void terrama2::core::DataSetItem::setId(uint64_t id)
{
  id_ = id;
  filter_.setDataSetItem(id);
}

terrama2::core::DataSetItem::Kind
terrama2::core::DataSetItem::kind() const
{
  return kind_;
}

void terrama2::core::DataSetItem::setKind(const Kind k)
{
  kind_ = k;
}

terrama2::core::DataSetItem::Status
terrama2::core::DataSetItem::status() const
{
  return status_;
}

void terrama2::core::DataSetItem::setStatus(const Status s)
{
  status_ = s;
}

const std::string&
terrama2::core::DataSetItem::mask() const
{
  return mask_;
}

void terrama2::core::DataSetItem::setMask(const std::string& m)
{
  mask_ = m;
}

const std::string&
terrama2::core::DataSetItem::timezone() const
{
  return timezone_;
}

void terrama2::core::DataSetItem::setTimezone(const std::string& tz)
{
  timezone_ = tz;
}

uint64_t terrama2::core::DataSetItem::dataset() const
{
  return dataset_;
}

void terrama2::core::DataSetItem::setDataSet(uint64_t id)
{
  dataset_ = id;
}

const terrama2::core::Filter&
terrama2::core::DataSetItem::filter() const
{
  return filter_;
}

void terrama2::core::DataSetItem::setFilter(const Filter& f)
{
  filter_ = f;
}

const std::map<std::string, std::string>&
terrama2::core::DataSetItem::metadata() const
{
  return metadata_;
}

std::map<std::string, std::string>&
terrama2::core::DataSetItem::metadata()
{
  return metadata_;
}

void terrama2::core::DataSetItem::setMetadata(const std::map<std::string, std::string>& metadata)
{
  metadata_ = metadata;
}

std::string terrama2::core::DataSetItem::path() const
{
  return path_;
}

void terrama2::core::DataSetItem::setPath(const std::string& path)
{
  path_ = path;
}

uint64_t terrama2::core::DataSetItem::srid() const
{
  return srid_;
}

void terrama2::core::DataSetItem::setSrid(const uint64_t srid)
{
  srid_ = srid;
}

terrama2::core::DataSetItem terrama2::core::DataSetItem::FromJson(const QJsonObject& json)
{
  if(! (json.contains("kind")
     && json.contains("id")
     && json.contains("dataset")
     && json.contains("status")
     && json.contains("mask")
     && json.contains("timezone")
     && json.contains("path")
     && json.contains("filter")
     && json.contains("metadata")
     && json.contains("srid")))
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  Kind kind = ToDataSetItemKind(json["kind"].toInt());
  uint64_t id = json["id"].toInt();
  uint64_t dataset = json["dataset"].toInt();
  DataSetItem item(kind, id, dataset);

  Status status = ToDataSetItemStatus(json["status"].toBool());
  item.setStatus(status);
  item.setMask(json["mask"].toString().toStdString());
  item.setTimezone(json["timezone"].toString().toStdString());
  item.setPath(json["path"].toString().toStdString());
  item.setFilter(Filter::FromJson(json["filter"].toObject()));

  QJsonObject metadataJson = json["metadata"].toObject();
  std::map<std::string, std::string> metadata;
  for(auto it = metadataJson.begin(); it != metadataJson.end(); ++it)
  {
    metadata[it.key().toStdString()] = it.value().toString().toStdString();
  }
  item.setMetadata(metadata);

  item.setSrid(json["srid"].toInt());

  return item;
}

QJsonObject terrama2::core::DataSetItem::toJson() const
{
  QJsonObject json;

  json["class"] = QString("DataSetItem");
  json["kind"] = QJsonValue((int)kind_);
  json["id"] = QJsonValue((int)id_);
  json["dataset"] = QJsonValue((int)dataset_);
  json["status"] = QJsonValue(ToBool(status_));
  json["mask"] = QString(mask_.c_str());
  json["timezone"] = QString(timezone_.c_str());
  json["path"] = QString(path_.c_str());
  json["filter"] = filter_.toJson();

  QJsonObject metadataJson;
  for(auto it = metadata_.begin(); it != metadata_.end(); ++it)
  {
    metadataJson[QString(it->first.c_str())] = QString(it->second.c_str());
  }
  json["metadata"] = metadataJson;
  json["srid"] = QJsonValue((int)srid_);

  return json;
}

bool terrama2::core::DataSetItem::operator==(const terrama2::core::DataSetItem& rhs)
{
  if(kind_ != rhs.kind_)
    return false;
  if(id_ != rhs.id_)
    return false;
  if(dataset_ != rhs.dataset_)
    return false;
  if(status_ != rhs.status_)
    return false;
  if(mask_ != rhs.mask_)
    return false;
  if(timezone_ != rhs.timezone_)
    return false;
  if(path_ != rhs.path_)
    return false;
  if(filter_ != rhs.filter_)
    return false;
  if(metadata_ != rhs.metadata_)
    return false;
  if(srid_ != rhs.srid_)
    return false;

  return true;
}

bool terrama2::core::DataSetItem::operator!=(const terrama2::core::DataSetItem& rhs)
{
  return !(*this == rhs);
}
