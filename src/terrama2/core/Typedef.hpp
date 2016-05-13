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
  \file terrama2/core/Typedef.hpp

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_TYPEDEF_HPP__
#define __TERRAMA2_CORE_TYPEDEF_HPP__

#include <cstdint>
#include <string>

//! Unique identifier of the ServiceInstance
typedef uint32_t ServiceInstanceId;
//! Unique identifier for Project
typedef uint32_t ProjectId;
//! Unique identifier for Schedule
typedef uint32_t ScheduleId;
//! Unique identifier for DataSet
typedef uint32_t DataSetId;
//! Unique identifier for DataSeries
typedef uint32_t DataSeriesId;
//! Unique identifier for DataProvider
typedef uint32_t DataProviderId;

typedef uint32_t ProcessId;
typedef uint32_t RegisterId;

typedef std::string DataProviderType;

namespace terrama2
{
  namespace core
  {
    //! Defines an invalid ID
    inline uint32_t InvalidId() { return 0;}
  } /* core */
} /* terrama2 */

typedef int Srid;

#endif // __TERRAMA2_CORE_TYPEDEF_HPP__
