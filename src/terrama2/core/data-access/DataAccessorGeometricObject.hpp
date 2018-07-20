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
  \file terrama2/core/data-access/DataAccessorGeometricObject.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GEOMETRIC_OBJECT_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GEOMETRIC_OBJECT_HPP__

//TerraMA2
#include "../Shared.hpp"
#include "DataAccessor.hpp"

namespace terrama2
{
  namespace core
  {
    /**
     * @brief Base class for accessing data in the form of geometric objects
     * 
     * In TerraMA2 we consider geometric objects static or dynamic geographic geometries.
     * They are used as monitored objects for analysis, of this kind.
     * 
     * Static geometric objects would be the shape of countries, for example.
     * 
     * Dynamic geometric objects are associated with a timestamp,
     * usually with many objects with the same timestamp.
     * 
     * In analysis, Occurrence operators can be applied to this data.
     * 
     */
    class DataAccessorGeometricObject : public virtual DataAccessor
    {
      public:
        //! Default constructor
        DataAccessorGeometricObject(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
         : DataAccessor(dataProvider, dataSeries) {}
        //! Default destructor.
        virtual ~DataAccessorGeometricObject() = default;
        //! Default copy constructor
        DataAccessorGeometricObject(const DataAccessorGeometricObject& other) = default;
        //! Default move constructor
        DataAccessorGeometricObject(DataAccessorGeometricObject&& other) : DataAccessor(std::move(other)) {}
        //! Default const assignment operator
        DataAccessorGeometricObject& operator=(const DataAccessorGeometricObject& other) = default;
        //! Default assignment operator
        DataAccessorGeometricObject& operator=(DataAccessorGeometricObject&& other) { DataAccessor::operator=(std::move(other)); return *this; }
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GEOMETRIC_OBJECT_HPP__
