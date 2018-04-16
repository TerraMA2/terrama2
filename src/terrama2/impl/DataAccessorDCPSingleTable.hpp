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

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_SINGLE_TABLE_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_SINGLE_TABLE_HPP__

#include "../core/Shared.hpp"
#include "DataAccessorDcpPostGIS.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorDcpSingleTable
      \brief DataAccessor for DCP DataSeries in a single table of a PostGIS database.

    */
    class TMIMPLEXPORT DataAccessorDcpSingleTable : public DataAccessorDcpPostGIS
    {
    public:
      DataAccessorDcpSingleTable(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
      virtual ~DataAccessorDcpSingleTable() = default;

      static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries);
      static DataAccessorType dataAccessorType(){ return "DCP-single_table"; }

    protected:
      virtual void addExtraConditions(terrama2::core::DataSetPtr /*dataSet*/, std::vector<std::string>& /*whereConditions*/) const override;
      virtual std::string getDataSetTableName(DataSetPtr dataSet) const override;
      std::string getDCPIdPorperty(terrama2::core::DataSetPtr dataSet) const;
      std::string getDCPId(terrama2::core::DataSetPtr dataSet) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_SINGLE_TABLE_HPP__
