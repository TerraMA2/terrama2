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
  \file terrama2/core/PCD.hpp

  \brief Models the information of a PCD, it's a wrapper to extract data from the DataSetItem's metadata.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "PCD.hpp"

// TerraLib
#include <terralib/geometry/Point.h>

terrama2::core::PCD::PCD(DataSetItem& dataSetItem)
: dataSetItem_(dataSetItem)
{
}

void  terrama2::core::PCD::setUnit(const std::string unit)
{
  auto metadata = dataSetItem_.metadata();
  metadata[UNIT] = unit;
  dataSetItem_.setMetadata(metadata);
}

std::string terrama2::core::PCD::unit() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[UNIT];
}

void  terrama2::core::PCD::setPrefix(const std::string prefix)
{
  auto metadata = dataSetItem_.metadata();
  metadata[PREFIX] = prefix;
  dataSetItem_.setMetadata(metadata);
}

std::string terrama2::core::PCD::prefix() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[PREFIX];
}

te::gm::Point* terrama2::core::PCD::location() const
{
  try
  {
    auto metadata = dataSetItem_.metadata();
    double latitude = atof(metadata[LATITUDE].c_str());
    double longitude = atof(metadata[LONGITUDE].c_str());
    te::gm::Point* p = new te::gm::Point(longitude, latitude, dataSetItem_.srid(), 0);
    return p;
  }
  catch(...)
  {
    return nullptr;
  }
}

void terrama2::core::PCD::setLocation(te::gm::Point* location)
{
  std::string latitude;
  std::string longitude;

  if(location != nullptr)
  {
    longitude = std::to_string(location->getX());
    latitude = std::to_string(location->getY());
  }

  auto metadata = dataSetItem_.metadata();
  metadata[LONGITUDE] = longitude;
  metadata[LATITUDE] = latitude;
  dataSetItem_.setMetadata(metadata);
}

terrama2::core::DataSetItem terrama2::core::PCD::dataSetItem() const
{
  return dataSetItem_;
}

terrama2::core::PCD::operator DataSetItem&()
{
  return dataSetItem_;
}
