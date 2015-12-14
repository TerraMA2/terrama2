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
  \file terrama2/collector/Utils.hpp

  \brief helper functions.

  \author Jano Simas
*/


#include "Utils.hpp"

//QT
#include <QTimeZone>

//boost
#include <boost/date_time/local_time/local_time_types.hpp>
#include <boost/date_time/local_time/posix_time_zone.hpp>

boost::local_time::local_date_time terrama2::collector::QDateTime2BoostLocalDateTime(const QDateTime& qDateTime)
{
  QTimeZone timeZone = qDateTime.timeZone();
  QString zoneStr = timeZone.displayName(qDateTime, QTimeZone::OffsetName);
  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(zoneStr.toStdString()));
  boost::local_time::local_date_time boostTime(boost::gregorian::date(qDateTime.date().year(), qDateTime.date().month(), qDateTime.date().day()),
                                               boost::posix_time::time_duration(qDateTime.time().hour(), qDateTime.time().minute(), qDateTime.time().second()), zone, true);

  return boostTime;
}

boost::local_time::local_date_time terrama2::collector::DateTimeString2BoostLocalDateTime(const std::string& date, const std::string& time, const std::string& timezone)
{
  boost::gregorian::date boostDate(boost::gregorian::from_string(date));
  boost::posix_time::time_duration boostTime(boost::posix_time::duration_from_string(time));
  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));

  boost::local_time::local_date_time boostLocalTime(boostDate, boostTime, zone, true);

  return boostLocalTime;
}

void terrama2::collector::BoostLocalDateTime2DateTimeString(const boost::local_time::local_date_time& boostLocalDate, std::string& date, std::string& time, std::string& timezone)
{
  date = boost::gregorian::to_iso_extended_string(boostLocalDate.date());
  time = boost::posix_time::to_simple_string(boostLocalDate.local_time().time_of_day());
  timezone = boostLocalDate.zone_as_posix_string();
}
