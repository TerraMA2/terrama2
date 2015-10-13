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
#include "DataSet.hpp"
#include "Filter.hpp"

terrama2::core::DataSetItem::DataSetItem(const uint64_t id, Kind k, const DataSet* d)
  : id_(id),
    kind_(k),
    dataset_(d),
    status_(INACTIVE)
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

const terrama2::core::DataSet* 
terrama2::core::DataSetItem::dataset() const
{
  return dataset_;
}

const terrama2::core::Filter*
terrama2::core::DataSetItem::filter() const
{
  return filter_.get();
}

void terrama2::core::DataSetItem::setFilter(std::unique_ptr<Filter> f)
{
  filter_ = std::move(f);
}

const std::map<std::string, std::string>&
terrama2::core::DataSetItem::storageMetadata() const
{
  return storageMetadata_;
}

std::map<std::string, std::string>&
terrama2::core::DataSetItem::storageMetadata()
{
  return storageMetadata_;
}


void terrama2::core::DataSetItem::setStorageMetadata(const std::map<std::string, std::string>& sm)
{
  storageMetadata_ = sm;
}


