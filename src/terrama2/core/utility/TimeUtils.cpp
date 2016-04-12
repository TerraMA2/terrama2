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
  \file terrama2/core/utility/TimeUtils.cpp

  \brief Time utils for Terrama2

  \author Vinicius Campanha
*/


// TerraMA2
#include "TimeUtils.hpp"


std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::TimeUtils::now()
{
  time_t ts = 0;
  struct tm t;
  char buf[16];
  localtime_r(&ts, &t);
  strftime(buf, sizeof(buf), "%Z", &t);


  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(buf));
  boost::local_time::local_date_time ldt = boost::local_time::local_microsec_clock::local_time(zone);

  return std::make_shared< te::dt::TimeInstantTZ >(ldt);
}

void terrama2::core::TimeUtils::addMonth(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, uint64_t i)
{
  boost::local_time::local_date_time t = timeInstant->getTimeInstantTZ();

  t += boost::gregorian::months(i);

  timeInstant.reset(new te::dt::TimeInstantTZ(t));
}

void terrama2::core::TimeUtils::addDay(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, uint64_t i)
{
  boost::local_time::local_date_time t = timeInstant->getTimeInstantTZ();

  t += boost::gregorian::days(i);

  timeInstant.reset(new te::dt::TimeInstantTZ(t));
}

void terrama2::core::TimeUtils::addYear(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, uint64_t i)
{
  boost::local_time::local_date_time t = timeInstant->getTimeInstantTZ();

  t += boost::gregorian::years(i);

  timeInstant.reset(new te::dt::TimeInstantTZ(t));
}
