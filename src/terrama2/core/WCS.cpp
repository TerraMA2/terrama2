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
  \file terrama2/core/WCS.hpp

  \brief Models the information of a WCS, it's a wrapper to extract data from the DataSetItem's metadata.

  \author Vinicius Campanha
*/

// TerraMA2
#include "WCS.hpp"

terrama2::core::WCS::WCS(DataSetItem& dataSetItem)
: dataSetItem_(dataSetItem)
{
}

void  terrama2::core::WCS::setRequest(const std::string request)
{
  auto metadata = dataSetItem_.metadata();
  metadata[REQUEST] = request;
  dataSetItem_.setMetadata(metadata);
}

std::string terrama2::core::WCS::request() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[REQUEST];
}

void  terrama2::core::WCS::setVersion(const std::string version)
{
  auto metadata = dataSetItem_.metadata();
  metadata[VERSION] = version;
  dataSetItem_.setMetadata(metadata);
}

std::string terrama2::core::WCS::version() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[VERSION];
}

std::string terrama2::core::WCS::coverageId() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[COVERAGEID];
}

void terrama2::core::WCS::setCoverageId(const std::string coverageId)
{
  auto metadata = dataSetItem_.metadata();
  metadata[COVERAGEID] = coverageId;
  dataSetItem_.setMetadata(metadata);
}

std::string terrama2::core::WCS::format() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[FORMAT];
}

void terrama2::core::WCS::setFormat(const std::string format)
{
  auto metadata = dataSetItem_.metadata();
  metadata[FORMAT] = format;
  dataSetItem_.setMetadata(metadata);
}


std::string terrama2::core::WCS::scaleFactor() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[SCALEFACTOR];
}


void terrama2::core::WCS::setScaleFactor(const std::string scaleFactor)
{
  auto metadata = dataSetItem_.metadata();
  metadata[SCALEFACTOR] = scaleFactor;
  dataSetItem_.setMetadata(metadata);
}


std::string terrama2::core::WCS::subset() const
{
  auto metadata = dataSetItem_.metadata();
  return metadata[SUBSET];
}


void terrama2::core::WCS::setSubset(const std::string subset)
{
  auto metadata = dataSetItem_.metadata();
  metadata[SUBSET] = subset;
  dataSetItem_.setMetadata(metadata);
}

terrama2::core::DataSetItem terrama2::core::WCS::dataSetItem() const
{
  return dataSetItem_;
}

terrama2::core::WCS::operator DataSetItem&()
{
  return dataSetItem_;
}
