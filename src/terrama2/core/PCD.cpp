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
  \file terrama2/core/PCD.cpp

  \brief Model for the PCD information.

  \author Paulo R. M. Oliveira
*/

#include "PCD.hpp"

// TerraLib
#include <terralib/geometry/Geometry.h>

terrama2::core::PCD::PCD(Kind kind, uint64_t id, uint64_t datasetId)
: DataSetItem(kind, id, datasetId)
{
}

te::gm::Geometry* terrama2::core::PCD::location() const
{
  return location_.get();
}

void terrama2::core::PCD::setLocation(std::unique_ptr<te::gm::Geometry> location)
{
  location_ = std::move(location);
}
