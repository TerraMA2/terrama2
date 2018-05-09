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
  \file terrama2/core/data-model/DataSet.hpp

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_DATA_SET_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_DATA_SET_HPP__

//terrama2
#include "../Typedef.hpp"

//STL
#include <map>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Base struct of DataSet information.

      A DataSet is the base abstract for data in TerraMA².

      A DataSet can represent the data from a DCP, an occurrence series or a satellite imagens series.

      Derive classes may have extra attributes base on the DataSeriesType.

      The format attribute is a map of implementation-specific information, see specific driver for tag documentation.

      ## JSon ##

      Base structure for transferece of a DataSet.

      Derived classes may have aditional fields, the format attribute is used by the
      specific access drivers.

      \code{.json}
        {
          "class" : "DataSet",
          "id" : INT,
          "data_series_id" : INT,
          "active" : BOOL,
          "format" : {...}
        }
      \endcode
    */
    struct DataSet
    {
      //! Default destructor.
      virtual ~DataSet() = default;

      DataSetId id = terrama2::core::InvalidId();//!< Unique identifier of the DataSet.
      DataSeriesId dataSeriesId = terrama2::core::InvalidId();//!< Identifier of the DataSeries responsible for the DataSet.
      bool active = true;//!< Attribute that indicates if the DataSet should be used.
      std::map<std::string, std::string> format;//!< implementation-specific information.

      //! Equality operator
      inline bool operator==(const DataSet& rhs) { return id == rhs.id; }
    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_HPP__
