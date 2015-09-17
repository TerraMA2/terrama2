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
  \file terrama2/core/Data.cpp

  \brief Metadata about a given data.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Data.hpp"
#include "DataSet.hpp"

terrama2::core::Data::Data(DataSetPtr dataSet, Kind kind)
  : id_(0),
    status_(INACTIVE),
    dataSet_(dataSet),
    kind_(kind)
{

}

terrama2::core::Data::~Data()
{

}

uint64_t terrama2::core::Data::id() const
{
  return id_;
}

void terrama2::core::Data::setId(uint64_t id)
{
  id_ = id;
}

terrama2::core::Data::Kind terrama2::core::Data::kind() const
{
  return kind_;
}

void terrama2::core::Data::setKind(const terrama2::core::Data::Kind& kind)
{
  kind_ = kind;
}

terrama2::core::Data::Status terrama2::core::Data::status() const
{
  return status_;
}

void terrama2::core::Data::setStatus(const terrama2::core::Data::Status& status)
{
  status_ = status;
}

std::string terrama2::core::Data::mask() const
{
  return mask_;
}

void terrama2::core::Data::setMask(const std::string& mask)
{
  mask_ = mask;
}

std::string terrama2::core::Data::timezone() const
{
  return timezone_;
}

void terrama2::core::Data::setTimezone(const std::string& timezone)
{
  timezone_ = timezone;
}

terrama2::core::DataSetPtr terrama2::core::Data::dataSet() const
{
  return dataSet_;
}
