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
  \file terrama2/ws/collector/core/Collector.cpp

  \brief Class that represents a collector

  \author Gilberto Ribeiro de Queiroz, Paulo R. M. Oliveira
 */

// TerraMA2
#include "Collector.hpp"
#include "StorageStrategy.hpp"

// STL
#include <memory>


terrama2::ws::collector::core::Collector::Collector(const int& id, const std::string& name, const std::string& description, const std::string& type, const Format& format, const int& updateFreqMinutes, const std::string& prefix, const int& srid, const std::string& mask, const std::string& unit, const std::string& timeZone, const QJsonObject& dynamicMetadata)
: id_(id), 
name_(name), 
description_(description), 
type_(type), 
format_(format), 
updateFreqMinutes_(updateFreqMinutes), 
prefix_(prefix), 
srid_(srid), 
mask_(mask), 
unit_(unit), 
timeZone_(timeZone), 
dynamicMetadata_(dynamicMetadata)
{

}

terrama2::ws::collector::core::Collector::Collector()
{

}

terrama2::ws::collector::core::Collector::~Collector()
{

}

int terrama2::ws::collector::core::Collector::getId() const
{
  return id_;
}

void terrama2::ws::collector::core::Collector::setId(const int &id)
{
  id_ = id;
}

std::string terrama2::ws::collector::core::Collector::getName() const
{
  return name_;
}

void terrama2::ws::collector::core::Collector::setName(const std::string &name)
{
  name_ = name;
}

std::string terrama2::ws::collector::core::Collector::getDescription() const
{
  return description_;
}

void terrama2::ws::collector::core::Collector::setDescription(const std::string &description)
{
  description_ = description;
}

terrama2::ws::collector::core::Format terrama2::ws::collector::core::Collector::getFormat() const
{
  return format_;
}

void terrama2::ws::collector::core::Collector::setFormat(const terrama2::ws::collector::core::Format &format)
{
  format_ = format;
}

int terrama2::ws::collector::core::Collector::getUpdateFreqMinutes() const
{
  return updateFreqMinutes_;
}

void terrama2::ws::collector::core::Collector::setUpdateFreqMinutes(const int &updateFreqMinutes)
{
  updateFreqMinutes_ = updateFreqMinutes;
}

std::string terrama2::ws::collector::core::Collector::getPrefix() const
{
  return prefix_;
}

void terrama2::ws::collector::core::Collector::setPrefix(const std::string &prefix)
{
  prefix_ = prefix;
}

int terrama2::ws::collector::core::Collector::getSrid() const
{
  return srid_;
}

void terrama2::ws::collector::core::Collector::setSrid(const int &srid)
{
  srid_ = srid;
}

std::string terrama2::ws::collector::core::Collector::getMask() const
{
  return mask_;
}

void terrama2::ws::collector::core::Collector::setMask(const std::string &mask)
{
  mask_ = mask;
}

std::string terrama2::ws::collector::core::Collector::getUnit() const
{
  return unit_;
}

void terrama2::ws::collector::core::Collector::setUnit(const std::string &unit)
{
  unit_ = unit;
}

std::string terrama2::ws::collector::core::Collector::getTimeZone() const
{
  return timeZone_;
}

void terrama2::ws::collector::core::Collector::setTimeZone(const std::string &timeZone)
{
  timeZone_ = timeZone;
}

QJsonObject terrama2::ws::collector::core::Collector::getDynamicMetadata()
{
  return dynamicMetadata_;
}

void terrama2::ws::collector::core::Collector::setDynamicMetadata(const QJsonObject &dynamicMetadata)
{
  dynamicMetadata_ = dynamicMetadata;
}

std::shared_ptr<terrama2::ws::collector::core::StorageStrategy> terrama2::ws::collector::core::Collector::getStorageStrategy() const
{
  return storageStrategy_;
}

void terrama2::ws::collector::core::Collector::setStorageStrategy(std::shared_ptr<StorageStrategy> storageStrategy)
{
  storageStrategy_ = storageStrategy;
}
