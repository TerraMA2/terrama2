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

//terrama2
#include "../Typedef.hpp"

//STL
#include <map>

#include <terralib/datatype/TimeDuration.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \struct Schedule

      \brief Model for a schedule.
    */
    struct Schedule
    {
      Schedule() : schedule(0,0,0) {}

      ScheduleId id = 0; //!< Schedule identifier.
      uint64_t frequency = 0; //!< The time frequency.
      std::string frequencyUnit; //!< Unit of the time frequency.
      te::dt::TimeDuration schedule; //!< Schedule time.
      uint64_t scheduleRetry = 0; //!< The time frequency to retry.
      std::string scheduleRetryUnit; //!< Unit of the schedule retry.
      uint64_t scheduleTimeout = 0; //!< The time limit to retry.
      std::string scheduleTimeoutUnit; //!< Unit of the schedule timeout.

      inline bool operator==(const Schedule& rhs){ return id == rhs.id; }
    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_HPP__
