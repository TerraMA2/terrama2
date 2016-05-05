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
  \file terrama2/core/data-model/DataSeriesSemantics.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_SEMANTICS_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_SEMANTICS_HPP__

#include <string>
#include <vector>

#include "../Typedef.hpp"

namespace terrama2
{
  namespace core
  {
    typedef std::string DataFormat;

    /*!
      \struct DataSeriesSemantics

      \brief The DataSeriesSemantics describes the format of the data.

      It's separated in 3 characteristics:
       - DataSeriesType : Know types of DataSeries
       - DataFormat : Defines how the data should be accessed.
         It could be a 'CSV' file or a PostgreSQL/PostGIS database.
       - Semantics : Represents the data internal structure, as attributes names and data-type

    */
    struct DataSeriesSemantics
    {
      enum DataSeriesType
      {
        DCP = 1,//!< Fixed position data producer station. (Data Collection Platform).
        OCCURRENCE = 2,//!< Dated-positioned occurrence.
        GRID = 3,//!< Spatialy indexed data matrix.
        MONITORED = 4,//!< Group of vector-spatial-geometry to be monitored (//TODO: NOT in use, to be used for dynamic monitored objects).
        STATIC = 5//<! Group  of vector-spatial-geometry.
      };

      std::string name;//!< Name of the semantics.
      std::string code;//!< Name of the semantics.
      DataSeriesType dataSeriesType = STATIC;//!< Semantics type of DataSeries
      DataFormat dataFormat;//TODO: how to doc this?!
      std::vector<DataProviderType> providersTypeList;

      bool operator<(const DataSeriesSemantics& rhs) const { return code < rhs.code; }
      bool operator==(const DataSeriesSemantics& rhs) const { return code == rhs.code; }
    };
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_SEMANTICS_HPP__
