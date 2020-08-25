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

//TerraMA2
#include "../../Config.hpp"
#include "../Typedef.hpp"
#include "../Shared.hpp"

//STL
#include <memory>

#include <boost/functional/hash.hpp>

//terralib

#include <terralib/geometry/Geometry.h>
#include <terralib/datatype/TimeInstantTZ.h>

// Forward declaration
namespace te
{
  namespace dt { class TimeInstantTZ; }
}

namespace terrama2
{
  namespace core
  {

    /*!
      \brief Information on how the data should be filtered.

      ## JSon ##

      Base structure for transference of a Filter.

      \code{.json}
      {
        "class" : "Filter",
        "discard_before" : STRING::TIMESTAMP WITH TIME ZONE,
        "discard_after" : STRING::TIMESTAMP WITH TIME ZONE,
        "last_value" : BOOLEAN
        "region" : STRING::WKT,
        "by_value" : STRING,
        "data_series_id" : INTEGER
      }
      \endcode
     */
    struct Filter
    {
      Filter() = default;
      Filter(const Filter&) = default;

      std::shared_ptr<te::dt::TimeInstantTZ> discardBefore = nullptr; //!< Initial date of interest for collecting data from the DataSet.
      std::shared_ptr<te::dt::TimeInstantTZ> discardAfter = nullptr; //!< Final date of interest for collecting data from the DataSet.
      std::shared_ptr<const te::gm::Geometry> region = nullptr; //!< Geometry to be used as area of interest for filtering the data during its collect.
      bool cropRaster = false; //! If true the raster will be cropped by the region envelope.
      std::shared_ptr<size_t> lastValues = nullptr; //!< Filter by number of dates.
      int limitTo = -1; //!< Limit of values to return. Default is -1, which represents all
      std::string byValue; //! Filter by value expression. Must be a valid sql filter expression.
      DataProviderPtr dataProvider; //! Provider from static data filter.
      DataSeriesPtr dataSeries; //! Static data data series.
      std::vector<std::string> fields; //! Fields in SELECT clause
      std::string joinableTable; //! Table to perform inner join. Used when both monitored identifier and additional identififer supplied.
      std::string monitoredIdentifier; //! Monitored attribute name to execute SQL JOIN while performing operation
      std::string additionalIdentifier; //! Additional attribute name to execute SQL join while performing operation
      bool isReprocessingHistoricalData = false;
      bool hasAfterFilter = false;
      bool hasBeforeFilter = false;
      bool isPythonAnalysis = false;
      bool isCollector = false;
      std::shared_ptr<te::dt::TimeInstantTZ> lastFileTimestamp = nullptr;

      //operator bool() const { return dataSetId != 0; }
    };
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATA_MODEL_FILTER_HPP__
