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
  \file terrama2/ws/collector/core/Data.cpp

  \brief Data...

  \author Paulo R. M. Oliveira
*/

#include "Data.hpp"


terrama2::ws::collector::core::Data::Data(const std::string &mask)
  : mask_(mask)
{

}

terrama2::ws::collector::core::Data::~Data()
{

}

std::string terrama2::ws::collector::core::Data::getMask() const
{
  return mask_;
}


void terrama2::ws::collector::core::Data::setMask(const std::string &mask)
{
  mask_ = mask;
}

void terrama2::ws::collector::core::Data::store(const std::string& uri)
{
  
}
