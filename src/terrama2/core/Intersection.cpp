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

void terrama2::core::Intersection::setAttributeMap(std::map<std::string, std::vector<std::string> >& attributeMap)
{
  attributeMap_ = attributeMap;
}

std::map<uint64_t, std::string> terrama2::core::Intersection::bandMap() const
{
  return bandMap_;
}

void terrama2::core::Intersection::setBandMap(std::map<uint64_t, std::string >& bandMap)
{
  bandMap_ = bandMap;
}
