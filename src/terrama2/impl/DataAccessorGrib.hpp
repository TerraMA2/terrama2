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
  \file terrama2/impl/DataAccessorGrib.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GRIB_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GRIB_HPP__

//TerraMA2

#include "DataAccessorGDAL.hpp"

namespace terrama2
{
  namespace core
  {
    struct Filter;
    /*!
      \brief DataAccessor for GRID DataSeries in Grib format.

    */
    class DataAccessorGrib : public DataAccessorGDAL
    {
    public:

      DataAccessorGrib(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
      //! Default destructor.
      virtual ~DataAccessorGrib() = default;
      //! Default copy constructor
      DataAccessorGrib(const DataAccessorGrib& other) = default;
      //! Default move constructor
      DataAccessorGrib(DataAccessorGrib&& other) = default;
      //! Default const assignment operator
      DataAccessorGrib& operator=(const DataAccessorGrib& other) = default;
      //! Default assignment operator
      DataAccessorGrib& operator=(DataAccessorGrib&& other) = default;

      inline static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
      {
        return std::make_shared<DataAccessorGrib>(dataProvider, dataSeries);
      }
      static DataAccessorType dataAccessorType(){ return "GRID-grib"; }

      virtual bool hasControlFile() const { return false; };

    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GRIB_HPP__
