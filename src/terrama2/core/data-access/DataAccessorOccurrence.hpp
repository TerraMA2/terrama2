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
  \file terrama2/core/data-access/DataAccessorOccurrence.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_HPP__

//TerraMA2
#include "../Shared.hpp"
#include "DataAccessor.hpp"
#include "OccurrenceSeries.hpp"

//TerraLib
#include <terralib/geometry/Point.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/memory/DataSet.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorOccurrence
      \brief DataAccessor for Occurence DataSeries.

      Occurrence data are any data:
       - With a Date/Time attribute
       - With a geographic position attribute

      Data of lightining occurrence is an example.

    */
    class DataAccessorOccurrence : public virtual DataAccessor
    {
    public:
      DataAccessorOccurrence(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, Filter filter = Filter()) : DataAccessor(dataProvider, dataSeries, filter) {}
      virtual ~DataAccessorOccurrence() {}

      virtual OccurrenceSeriesPtr getOccurrenceSeries(const Filter& filter);
      // Doc in base class
      virtual te::dt::TimeInstantTZ lastDateTime() const override;

    protected:
      // Doc in base class
      virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const override;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_HPP__
