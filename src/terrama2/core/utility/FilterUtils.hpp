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
  \file terrama2/core/utility/FilterUtils.hpp

  \brief Filter information of a given DataSetItem.

  \author Jano Simas
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_UTILITY_FILTER_UTILS_HPP__
#define __TERRAMA2_CORE_UTILITY_FILTER_UTILS_HPP__

// TerraMA2
#include "../../Config.hpp"
#include "../data-model/Filter.hpp"

// Terralib
#include <terralib/datatype/TimeInstantTZ.h>

// STL
#include <vector>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Check if the mask cases with name and filter the name by date in it.
              It also fullfill the fileTimestamp with the date in name

      \param mask A mask to check  names
      \param filter Contains the checks to validate name and date
      \param timezone The timezone of the date in name
      \param name A name to check
      \param fileTimestamp A TimeInstantTZ to be filed with the timestamp in the filename

      \return Returns if the name is valid or not.

      \exception terrama2::Exception If it is not possible to form a date with mask.
      \exception terrama2::Exception If it was not possible to find a valid date in name.
    */
    bool isValidDataSetName(const std::string& mask, const Filter& filter, const std::string& timezone, const std::string& name, std::shared_ptr< te::dt::TimeInstantTZ >& fileTimestamp);

    bool terramaMaskMatch(const std::string& mask, const std::string& string);

    /*!
      \brief Check timestamp is between a valid period.

      \param filter Contains the valid period to validate date
      \param fileTimestamp A TimeInstantTZ with the date to be validated

      \return Returns if the Timestamp is valid or not.
    */
    bool isValidTimestamp(const Filter& filter, const std::shared_ptr< te::dt::TimeInstantTZ >& fileTimestamp);

    /*!
      \brief Check mask has the minimal needed to form a date.

      \param mask A mask to validate

      \return Returns if the mask is valid or not.
    */
    bool isValidDatedMask(const std::string& mask);

    //! Convert a TerraMA2 mask to a regular expression
    std::string terramaMask2Regex(const std::string& mask);
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_UTILITY_FILTER_UTILS_HPP__
