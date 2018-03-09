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
  \file terrama2/core/data-access/DataAccessorGDAL.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GDAL_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GDAL_HPP__

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
    class TMIMPLEXPORT DataAccessorGDAL : public DataAccessorGrid, public DataAccessorFile
    {
    public:

      DataAccessorGDAL(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
      //! Default destructor.
      virtual ~DataAccessorGDAL() = default;
      //! Default copy constructor
      DataAccessorGDAL(const DataAccessorGDAL& other) = default;
      //! Default move constructor
      DataAccessorGDAL(DataAccessorGDAL&& other) = default;
      //! Default const assignment operator
      DataAccessorGDAL& operator=(const DataAccessorGDAL& other) = default;
      //! Default assignment operator
      DataAccessorGDAL& operator=(DataAccessorGDAL&& other) = default;

      inline static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
      {
        return std::make_shared<DataAccessorGDAL>(dataProvider, dataSeries);
      }
      static DataAccessorType dataAccessorType(){ return "GRID-gdal"; }

      virtual std::shared_ptr<te::mem::DataSet> createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const override;

      virtual void addToCompleteDataSet(DataSetPtr dataSet,
                                        std::shared_ptr<te::mem::DataSet> completeDataSet,
                                        std::shared_ptr<te::da::DataSet> teDataSet,
                                        std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp,
                                        const std::string& filename) const override;

    protected:
      virtual std::string dataSourceType() const override;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GDAL_HPP__
