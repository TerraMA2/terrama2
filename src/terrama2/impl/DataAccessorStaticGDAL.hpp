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
  \file terrama2/core/data-access/DataAccessorStaticGDAL.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_STATIC_GDAL_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_STATIC_GDAL_HPP__

//TerraMA2
#include "DataAccessorGDAL.hpp"

namespace terrama2
{
  namespace core
  {
    struct Filter;
    /*!
      \brief DataAccessor for GRID DataSeries in GeoTiff format.

    */
    class TMIMPLEXPORT DataAccessorStaticGDAL : public DataAccessorGDAL
    {
    public:

      DataAccessorStaticGDAL(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
      //! Default destructor.
      virtual ~DataAccessorStaticGDAL() = default;
      //! Default copy constructor
      DataAccessorStaticGDAL(const DataAccessorStaticGDAL& other) = default;
      //! Default move constructor
      DataAccessorStaticGDAL(DataAccessorStaticGDAL&& other) = default;
      //! Default const assignment operator
      DataAccessorStaticGDAL& operator=(const DataAccessorStaticGDAL& other) = default;
      //! Default assignment operator
      DataAccessorStaticGDAL& operator=(DataAccessorStaticGDAL&& other) = default;

      inline static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
      {
        return std::make_shared<DataAccessorStaticGDAL>(dataProvider, dataSeries);
      }

      static DataAccessorType dataAccessorType(){ return "GRID-static_gdal"; }

      virtual std::shared_ptr<te::mem::DataSet> createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const override;

      // Override function to avoid missing timezone warning, this is not used for static data
      virtual std::string getTimeZone(DataSetPtr /*dataSet*/, bool /*logErrors*/) const override { return "UTC+00"; };
      virtual bool isValidTimestamp(std::shared_ptr<SynchronizedDataSet> dataSet,
                                    size_t index,
                                    const Filter& filter,
                                    size_t dateColumn) const override {return true;}
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_STATIC_GDAL_HPP__
