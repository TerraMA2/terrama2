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
  \file terrama2/core/data-access/DataAccessorOccurrenceMvf.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_MVF_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_MVF_HPP__

//TerraMA2
#include "DataAccessorFile.hpp"
#include "../core/shared.hpp"
#include "../core/data-access/DataAccessorOccurrence.hpp"

namespace terrama2
{
  namespace core
  {
    struct Filter;
    /*!
      \class DataAccessorDcpInpe
      \brief DataAccessor for DCP DataSeries from INPE.

    */
    class DataAccessorOccurrenceMvf : public DataAccessorOccurrence, virtual public DataAccessorFile
    {
    public:
      DataAccessorOccurrenceMvf(const DataProvider& dataProvider, const DataSeries& dataSeries, const Filter& filter = Filter());
      virtual ~DataAccessorOccurrenceMvf() {}

    protected:
      virtual std::string dataSourceTye() const override;
      virtual std::string typePrefix() const override;

      virtual void adapt(const DataSet& dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const override;
      virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const override;

    private:
      Srid getSrid(const DataSet& dataSet) const;
      std::string getTimeZone(const DataSet& dataSet) const;
      std::string timestampColumnName() const;
      std::string latitudeColumnName() const;
      std::string longitudeColumnName() const;
      /*!
        \brief Convert string to TimeInstantTZ.

        \note Format recognized:  YYYY-mm-dd HH:MM:SS"

      */
      te::dt::AbstractData* stringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/, const std::string& timezone) const;

      //! Convert string to Geometry
      te::dt::AbstractData* stringToPoint(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType, const Srid& srid) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_MVF_HPP__
