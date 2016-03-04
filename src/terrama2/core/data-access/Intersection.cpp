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
  \file terrama2/core/Filter.cpp

  \brief Intersection information of a dataset.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Intersection.hpp"
#include "../Exception.hpp"

// Qt
#include <QObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>

terrama2::core::Intersection::Intersection(uint64_t dataSetId)
: dataset_(dataSetId)
{

}

terrama2::core::Intersection::~Intersection()
{

}

uint64_t terrama2::core::Intersection::dataset() const
{
  return dataset_;
}

void terrama2::core::Intersection::setDataSet(uint64_t id)
{
  dataset_ = id;
}

std::map<std::string, std::vector<std::string> > terrama2::core::Intersection::attributeMap() const
{
  return attributeMap_;
}

void terrama2::core::Intersection::setAttributeMap(const std::map<std::string, std::vector<std::string> >& attributeMap)
{
  attributeMap_ = attributeMap;
}

std::map<uint64_t, std::string> terrama2::core::Intersection::bandMap() const
{
  return bandMap_;
}

void terrama2::core::Intersection::setBandMap(const std::map<uint64_t, std::string >& bandMap)
{
  bandMap_ = bandMap;
}

terrama2::core::Intersection terrama2::core::Intersection::FromJson(const QJsonObject& json)
{
  if(! (json.contains("dataset") && json.contains("attributeMap") && json.contains("bandMap")))
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  Intersection intersection(json["dataset"].toInt());

  QJsonObject attributeMapJson = json["attributeMap"].toObject();
  std::map<std::string, std::vector<std::string> > attributeMap;
  for(auto it = attributeMapJson.begin(); it != attributeMapJson.end(); ++it)
  {
    QJsonArray attrListJson = it.value().toArray();

    std::vector<std::string> attrList;
    for (const QJsonValue & value: attrListJson)
    {
      attrList.push_back(value.toString().toStdString());
    }

    attributeMap[it.key().toStdString()] = attrList;
  }
  intersection.setAttributeMap(attributeMap);

  QJsonObject bandMapJson = json["bandMap"].toObject();
  std::map<uint64_t, std::string> bandMap;
  for(auto it = bandMapJson.begin(); it != bandMapJson.end(); ++it)
  {
    uint64_t key = it.key().toInt();
    bandMap[key] = it.value().toString().toStdString();
  }
  intersection.setBandMap(bandMap);

  return intersection;
}

QJsonObject terrama2::core::Intersection::toJson() const
{
  QJsonObject json;

  json["class"] = QString("Intersection");
  json["dataset"] = QJsonValue((int)dataset_);

  QJsonObject attributeMapJson;
  for(auto it = attributeMap_.begin(); it != attributeMap_.end(); ++it)
  {
    QJsonArray attrList;
    for(auto attr : it->second)
    {
      attrList.append(QString(attr.c_str()));
    }
    attributeMapJson[QString(it->first.c_str())] = attrList;
  }
  json["attributeMap"] = attributeMapJson;

  QJsonObject bandMapJson;
  for(auto it = bandMap_.begin(); it != bandMap_.end(); ++it)
  {
    bandMapJson[QString(std::to_string((int)it->first).c_str())] = QString(it->second.c_str());
  }
  json["bandMap"] = bandMapJson;

  return json;

}

bool terrama2::core::Intersection::operator==(const terrama2::core::Intersection& rhs)
{
  if(dataset_ != rhs.dataset_)
    return false;
  if(attributeMap_ != rhs.attributeMap_)
    return false;
  if(bandMap_ != rhs.bandMap_)
    return false;

  return true;
}

bool terrama2::core::Intersection::operator!=(const terrama2::core::Intersection& rhs)
{
  return !(*this == rhs);
}
