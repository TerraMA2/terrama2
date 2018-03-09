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
  \file terrama2/core/utility/Verify.hpp

  \brief Utility functions for easy consistency check

  \author Jano Simas
*/


#ifndef __TERRAMA2_CORE_UTILITY_VERIFY_HPP__
#define __TERRAMA2_CORE_UTILITY_VERIFY_HPP__

// TerraMa2
#include "../Config.hpp"

// Boost
#include <boost/date_time/local_time/local_date_time.hpp>
#include <boost/date_time/time.hpp>

// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/datatype/TimeInstant.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Functions for consistency check

      This namespace agregates functions to verify if some data is valid,
      all functions throw if some inconsitency is found and do nothing otherwise.

      ## Intructions for new tests ##

        - The name of the function must be minimal and refere to object beeing tested
        - The tests must be minimal and must no interfere in the code they are used
        - Test for overrall validity, for example, a boost::local_date must not be an invalid date.
    */
    namespace verify {
      /*!
        \brief Verifies if the SRID is a valid number.

        \note Doesn't check if the SRID actualy exists.
      */
      TMCOREEXPORT void srid(int srid_, bool log = true);

      /*!
        \brief Verifies if the date and the timezone are valid.
      */
      TMCOREEXPORT void date(const std::shared_ptr<te::dt::TimeInstantTZ>& date);

      /*!
        \brief Verifies if the date is valid.
      */
      TMCOREEXPORT void date(const std::shared_ptr<te::dt::TimeInstant>& date);

      /*!
        \brief Verifies if the date and the timezone are valid.
      */

      TMCOREEXPORT void date(const boost::local_time::local_date_time& date);
      /*!
        \brief Verifies if the date is valid.
      */
      TMCOREEXPORT void date(const boost::posix_time::ptime::base_time& date);
    } /* verify */
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_UTILITY_VERIFY_HPP__
