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
  \file terrama2/core/data-access/DataAccessorPostGIS.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_POSTGIS_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_POSTGIS_HPP__

// TerraMA2
#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessor.hpp"
#include "../core/data-model/DataSet.hpp"
#include "../core/data-model/Filter.hpp"

#include <terralib/dataaccess/query/Expression.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorPostGIS

      \brief Base class for DataAccessor classes that access a PostgreSQL/PostGIS SGDB.

    */
    class DataAccessorPostGIS : public virtual DataAccessor
    {
      public:
        DataAccessorPostGIS(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool /*checkSemantics*/ = true)
          : DataAccessor(dataProvider, dataSeries, false)
        {
        }
        virtual ~DataAccessorPostGIS() {}

        using terrama2::core::DataAccessor::getSeries;
        // Doc in base class
        virtual DataSetSeries getSeries(const std::string& uri, const terrama2::core::Filter& filter, terrama2::core::DataSetPtr dataSet, std::shared_ptr<terrama2::core::FileRemover> remover) const override;

        //! Recover table name where data is stored
        virtual std::string getDataSetTableName(DataSetPtr dataSet) const;

      protected:
        // Doc in base class
        virtual std::string retrieveData(const DataRetrieverPtr dataRetriever,
                                         DataSetPtr dataSet,
                                         const Filter& filter,
                                         std::shared_ptr<FileRemover> remover) const override;

        virtual std::string whereConditions(terrama2::core::DataSetPtr dataSet,
                                            const std::string datetimeColumnName,
                                            const terrama2::core::Filter& filter) const;

        virtual void addDateTimeFilter(const std::string datetimeColumnName,
                                       const terrama2::core::Filter& filter,
                                       std::vector<std::string>& whereConditions) const;

        virtual void addGeometryFilter(terrama2::core::DataSetPtr dataSet,
                                       const terrama2::core::Filter& filter,
                                       std::vector<std::string>& whereConditions) const;

        virtual void addValueFilter(const terrama2::core::Filter& filter,
                                    std::string& conditions) const;

        virtual std::string addLastDatesFilter(terrama2::core::DataSetPtr dataSet,
                                        const std::string datetimeColumnName,
                                        const terrama2::core::Filter& filter,
                                        std::string whereCondition) const;


        virtual void updateLastTimestamp(DataSetPtr dataSet, std::shared_ptr<te::da::DataSourceTransactor> transactor) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_POSTGIS_HPP__
