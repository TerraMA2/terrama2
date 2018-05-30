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
  \file terrama2/core/impl/DataAccessorStaticDataOGR.hpp

  \brief DataAccessor class for static data accessed via OGR driver.

  \author Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_STATIC_DATA_OGR_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_STATIC_DATA_OGR_HPP__

//TerraMA2
#include "DataAccessorFile.hpp"
#include "../core/Shared.hpp"
#include "../core/data-model/DataProvider.hpp"
#include "../core/data-model/DataSet.hpp"
#include "../core/data-model/Filter.hpp"
#include "../core/data-access/DataAccessorGeometricObject.hpp"

// TerraLib
#include <terralib/memory/DataSet.h>

// STL
#include <memory>
#include <string>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorStaticDataOGR

      \brief DataAccessor class for static data accessed via OGR driver.

    */
    class TMIMPLEXPORT DataAccessorStaticDataOGR : public DataAccessorGeometricObject, public DataAccessorFile
    {
      public:

        DataAccessorStaticDataOGR(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
        virtual ~DataAccessorStaticDataOGR();

        static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
        {
          return std::make_shared<DataAccessorStaticDataOGR>(dataProvider, dataSeries);
        }
        static DataAccessorType dataAccessorType(){ return "STATIC_DATA-ogr"; }

        // Doc in base class
        virtual std::string dataSourceType() const override;
        // Doc in base class
        virtual std::string getTimeZone(DataSetPtr dataSet, bool /*logErrors = false*/) const override;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_STATIC_DATA_OGR_HPP__
