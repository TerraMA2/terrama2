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
  \file terrama2/core/data-model/DataSetDcp.hpp

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_DATA_SET_DCP_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_DATA_SET_DCP_HPP__

#include "DataSet.hpp"

//Terralib
#include <terralib/geometry/Point.h>

//STL
#include <memory>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief DataSet information for Dcp

      A DCP is a fixed position data producer platform.

      DCP data is supposed to have a timestamp associated.

      ## JSon ##

      Structure for transference of a DataSetDcp.

      \code{.json}
      {
        "class" : "DataSet",
        "id" : INT,
        "data_series_id" : INT,
        "active" : BOOL,
        "format" : {...},
        "position" : STRING::WKT
      }
      \endcode

    */
    struct DataSetDcp : public DataSet
    {
      struct Field
      {
        int number = 0;
        std::string alias = "";
        std::string type = "";
      };

      //! DCP position
      std::shared_ptr<te::gm::Point> position;//TODO: A posição é sempre um ponto? pode ser uma região? delete?
      //! DCP Fields
      std::vector<Field> fields;

    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_DCP_HPP__
