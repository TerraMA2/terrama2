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

  \brief Metadata about a given dataset item.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "DataSetItem.hpp"
#include "DataSet.hpp"

terrama2::core::DataSetItem::DataSetItem(DataSetPtr dataSet, Kind kind, const uint64_t id)
  : id_(id),
    status_(INACTIVE),
    dataSet_(dataSet),
    kind_(kind)
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

terrama2::core::DataSetItem::Kind terrama2::core::DataSetItem::kind() const
{
  return kind_;
}

void terrama2::core::DataSetItem::setKind(const terrama2::core::DataSetItem::Kind& kind)
{
  kind_ = kind;
}

terrama2::core::DataSetItem::Status terrama2::core::DataSetItem::status() const
{
  return status_;
}

void terrama2::core::DataSetItem::setStatus(const terrama2::core::DataSetItem::Status& status)
{
  status_ = status;
}

std::string terrama2::core::DataSetItem::mask() const
{
  return mask_;
}

void terrama2::core::DataSetItem::setMask(const std::string& mask)
{
  mask_ = mask;
}

std::string terrama2::core::DataSetItem::timezone() const
{
  return timezone_;
}

void terrama2::core::DataSetItem::setTimezone(const std::string& timezone)
{
  timezone_ = timezone;
}

terrama2::core::DataSetPtr terrama2::core::DataSetItem::dataSet() const
{
  return dataSet_;
}

terrama2::core::FilterPtr terrama2::core::DataSetItem::filter() const
{
  return filter_;
}

void terrama2::core::DataSetItem::setFilter(terrama2::core::FilterPtr filter)
{
  filter_ = filter;
}
