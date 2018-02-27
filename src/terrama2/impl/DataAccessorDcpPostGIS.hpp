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
  \file terrama2/core/data-access/DataAccessorDcpPostGIS.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_POSTGIS_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_POSTGIS_HPP__

//TerraMA2
#include "DataAccessorPostGIS.hpp"

#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessorDcp.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorDcpPostGIS
      \brief DataAccessor for DCP DataSeries in a PostGIS database.

    */
    class DataAccessorDcpPostGIS : public DataAccessorDcp, public DataAccessorPostGIS
    {
    public:
      DataAccessorDcpPostGIS(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
      virtual ~DataAccessorDcpPostGIS() {}

      static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries);
      static DataAccessorType dataAccessorType(){ return "DCP-postgis"; }

    protected:
      virtual std::string dataSourceType() const override;

      //no geometry column to filter
      virtual void addGeometryFilter(const std::string& /*geomPropertyName*/, const terrama2::core::Filter& /*filter*/, std::vector<std::string>& /*where*/) const override{}
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_POSTGIS_HPP__
