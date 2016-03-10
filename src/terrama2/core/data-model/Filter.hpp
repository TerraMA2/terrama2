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
  \file terrama2/core/data-model/Filter.hpp

  \brief Filter information of a given DataSetItem.

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_FILTER_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_FILTER_HPP__

#include "../Config.hpp"
#include "../typedef.hpp"

// Forward declaration
namespace te
{
  namespace dt { class TimeInstantTZ; }
  namespace gm { class MultiPolygon; }
}


namespace terrama2
{
  namespace core
  {

    /*!
      \struct Filter

      \brief Filter information of a given DataSetItem.
     */
    struct Filter
    {
        DataSetId dataSetId; //!< Associates the filter to a given DataSetItem.
        std::unique_ptr<te::dt::TimeInstantTZ> discardBefore_; //!< Initial date of interest for collecting data from the DataSetItem.
        std::unique_ptr<te::dt::TimeInstantTZ> discardAfter_; //!< Final date of interest for collecting data from the DataSetItem.
        std::unique_ptr<te::gm::MultiPolygon> geometry_; //!< Geometry to be used as area of interest for filtering the data during its collect.
        std::unique_ptr<double> value_; //!< Value to be used in a filter by value.
        //TODO: filter by value operation
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATA_MODEL_FILTER_HPP__
