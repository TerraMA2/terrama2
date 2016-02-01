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

  \brief Filter information of a given dataset item.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

// TerraMA2
#include "Filter.hpp"
#include "DataSetItem.hpp"
#include "Utils.hpp"
#include "../Exception.hpp"

// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/Polygon.h>
#include <terralib/geometry/WKTReader.h>

// Qt
#include <QObject>
#include <QString>

// Boost
#include <boost/date_time/local_time/local_time_types.hpp>

terrama2::core::Filter::Filter(uint64_t dataSetItemId)
  : datasetItem_(dataSetItemId),
    expressionType_(NONE_TYPE),
    staticDataId_(0)
{

}

terrama2::core::Filter::~Filter()
{

}

uint64_t terrama2::core::Filter::datasetItem() const
{
  return datasetItem_;
}

void terrama2::core::Filter::setDataSetItem(uint64_t datasetItem)
{
  datasetItem_ = datasetItem;
}

const te::dt::TimeInstantTZ* terrama2::core::Filter::discardBefore() const
{
  return discardBefore_.get();
}

void terrama2::core::Filter::setDiscardBefore(std::unique_ptr<te::dt::TimeInstantTZ> t)
{
  discardBefore_ = std::move(t);
}

const te::dt::TimeInstantTZ* terrama2::core::Filter::discardAfter() const
{
  return discardAfter_.get();
}

void terrama2::core::Filter::setDiscardAfter(std::unique_ptr<te::dt::TimeInstantTZ> t)
{
  discardAfter_ = std::move(t);
}

const te::gm::Polygon* terrama2::core::Filter::geometry() const
{
  return geometry_.get();
}

void terrama2::core::Filter::setGeometry(std::unique_ptr<te::gm::Polygon> geom)
{
  geometry_ = std::move(geom);
}

const double* terrama2::core::Filter::value() const
{
  return value_.get();
}

void terrama2::core::Filter::setValue(std::unique_ptr<double> v)
{
  value_ = std::move(v);
}

terrama2::core::Filter::ExpressionType
terrama2::core::Filter::expressionType() const
{
  return expressionType_;
}

void terrama2::core::Filter::setExpressionType(const ExpressionType t)
{
  expressionType_ = t;
}

const std::string&
terrama2::core::Filter::bandFilter() const
{
  return bandFilter_;
}

void terrama2::core::Filter::setBandFilter(const std::string& f)
{
  bandFilter_ = f;
}

uint64_t terrama2::core::Filter::staticDataId() const
{
  return staticDataId_;
}

void terrama2::core::Filter::setStaticDataId(const uint64_t staticDataId)
{
  staticDataId_ = staticDataId;
}

terrama2::core::Filter& terrama2::core::Filter::operator=(const terrama2::core::Filter& rhs)
{
  if( this != &rhs )
  {
    datasetItem_ = rhs.datasetItem_;

    if(rhs.discardBefore_ == nullptr)
      discardBefore_.reset(nullptr);
    else
    {
      te::dt::TimeInstantTZ* discardBefore = rhs.discardBefore_.get();
      discardBefore_.reset(new te::dt::TimeInstantTZ(*discardBefore));
    }

    if(rhs.discardAfter_ == nullptr)
      discardAfter_.reset(nullptr);
    else
    {
      te::dt::TimeInstantTZ* discardAfter = rhs.discardAfter_.get();
      discardAfter_.reset(new te::dt::TimeInstantTZ(*discardAfter));
    }

    if(rhs.geometry_ == nullptr)
      geometry_.reset(nullptr);
    else
    {
      te::gm::Polygon* geom = dynamic_cast<te::gm::Polygon*>(rhs.geometry_.get());
      if(geom != nullptr)
        geometry_.reset(new te::gm::Polygon(*geom));
      else
        geometry_.reset(nullptr);
    }

    if(rhs.value_ == nullptr)
      value_.reset(nullptr);
    else
    {
      value_.reset(new double(*rhs.value_));
    }

    expressionType_ = rhs.expressionType_;
    staticDataId_ = rhs.staticDataId_;
    bandFilter_ = rhs.bandFilter_;
  }
  return *this;
}

terrama2::core::Filter::Filter(const terrama2::core::Filter& rhs)
{
  datasetItem_ = rhs.datasetItem_;

  if(rhs.discardBefore_ == nullptr)
    discardBefore_.reset(nullptr);
  else
  {
    te::dt::TimeInstantTZ* discardBefore = rhs.discardBefore_.get();
    if(discardBefore != nullptr)
      discardBefore_.reset(new te::dt::TimeInstantTZ(*discardBefore));
    else
      discardBefore_.reset(nullptr);
  }

  if(rhs.discardAfter_ == nullptr)
    discardAfter_.reset(nullptr);
  else
  {
    te::dt::TimeInstantTZ* discardAfter = rhs.discardAfter_.get();
    if(discardAfter != nullptr)
      discardAfter_.reset(new te::dt::TimeInstantTZ(*discardAfter));
    else
      discardAfter_.reset(nullptr);
  }

  if(rhs.geometry_ == nullptr)
    geometry_.reset(nullptr);
  else
  {
    te::gm::Polygon* geom = dynamic_cast<te::gm::Polygon*>(rhs.geometry_.get());
    if(geom != nullptr)
      geometry_.reset(new te::gm::Polygon(*geom));
    else
      geometry_.reset(nullptr);
  }

  if(rhs.value_ == nullptr)
    value_.reset(nullptr);
  else
  {
    value_.reset(new double(*rhs.value_));
  }

  expressionType_ = rhs.expressionType_;
  staticDataId_ = rhs.staticDataId_;
  bandFilter_ = rhs.bandFilter_;
}


terrama2::core::Filter terrama2::core::Filter::FromJson(const QJsonObject& json)
{
  if(! (json.contains("datasetItem")
     && json.contains("discardBefore")
     && json.contains("discardAfter")
     && json.contains("geometry")
     && json.contains("value")
     && json.contains("bandFilter")
     && json.contains("staticDataId") ))
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid JSON object."));

  Filter filter(json["datasetItem"].toInt());

  if(!json["discardBefore"].isNull())
  {
    QJsonObject discardBeforeJson = json["discardBefore"].toObject();
    std::string date = discardBeforeJson["date"].toString().toStdString();
    std::string time = discardBeforeJson["time"].toString().toStdString();
    std::string timezone = discardBeforeJson["timezone"].toString().toStdString();

    boost::gregorian::date boostDate(boost::gregorian::from_string(date));
    boost::posix_time::time_duration boostTime(boost::posix_time::duration_from_string(time));
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));

    boost::local_time::local_date_time boostLocalTime(boostDate, boostTime, zone, true);
    std::unique_ptr< te::dt::TimeInstantTZ > discardBefore(new te::dt::TimeInstantTZ(boostLocalTime));

    filter.setDiscardBefore(std::move(discardBefore));
  }


  if(!json["discardAfter"].isNull())
  {
    QJsonObject discardAfterJson = json["discardAfter"].toObject();
    std::string date = discardAfterJson["date"].toString().toStdString();
    std::string time = discardAfterJson["time"].toString().toStdString();
    std::string timezone = discardAfterJson["timezone"].toString().toStdString();

    boost::gregorian::date boostDate(boost::gregorian::from_string(date));
    boost::posix_time::time_duration boostTime(boost::posix_time::duration_from_string(time));
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));

    boost::local_time::local_date_time boostLocalTime(boostDate, boostTime, zone, true);
    std::unique_ptr< te::dt::TimeInstantTZ > discardAfter(new te::dt::TimeInstantTZ(boostLocalTime));

    filter.setDiscardAfter(std::move(discardAfter));
  }

  if(!json["value"].isNull())
  {
    std::unique_ptr<double> value(new double(json["value"].toDouble()));
    filter.setValue(std::move(value));
  }

  int expressionType = json["expressionType"].toInt();
  filter.setExpressionType(ToFilterExpressionType(expressionType));
  filter.setStaticDataId(json["staticDataId"].toInt());
  filter.setBandFilter(json["bandFilter"].toString().toStdString());

  if(!json["geometry"].isNull())
  {
    std::unique_ptr<te::gm::Polygon> geometry(static_cast<te::gm::Polygon*>(te::gm::WKTReader::read(json["geometry"].toString().toStdString().c_str())));
    filter.setGeometry(std::move(geometry));
  }

  return filter;
}

QJsonObject terrama2::core::Filter::toJson() const
{
  QJsonObject json;

  json["class"] = QString("Filter");
  json["datasetItem"] = QJsonValue((int)datasetItem_);

  if(discardBefore_.get())
  {
    QJsonObject discardBeforeJson;
    boost::local_time::local_date_time boostLocalDate = discardBefore_->getTimeInstantTZ();
    std::string date = boost::gregorian::to_iso_extended_string(boostLocalDate.date());
    std::string time = boost::posix_time::to_simple_string(boostLocalDate.local_time().time_of_day());
    std::string timezone = boostLocalDate.zone_as_posix_string();

    discardBeforeJson["date"] = QString(date.c_str());
    discardBeforeJson["time"] = QString(time.c_str());
    discardBeforeJson["timezone"] = QString(timezone.c_str());
    json["discardBefore"] = discardBeforeJson;
  }
  else
    json["discardBefore"] = QJsonValue();

  if(discardAfter_.get())
  {
    QJsonObject discardAfterJson;
    boost::local_time::local_date_time boostLocalDate = discardAfter_->getTimeInstantTZ();
    std::string date = boost::gregorian::to_iso_extended_string(boostLocalDate.date());
    std::string time = boost::posix_time::to_simple_string(boostLocalDate.local_time().time_of_day());
    std::string timezone = boostLocalDate.zone_as_posix_string();

    discardAfterJson["date"] = QString(date.c_str());
    discardAfterJson["time"] = QString(time.c_str());
    discardAfterJson["timezone"] = QString(timezone.c_str());
    json["discardAfter"] = discardAfterJson;
  }
  else
    json["discardAfter"] = QJsonValue();

  if(value_.get())
    json["value"] = QJsonValue(*value_.get());
  else
    json["value"] = QJsonValue();

  json["expressionType"] = QJsonValue((int)expressionType_);
  json["staticDataId"] = QJsonValue((int)staticDataId_);
  json["bandFilter"] = QString(bandFilter_.c_str());


  if(geometry_.get())
  {
    std::string wkt = geometry_->toString();
    std::replace( wkt.begin(), wkt.end(), '\n', ',');
    json["geometry"] = QString(wkt.c_str());
  }
  else
    json["geometry"] = QJsonValue();

  return json;

}

bool terrama2::core::Filter::operator==(const terrama2::core::Filter& rhs)
{
  if(datasetItem_ != rhs.datasetItem_)
    return false;
  if(discardBefore_ != nullptr && rhs.discardBefore_ != nullptr && *discardBefore_ != *rhs.discardBefore_)
    return false;
  if(discardAfter_ != nullptr && rhs.discardAfter_ != nullptr && *discardAfter_ != *rhs.discardAfter_)
    return false;
  if(geometry_ != nullptr && rhs.geometry_ != nullptr && !geometry_->equals(rhs.geometry()))
    return false;
  if(value_ != nullptr && rhs.value_ != nullptr &&  *value_ != *rhs.value_)
    return false;
  if(expressionType_ != rhs.expressionType_)
    return false;
  if(bandFilter_ != rhs.bandFilter_)
    return false;
  if(staticDataId_ != rhs.staticDataId_)
    return false;

  return true;
}

bool terrama2::core::Filter::operator!=(const terrama2::core::Filter& rhs)
{
  return !(*this == rhs);
}
