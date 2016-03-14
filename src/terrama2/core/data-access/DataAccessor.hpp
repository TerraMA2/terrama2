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
  \file terrama2/core/data-access/DataAccessor.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_HPP__

//TerraMA2
#include "../../Config.hpp"
#include "../data-model/DataProvider.hpp"
#include "../data-model/Filter.hpp"

//TerraLib
#include <terralib/datatype/TimeInstantTZ.h>

namespace terrama2
{
  namespace core
  {
    /*!
    \class DataAccessor
    \brief Base class to access data from a DataSeries.

    Derived classes as responsible for the whole data access process,
    from downloading, when necessary, to instantiating the proper data access class.

    Derived classes should also be able read the format data from the dataset format map.

    */
    class DataAccessor
    {
    public:
      //! Returns the last Data date found on last access.
      virtual te::dt::TimeInstantTZ lastDateTime() const = 0;

    protected:
      /*!
        \brief TODO: doc DataAccessor

        \param filter If defined creates a cache for the filtered data.
      */
      DataAccessor(DataProvider dataProvider, DataSeries dataSeries, Filter filter = Filter())
        : dataProvider_(dataProvider),
          dataSeries_(dataSeries),
          filter_(filter) {}

      virtual ~DataAccessor() {}

      DataProvider dataProvider_;
      DataSeries dataSeries_;
      Filter filter_;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_HPP__
