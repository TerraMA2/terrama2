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
  \file terrama2/core/data-access/DataAccessorGeoTiff.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GEOTIFF_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GEOTIFF_HPP__

//TerraMA2
#include "DataAccessorFile.hpp"
#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessorGrid.hpp"

namespace terrama2
{
  namespace core
  {
    struct Filter;
    /*!
      \class DataAccessorGeoTiff

    */
    class DataAccessorGeoTiff : public DataAccessorGrid, public DataAccessorFile
    {
    public:
      DataAccessorGeoTiff(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const Filter& filter = Filter());
      virtual ~DataAccessorGeoTiff() {}

      inline static DataAccessor* make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const Filter& filter = Filter())
      {
        return new DataAccessorGeoTiff(dataProvider, dataSeries, filter);
      }

      virtual void addToCompleteDataSet(std::shared_ptr<te::da::DataSet> completeDataSet,
                                        std::shared_ptr<te::da::DataSet> dataSet) const override;

    protected:
      virtual std::string dataSourceType() const override;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GEOTIFF_HPP__
