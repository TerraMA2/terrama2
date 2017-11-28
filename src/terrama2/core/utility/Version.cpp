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
  \file terrama2/core/Version.cpp

  \brief Utility class for system versioning.

  \author Gilberto Ribeiro de Queiroz
*/

// TerraMA2
#include "Version.hpp"
#include "../../Version.hpp"

// STL
#include <cassert>

int terrama2::core::Version::majorNumber()
{
  return TERRAMA2_VERSION_MAJOR;
}

int terrama2::core::Version::minorNumber()
{
  return TERRAMA2_VERSION_MINOR;
}

int terrama2::core::Version::patchNumber()
{
  return TERRAMA2_VERSION_PATCH;
}

std::string terrama2::core::Version::buildDate()
{
  assert(__DATE__ " " __TIME__);
  return std::string(__DATE__ " " __TIME__);
}

std::string terrama2::core::Version::asString()
{
  return TERRAMA2_VERSION_STRING;
}

int terrama2::core::Version::asInt()
{
  return TERRAMA2_VERSION;
}
