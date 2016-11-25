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
      \brief DataAccessor for GRID DataSeries in GeoTiff format.

    */
    class DataAccessorGeoTiff : public DataAccessorGrid, public DataAccessorFile
    {
    public:

      DataAccessorGeoTiff(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
      //! Default destructor.
      virtual ~DataAccessorGeoTiff() = default;
      //! Default copy constructor
      DataAccessorGeoTiff(const DataAccessorGeoTiff& other) = default;
      //! Default move constructor
      DataAccessorGeoTiff(DataAccessorGeoTiff&& other) = default;
      //! Default const assignment operator
      DataAccessorGeoTiff& operator=(const DataAccessorGeoTiff& other) = default;
      //! Default assignment operator
      DataAccessorGeoTiff& operator=(DataAccessorGeoTiff&& other) = default;

      inline static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
      {
        return std::make_shared<DataAccessorGeoTiff>(dataProvider, dataSeries);
      }
      static DataAccessorType dataAccessorType(){ return "GRID-geotiff"; }

      virtual std::shared_ptr<te::mem::DataSet> createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const override;

      virtual void addToCompleteDataSet(std::shared_ptr<te::mem::DataSet> completeDataSet,
                                        std::shared_ptr<te::da::DataSet> dataSet,
                                        std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp,
                                        const std::string& filename) const override;

    protected:
      virtual std::string dataSourceType() const override;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GEOTIFF_HPP__
