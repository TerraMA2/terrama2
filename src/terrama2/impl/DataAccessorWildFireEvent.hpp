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
  \file terrama2/core/impl/DataAccessorWildFireEvent.hpp

  \brief DataAccessor class for dynamic data accessed via OGR driver.

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_WILD_FIRE_EVENT_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_WILD_FIRE_EVENT_HPP__

//TerraMA2
#include "DataAccessorGeometricObjectOGR.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorWildFireEvent

      \brief DataAccessor class for static data accessed via OGR driver.

    */
    class DataAccessorWildFireEvent : public DataAccessorGeometricObjectOGR
    {
      public:

        DataAccessorWildFireEvent(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
        virtual ~DataAccessorWildFireEvent() = default;

        static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
        {
          return std::make_shared<DataAccessorWildFireEvent>(dataProvider, dataSeries);
        }
        static DataAccessorType dataAccessorType(){ return "GEOMETRIC_OBJECT-wfe"; }

        virtual void adapt(DataSetPtr dataset, std::shared_ptr<te::da::DataSetTypeConverter> converter) const override;
        virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const override;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_WILD_FIRE_EVENT_HPP__
