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
  \file terrama2/core/utility/TimeUtils.hpp

  \brief Time utils for Terrama2

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_TIMEUTILS_HPP__
#define __TERRAMA2_CORE_TIMEUTILS_HPP__

// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>

namespace terrama2
{
  namespace core
  {
    namespace TimeUtils
    {

      /*!
        \brief Return a TimeInstantTZ with the current time in UTC.

        \return A TimeInstantTZ  with the current time in UTC.
      */
      std::shared_ptr< te::dt::TimeInstantTZ > nowUTC();

      /*!
        \brief Add or subtrac a number of months from a TimeInstantTZ

        \param timeInstant A TimeInstantTZ to be added or subtracted
        \param month A number of months to add or subtract in the TimeInstantTZ
       */
      void addMonth(std::shared_ptr<te::dt::TimeInstantTZ> timeInstant, int64_t months);

      /*!
        \brief Add or subtrac a number of days from a TimeInstantTZ

        \param timeInstant A TimeInstantTZ to be added or subtracted
        \param days A number of days to add or subtract in the TimeInstantTZ
       */
      void addDay(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, int64_t days);

      /*!
        \brief Add or subtrac a number of years from a TimeInstantTZ

        \param timeInstant A TimeInstantTZ to be added or subtracted
        \param years A number of years to add or subtract in the TimeInstantTZ
       */
      void addYear(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, int64_t years);

      /*!
        \brief Read units of time from string and convert it to the given unit.

        \param time String with the time.
        \param unitName The name of the output unit, it must be registered in UnitsOfMeasureManager.
       */
      double convertStringToUnitOfMeasure(const std::string& time, std::string unitName);

    }
  }
}
#endif //__TERRAMA2_CORE_TIMEUTILS_HPP__
