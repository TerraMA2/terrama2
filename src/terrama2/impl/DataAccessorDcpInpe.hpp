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
  \file terrama2/core/data-access/DataAccessorDcpInpe.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_INPE_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_INPE_HPP__

//TerraMA2
#include "../core/shared.hpp"
#include "../core/data-access/DataAccessorDcp.hpp"
#include "../core/data-model/DataSet.hpp"
#include "../core/data-model/Filter.hpp"

//terralib
#include <terralib/common/URI/uri.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorDcpInpe
      \brief DataAccessor for DCP DataSeries from INPE.

    */
    class DataAccessorDcpInpe : public DataAccessorDcp
    {
    public:
      DataAccessorDcpInpe(const DataProvider& dataProvider, const DataSeries& dataSeries, const Filter& filter = Filter()) : DataAccessorDcp(dataProvider, dataSeries, filter) {}
      virtual ~DataAccessorDcpInpe() {};

      virtual std::shared_ptr<te::mem::DataSet> getDataSet(const std::string& uri, const Filter& filter, const DataSetDcp& datasetDcp) const override;

      virtual std::string retrieveData(const DataRetrieverPtr dataRetriever, const DataSetDcp& dataset, const Filter& filter) const override;

      std::string getMask(const DataSetDcp& dataset) const;
      virtual te::gm::Point getPosition(const DataSetDcp& dataset) const override;
      virtual std::string dataSourceTye() const override;
      virtual std::string typePrefix() const override;

      virtual void adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter) const override;
      virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const override;

      te::dt::AbstractData* StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const;
      te::dt::AbstractData* StringToDouble(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_INPE_HPP__
