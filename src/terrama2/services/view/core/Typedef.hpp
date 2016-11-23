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
  \file terrama2/services/view/Typedef.hpp

  \brief Type definitions for View Service.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_TYPEDEF_HPP__
#define __TERRAMA2_SERVICES_VIEW_TYPEDEF_HPP__

// STL
#include <cstdint>
#include <unordered_map>

// TerraMA2
#include "../../../core/Shared.hpp"
#include "../../../core/Typedef.hpp"

//! Unique identifier of a View
typedef ProcessId ViewId;

// Forward declaration
namespace terrama2
{
  namespace core
  {
    struct DataSetSeries;
  }
}

typedef std::unordered_map< terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > SeriesMap;

//! MapsServerType code
typedef std::string MapsServerType;

#endif // __TERRAMA2_SERVICES_VIEW_TYPEDEF_HPP__
