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
  \file terrama2/core/data-model/Schedule.hpp

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_SCHEDULE_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_SCHEDULE_HPP__

//STL
#include <map>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>

// TerraLib
#include <terralib/datatype/TimeDuration.h>
#include <terralib/datatype/TimeInstantTZ.h>

//terrama2
#include "../Typedef.hpp"


namespace terrama2
{
  namespace core
  {
    /*!
      \brief Schedule information for processes.

      The schedule struct holds information for when a process should be executed.

      It can be of 2 forms:

        Type | Description
        ----- | -----
        Frequency | The timeout interval when a process should be executed.
        %Schedule | Every day of the week or month.

      The frequency type will repeat indefinitely every timeout interval.

      The schedule will calculate the next date to execute based on the month or day of the week.

      ## JSon ##

      \code{.json}
        {
          "class" : "Schedule",
          "id" : INT,
          "frequency" : INT,
          "frequency_unit" : STRING::UNIT,
          "schedule" : INT,
          "schedule_unit" : STRING::UNIT,
          "schedule_retry" : INT,
          "schedule_retry_unit" : STRING::UNIT,
          "schedule_timeout" : INT,
          "schedule_timeout_unit" : STRING::UNIT
        }
      \endcode

    */
    struct Schedule
    {
      //! Default constructor.
      Schedule() {}

      ScheduleId id = 0; //!< Schedule identifier.

      uint32_t frequency = 0; //!< The value for time frequency. Ex: From 5 to 5 minutes.
      std::string frequencyUnit; //!< Unit of the time frequency (years, months, days, minutes, hours or seconds)
      std::string frequencyStartTime; //!< Base date and hour to start by frequency. Format: 03:00:00.000-03:00

      uint32_t schedule = 0; //!< Value for Schedule day of week, day of year or day of month. Ex: The Third day of a week or the day 137 of a year.
      std::string scheduleTime; //!< The time to execute a process in the scheduled Day.
      std::string scheduleUnit; //!< Unit of the schedule frequency. (Ex: week, month or year)

      uint32_t scheduleRetry = 0; //!< The value of time between retrys. Ex: If fails retry at every 5 minutes.
      std::string scheduleRetryUnit; //!< Unit of the schedule retry. (years, months, days, minutes, hours or seconds)

      uint32_t scheduleTimeout = 0; //!< The time limit to retry. Ex: Stop retrys after 1 hour.
      std::string scheduleTimeoutUnit; //!< Unit of the schedule timeout. (years, months, days, minutes, hours or seconds)

      inline bool valid() const { return id; } 
      inline bool operator==(const Schedule& rhs) { return id == rhs.id; }
    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_HPP__
