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
  \file terrama2/impl/DataAccessorNetCDF.hpp

  \brief Parser of NetCDF files.
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_NETCDF_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_NETCDF_HPP__

#include "Config.hpp"
#include "DataAccessorGDAL.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
      \brief DataAccessor for GRID DataSeries in NetCDF format.

    */
    class TMIMPLEXPORT DataAccessorNetCDF : public DataAccessorGDAL
    {
    public:

      DataAccessorNetCDF(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
      //! Default destructor.
      virtual ~DataAccessorNetCDF() = default;
      //! Default copy constructor
      DataAccessorNetCDF(const DataAccessorNetCDF& other) = default;
      //! Default move constructor
      DataAccessorNetCDF(DataAccessorNetCDF&& other) = default;
      //! Default const assignment operator
      DataAccessorNetCDF& operator=(const DataAccessorNetCDF& other) = default;
      //! Default assignment operator
      DataAccessorNetCDF& operator=(DataAccessorNetCDF&& other) = default;

      inline static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
      {
        return std::make_shared<DataAccessorNetCDF>(dataProvider, dataSeries);
      }
      static DataAccessorType dataAccessorType(){ return "NETCDF-nc"; }

      virtual void addToCompleteDataSet(DataSetPtr dataSet,
                                        std::shared_ptr<te::mem::DataSet> completeDataSet,
                                        std::shared_ptr<te::da::DataSet> teDataSet,
                                        std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp,
                                        const std::string& filename) const override;

    protected:
      virtual std::unique_ptr<te::rst::Raster> getRasterBand(DataSetPtr dataSet,
                                                             std::shared_ptr<te::rst::Raster> gribRaster) const;
      std::set<int> getBand(DataSetPtr dataSet, std::shared_ptr<te::rst::Raster> raster) const;

    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_NETCDF_HPP__
