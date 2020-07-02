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

#include <boost/date_time/local_time/local_date_time.hpp>
// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>
#include <memory>
#include <string>

// TerraMa2
#include "../Config.hpp"
#include "../data-model/Schedule.hpp"

namespace terrama2 {
namespace core {
struct Schedule;
}  // namespace core
}  // namespace terrama2
namespace te {
namespace dt {
class TimeInstantTZ;
}  // namespace dt
}  // namespace te

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Utility functions for time manipulation.
    */
    namespace TimeUtils
    {
      const std::string webgui_timefacet = "%Y-%m-%dT%H:%M:%S%F%ZP";

      /*!
        \brief Return a TimeInstantTZ with the current time in UTC.

        \return A TimeInstantTZ  with the current time in UTC.
      */
      TMCOREEXPORT std::shared_ptr< te::dt::TimeInstantTZ > nowUTC();
      TMCOREEXPORT boost::local_time::local_date_time nowBoostLocal();

      /*!
        \brief Add or subtrac a number of months from a TimeInstantTZ

        \param timeInstant A TimeInstantTZ to be added or subtracted
        \param month A number of months to add or subtract in the TimeInstantTZ
       */
      TMCOREEXPORT void addMonth(std::shared_ptr<te::dt::TimeInstantTZ> timeInstant, int32_t months);

      /*!
        \brief Add or subtrac a number of days from a TimeInstantTZ

        \param timeInstant A TimeInstantTZ to be added or subtracted
        \param days A number of days to add or subtract in the TimeInstantTZ
       */
      TMCOREEXPORT void addDay(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, int32_t days);

      /*!
        \brief Add or subtrac a number of years from a TimeInstantTZ

        \param timeInstant A TimeInstantTZ to be added or subtracted
        \param years A number of years to add or subtract in the TimeInstantTZ
       */
      TMCOREEXPORT void addYear(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, int32_t years);

      /*!
        \brief Read units of time from string and convert it to the given unit.

        \param time String with the time.
        \param unitName The name of the output unit, it must be registered in UnitsOfMeasureManager.
        \param defaultUnit The default unit to be used in case there is no unit in the given string.
       */
      TMCOREEXPORT double convertTimeString(const std::string& time, std::string unitName, const std::string& defaultUnit = "");

      std::unique_ptr<te::dt::TimeInstantTZ> timeFromStringInterval(std::shared_ptr<te::dt::TimeInstantTZ> baseTime, std::string timeString);

      /*!
        \brief Converts a date string to TimeInstantTZ object.

        \param dateTime String with the time.
        \param mask Date mask.
        \return The TimeInstantTZ object created.
       */
      TMCOREEXPORT std::shared_ptr<te::dt::TimeInstantTZ> stringToTimestamp(const std::string& dateTime, const std::string& mask);

      /*!
        \brief Converts a date string to a boost local date object.

        \param dateTime String with the time.
        \param mask Date mask.
        \return The boost local date object created.
       */
      TMCOREEXPORT boost::local_time::local_date_time stringToBoostLocalTime(const std::string& dateTime, const std::string& mask);

      TMCOREEXPORT std::string boostLocalTimeToString(const boost::local_time::local_date_time& dateTime, const std::string& mask);


      /*!
       \brief A method to calculate the seconds for frequency stored in the Schedule
       \param dataSchedule The schedule struct with the information for when a process should be executed
       \return A double containing the frequency in seconds
       */
      TMCOREEXPORT double frequencySeconds(const Schedule& dataSchedule);

      /*!
       \brief A method to calculate the seconds until the scheduled date and time in stored in the Schedule
       \param dataSchedule The schedule struct with the information for when a process should be executed
       \param baseTime The time to be used to calculate how many seconds until the scheduled time, if null it will use the current time.
       \return A double containing the seconds until the scheduled timestamp
       */
      TMCOREEXPORT double scheduleSeconds(const Schedule& dataSchedule, std::shared_ptr < te::dt::TimeInstantTZ > baseTime = std::shared_ptr < te::dt::TimeInstantTZ >());


      TMCOREEXPORT std::string terramaDateMask2BoostFormat(const std::string& mask);

      //! Return iso string from TimeInstantTZ
      TMCOREEXPORT std::string getISOString(std::shared_ptr<te::dt::TimeInstantTZ> timeinstant);

      TMCOREEXPORT bool isValid(std::shared_ptr<te::dt::TimeInstantTZ> timeinstant);
    }
  }
}
#endif //__TERRAMA2_CORE_TIMEUTILS_HPP__
