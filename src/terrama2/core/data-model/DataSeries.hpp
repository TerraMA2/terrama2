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
  \file terrama2/core/DataSeries.hpp

  \brief Models the information of a DataSeries.

  \author Evandro Delatin
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_DATASERIES_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_DATASERIES_HPP__

// STL
#include <string>

namespace terrama2
{
  namespace core
  {
    struct DataSeries
    {
      uint64_t data_provider_id;
      DataSeriesSemantics data_series_semantics_id;
      uint64_t id;
      std::string name;
      std::string description;
    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_DATASERIES_HPP__


