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
  \file terrama2/collector/TsUtils.cpp

  \brief Tests for the TestFilter class.

  \author Jano Simas
*/

#include "TsUtils.hpp"

//terrama
#include <terrama2/collector/Utils.hpp>

//terrilib
#include <terralib/datatype/TimeInstantTZ.h>

//boost
#include <boost/date_time/local_time/local_date_time.hpp>

void TsUtils::DateConvertionTest()
{
  std::string date = "2015-12-14";
  std::string time = "10:52:33";
  QDate qDate = QDate::fromString(date.c_str(), "yyyy-MM-dd");
  QTime qTime = QTime::fromString(time.c_str(), "hh:mm:ss");
  QDateTime dateTime(qDate, qTime);
  boost::local_time::local_date_time QDate2Boost = terrama2::collector::QDateTime2BoostLocalDateTime(dateTime, "-02:00");
  qDebug() << "QDate2Boost posixString: " << QDate2Boost.to_string().c_str();
  qDebug() << "QDate2Boost timezoneStr: " << QDate2Boost.zone_as_posix_string().c_str();
  qDebug() << "QDate2Boost UTC   time : " << QDate2Boost.utc_time().time_of_day().hours();
  qDebug() << "QDate2Boost local time : " << QDate2Boost.local_time().time_of_day().hours();

  boost::local_time::local_date_time String2Boost = terrama2::collector::DateTimeString2BoostLocalDateTime(date, time, "-02:00");
  qDebug() << "String2Boost posixString: " << String2Boost.to_string().c_str();
  qDebug() << "String2Boost timezoneStr: " << String2Boost.zone_as_posix_string().c_str();
  qDebug() << "String2Boost UTC   time : " << String2Boost.utc_time().time_of_day().hours();
  qDebug() << "String2Boost local time : " << String2Boost.local_time().time_of_day().hours();

  std::string outDate, outTime, outTimezone;
  terrama2::collector::BoostLocalDateTime2DateTimeString(String2Boost, outDate, outTime, outTimezone);

  qDebug() << "Boost2String date: " << outDate.c_str();
  qDebug() << "Boost2String time: " << outTime.c_str();
  qDebug() << "Boost2String TZ  : " << outTimezone.c_str();

  te::dt::TimeInstantTZ terralibTz(String2Boost);
  qDebug() << "TerralibString   : " << terralibTz.toString().c_str();

  qDebug() << "end";
}
