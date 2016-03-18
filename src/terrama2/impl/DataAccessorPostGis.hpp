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
  \file terrama2/core/data-access/DataAccessorPostGis.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_POSTGIS_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_POSTGIS_HPP__

//TerraMA2
#include "../core/shared.hpp"
#include "../core/data-access/DataAccessor.hpp"
#include "../core/data-model/DataSet.hpp"
#include "../core/data-model/Filter.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorPostGis

      \brief Base class for DataAccessor classes that access a PostgreSQL/PostGIS SGDB.

    */
    class DataAccessorPostGis : public virtual DataAccessor
    {
    public:
      // Doc in base class
      virtual std::shared_ptr<te::mem::DataSet> getDataSet(const std::string& uri, const Filter& filter, const DataSet& dataSet) const;

    protected:
      // Doc in base class
      virtual std::string retrieveData(const DataRetrieverPtr dataRetriever, const DataSet& dataSet, const Filter& filter) const {throw;}//TODO: throw here
      //! Recover table name where data is stored
      virtual std::string getTableName(const DataSet& dataSet) const = 0;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_POSTGIS_HPP__
