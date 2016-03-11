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
  \file terrama2/core/data-model/DataSeries.hpp

  \brief Models the information of a DataSeries.

  \author Evandro Delatin
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_HPP__

#include "../typedef.hpp"

// STL
#include <string>
#include <vector>

namespace terrama2
{
  namespace core
  {
    /*!
      \struct DataSeries

      \brief Models the information of a DataSeries.

      A DataSeries is description of a dataset. This is the set to be
      referenced one collection or analysis. Ex. set of PCD.
     */

    typedef uint64_t DataSeriesSemantics;//TODO: fix types
    struct DataSeries
    {
      DataSeriesId id; //!< The identifier of the DataSeries.
      DataProviderId dataProviderId; //!< The identifier of the DataProvider
      DataSeriesSemantics semantic; //!< Semantic of the DataSeries.
      std::string name; //!< Name of the DataSeries, must be unique.
      std::string description; //!< Description of the DataSeries.

      std::vector<DataSetId> datasetList; //TODO: id or object?

      inline bool operator==(const DataSeries& dataseries){ return id == dataseries.id; }
    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_HPP__
