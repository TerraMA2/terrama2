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
  \file terrama2/core/data-access/DataAccessorDcp.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_HPP__

//TerraMA2
#include "../shared.hpp"
#include "DataAccessor.hpp"
#include "DcpSeries.hpp"

//TerraLib
#include <terralib/geometry/Point.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorDcp
      \brief DataAccessor for DCP DataSeries.

    */
    class DataAccessorDcp : public DataAccessor
    {
    public:
      DataAccessorDcp(DataProvider dataProvider, DataSeries dataSeries, Filter filter = Filter()) : DataAccessor(dataProvider, dataSeries, filter) {}

      std::shared_ptr<te::da::DataSetTypeConverter> getConverter(const std::shared_ptr<te::da::DataSetType>& datasetType) const;
      void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const;

      virtual DcpSeriesPtr getDcpSeries(const Filter& filter);
      virtual void adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter) const { }
      virtual std::string typePrefix() const { return ""; }
      virtual te::dt::TimeInstantTZ lastDateTime() const override;

      virtual std::string retrieveData(const DataRetrieverPtr dataRetriever, const DataSetDcp& dataset, const Filter& filter) = 0;
      virtual std::vector<std::string> validDataSetNames(const DataSetDcp& dataSetDcp ) const = 0;
      virtual te::gm::Point getPosition(const DataSetDcp& dataset) const = 0;
      virtual std::string dataSourceTye() const = 0;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_HPP__
