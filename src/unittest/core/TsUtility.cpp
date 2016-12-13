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
  \file unittest/core/TsUtility.cpp

  \brief Tests for Core Utility class

  \author Vinicius Campanha
*/

//TerraMA2
#include <terrama2/core/Typedef.hpp>
#include <terrama2/core/data-model/Filter.hpp>
#include <terrama2/core/utility/Timer.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/FilterUtils.hpp>


#include "MockProcessLogger.hpp"
#include "TsUtility.hpp"

// GMock
#include <gtest/gtest.h>


void TsUtility::testTimerNoFrequencyException()
{
  try
  {
    terrama2::core::Schedule schedule;
    schedule.id = 0;
    schedule.frequency = 0;
    schedule.frequencyUnit = "second";

    terrama2::core::MockProcessLogger logger;
    EXPECT_CALL(logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(terrama2::core::TimeUtils::nowUTC()));
    auto lastTime = logger.getLastProcessTimestamp(1);
    terrama2::core::Timer timer(schedule, 1, lastTime);

    QFAIL("Should not be here!");
  }
  catch(...)
  {

  }
}

void TsUtility::testTimerInvalidUnitException()
{
  try
  {
    terrama2::core::Schedule schedule;
    schedule.id = 0;
    schedule.frequency = 30;
    schedule.frequencyUnit = "invalid";

    terrama2::core::MockProcessLogger logger;
    EXPECT_CALL(logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(terrama2::core::TimeUtils::nowUTC()));
    auto lastTime = logger.getLastProcessTimestamp(1);
    terrama2::core::Timer timer(schedule, 1, lastTime);

    QFAIL("Should not be here!");
  }
  catch(...)
  {

  }
}

void TsUtility::testFrequencyTimer()
{
  try
  {
    terrama2::core::Schedule schedule;

    // Schedule a timer in seconds
    schedule.id = 0;
    schedule.frequency = 800;
    schedule.frequencyUnit = "second";

    terrama2::core::MockProcessLogger logger;
    EXPECT_CALL(logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(terrama2::core::TimeUtils::nowUTC()));
    auto lastTime = logger.getLastProcessTimestamp(1);
    terrama2::core::Timer timerSecond1(schedule, 1, lastTime);

    schedule.frequencyUnit = "ss";
    terrama2::core::Timer timerSecond2(schedule, 1, lastTime);

    schedule.frequencyUnit = "s";
    terrama2::core::Timer timerSecond3(schedule, 1, lastTime);

    schedule.frequencyUnit = "sec";
    terrama2::core::Timer timerSecond4(schedule, 1, lastTime);

    // Schedule a timer in minutes
    schedule.frequency = 35;
    schedule.frequencyUnit = "minute";

    terrama2::core::Timer timerMinute1(schedule, 1, lastTime);

    schedule.frequencyUnit = "min";
    terrama2::core::Timer timerMinute2(schedule, 1, lastTime);

    schedule.frequencyUnit = "minutes";
    terrama2::core::Timer timerMinute3(schedule, 1, lastTime);

    // Schedule a timer in hours
    schedule.frequency = 2;
    schedule.frequencyUnit = "hour";

    terrama2::core::Timer timerHour1(schedule, 1, lastTime);

    schedule.frequencyUnit = "hh";

    terrama2::core::Timer timerHour2(schedule, 1, lastTime);

    schedule.frequencyUnit = "h";

    terrama2::core::Timer timerHour3(schedule, 1, lastTime);

    // Schedule a timer in days
    schedule.frequency = 3;
    schedule.frequencyUnit = "day";

    terrama2::core::Timer timerDay1(schedule, 1, lastTime);

    schedule.frequencyUnit = "d";
    terrama2::core::Timer timerDay2(schedule, 1, lastTime);

    schedule.frequencyUnit = "dd";
    terrama2::core::Timer timerDay3(schedule, 1, lastTime);

  }
  catch(...)
  {
    QFAIL("Should not be here!");
  }
}


void TsUtility::testFrequencyTimerBase()
{
  try
  {
    terrama2::core::Schedule schedule;
    schedule.id = 0;
    schedule.frequency = 30;
    schedule.frequencyUnit = "minute";
    schedule.frequencyStartTime = "00:01:00.123-03:00";

    terrama2::core::MockProcessLogger logger;

    auto lastExecution = terrama2::core::TimeUtils::nowUTC();
    terrama2::core::TimeUtils::addDay(lastExecution, -1);

    EXPECT_CALL(logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(lastExecution));

    auto lastTime = logger.getLastProcessTimestamp(1);
    terrama2::core::Timer timer(schedule, 1, lastTime);

  }
  catch (const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString().c_str());
  }
  catch (const boost::exception& e)
  {
    QFAIL(boost::diagnostic_information(e).c_str());
  }
  catch (const std::exception& e)
  {
    QFAIL(e.what());
  }
  catch(...)
  {
    QFAIL("Should not be here!");
  }
}


void TsUtility::testFrequencyTimerFirstExecutionEarly()
{
  try
  {
    terrama2::core::Schedule schedule;
    schedule.id = 0;
    schedule.frequency = 30;
    schedule.frequencyUnit = "minute";
    schedule.frequencyStartTime = "00:01:00.123-03:00";

    terrama2::core::MockProcessLogger logger;
    EXPECT_CALL(logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    auto lastTime = logger.getLastProcessTimestamp(1);
    terrama2::core::Timer timer(schedule, 1, lastTime);

  }
  catch (const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString().c_str());
  }
  catch (const boost::exception& e)
  {
    QFAIL(boost::diagnostic_information(e).c_str());
  }
  catch (const std::exception& e)
  {
    QFAIL(e.what());
  }
  catch(...)
  {
    QFAIL("Should not be here!");
  }
}


void TsUtility::testFrequencyTimerFirstExecutionLater()
{
  try
  {
    terrama2::core::Schedule schedule;
    schedule.id = 0;
    schedule.frequency = 30;
    schedule.frequencyUnit = "minute";
    schedule.frequencyStartTime = "23:59:00.000-03:00";

    terrama2::core::MockProcessLogger logger;
    EXPECT_CALL(logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    auto lastTime = logger.getLastProcessTimestamp(1);
    terrama2::core::Timer timer(schedule, 1, lastTime);

  }
  catch (const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString().c_str());
  }
  catch (const boost::exception& e)
  {
    QFAIL(boost::diagnostic_information(e).c_str());
  }
  catch (const std::exception& e)
  {
    QFAIL(e.what());
  }
  catch(...)
  {
    QFAIL("Should not be here!");
  }
}

void TsUtility::testScheduleTimer()
{
  try
  {
    terrama2::core::Schedule schedule;

    // Schedule a timer in seconds
    schedule.id = 0;
    schedule.schedule = 6;
    schedule.scheduleTime = "09:00:00.000";
    schedule.scheduleUnit = "week";

    terrama2::core::MockProcessLogger logger;
    EXPECT_CALL(logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(terrama2::core::TimeUtils::nowUTC()));
    auto lastTime = logger.getLastProcessTimestamp(1);
    terrama2::core::Timer timerWeek1(schedule, 1, lastTime);
  }
  catch(...)
  {
    QFAIL("Should not be here!");
  }
}

void TsUtility::testTimeUtilsAddMonth()
{
  std::string dateTime = "2016-Apr-12 12:59:00.373302BRT+00";

  boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

  std::shared_ptr< te::dt::TimeInstantTZ > dt(new te::dt::TimeInstantTZ(date));

  terrama2::core::TimeUtils::addMonth(dt, 3);
  terrama2::core::TimeUtils::addMonth(dt, -1);

  std::string dateTime2 = "2016-Jun-12 12:59:00.373302BRT+00";

  boost::posix_time::ptime boostDate2(boost::posix_time::time_from_string(dateTime2));

  boost::local_time::time_zone_ptr zone2(new boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time date2(boostDate2.date(), boostDate2.time_of_day(), zone2, true);

  std::shared_ptr< te::dt::TimeInstantTZ > dt2(new te::dt::TimeInstantTZ(date2));

  if(*dt.get() != *dt2.get())
    QFAIL("Should not be here!");
}

void TsUtility::testTimeUtilsAddDay()
{
  std::string dateTime = "2016-Apr-12 12:59:00.373302BRT+00";

  boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

  std::shared_ptr< te::dt::TimeInstantTZ > dt(new te::dt::TimeInstantTZ(date));

  terrama2::core::TimeUtils::addDay(dt, 3);
  terrama2::core::TimeUtils::addDay(dt, -1);

  std::string dateTime2 = "2016-Apr-14 12:59:00.373302BRT+00";

  boost::posix_time::ptime boostDate2(boost::posix_time::time_from_string(dateTime2));

  boost::local_time::time_zone_ptr zone2(new boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time date2(boostDate2.date(), boostDate2.time_of_day(), zone2, true);

  std::shared_ptr< te::dt::TimeInstantTZ > dt2(new te::dt::TimeInstantTZ(date2));

  if(*dt.get() != *dt2.get())
    QFAIL("Should not be here!");
}

void TsUtility::testTimeUtilsAddYear()
{
  std::string dateTime = "2016-Apr-12 12:59:00.373302BRT+00";

  boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

  std::shared_ptr< te::dt::TimeInstantTZ > dt(new te::dt::TimeInstantTZ(date));

  terrama2::core::TimeUtils::addYear(dt, 3);
  terrama2::core::TimeUtils::addYear(dt, -1);

  std::string dateTime2 = "2018-Apr-12 12:59:00.373302BRT+00";

  boost::posix_time::ptime boostDate2(boost::posix_time::time_from_string(dateTime2));

  boost::local_time::time_zone_ptr zone2(new boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time date2(boostDate2.date(), boostDate2.time_of_day(), zone2, true);

  std::shared_ptr< te::dt::TimeInstantTZ > dt2(new te::dt::TimeInstantTZ(date2));

  if(*dt.get() != *dt2.get())
    QFAIL("Should not be here!");
}

void TsUtility::testTimeUtilsScheduleSeconds()
{
  terrama2::core::Schedule dataSchedule;

  dataSchedule.scheduleUnit = "WEEK";
  dataSchedule.schedule     = 1;
  dataSchedule.scheduleTime = "00:00";

  double res = terrama2::core::TimeUtils::scheduleSeconds(dataSchedule);

  if( res < 0)
    QFAIL("Should not be here!");
}

void TsUtility::ValidMask()
{
  std::string mask = "%MM%YYYY%DD-hhssmm.file";

  if(!terrama2::core::isValidDatedMask(mask))
    QFAIL("Should not be here!");

}

void TsUtility::invalidMask()
{
  std::string mask = "%MM-hhssmm.file";

  if(terrama2::core::isValidDatedMask(mask))
    QFAIL("Should not be here!");
}


void TsUtility::testTerramaMask2Regex()
{
  std::string mask = "({[]})?$^|?&*%DD%MM%YYYY%YY-%hh%mm%ss.file";

  if(!terrama2::core::isValidDatedMask(mask))
    QFAIL("Should not be here!");

  std::string regex = terrama2::core::terramaMask2Regex(mask);

  std::string maskConverted("\\(\\{\\[\\]\\}\\)\\?\\$\\^\\|\\?\\&"
                            ".*(?<DAY>0[1-9]|[12][0-9]|3[01])(?<MONTH>0[1-9]|1[012])"
                            "(?<YEAR>[0-9]{4})(?<YEAR2DIGITS>[0-9]{2})-"
                            "(?<HOUR>[0-1][0-9]|2[0-4])(?<MINUTES>[0-5][0-9])(?<SECONDS>[0-5][0-9])"
                            "\\.file"
                            "(?<EXTENSIONS>((\\.[^.]+)+\\.(gz|zip|rar|7z|tar)|\\.[^.]+))?");

  QCOMPARE(regex, maskConverted);
}

void TsUtility::testTerramaMaskMatch()
{
  std::string mask = "%YYYY";

  if(!terrama2::core::terramaMaskMatch(mask, "2016"))
    QFAIL("Should not be here!");

  if(terrama2::core::terramaMaskMatch(mask, "invalid"))
    QFAIL("Should not be here!");
}


void TsUtility::testValidDataSetName()
{
  std::string name = "file2016-04-19153726.file";
  std::string mask = "file%YYYY-%MM-%DD%hh%mm%ss.file";
  std::string timezone = "00";
  terrama2::core::Filter filter;
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  if(!terrama2::core::isValidDataSetName(mask, filter, timezone, name, fileTimestamp))
    QFAIL("Should not be here!");
}

void TsUtility::testValidDataSetNameCompress()
{
  std::string name = "file2016-04-19153726.tar.gz";
  std::string mask = "file%YYYY-%MM-%DD%hh%mm%ss";
  std::string timezone = "00";
  terrama2::core::Filter filter;
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  if(!terrama2::core::isValidDataSetName(mask, filter, timezone, name, fileTimestamp))
    QFAIL("Should not be here!");
}

void TsUtility::testValidDataSetNameCompressError()
{
  std::string name = "file2016-04-19153726.tar.error";
  std::string mask = "file%YYYY-%MM-%DD%hh%mm%ss";
  std::string timezone = "00";
  terrama2::core::Filter filter;
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  if(terrama2::core::isValidDataSetName(mask, filter, timezone, name, fileTimestamp))
    QFAIL("Should not be here!");
}

void TsUtility::testValidDataSetName2DigitsYear()
{
  std::string name = "file16-04-19153726.file";
  std::string mask = "file%YY-%MM-%DD%hh%mm%ss.file";
  std::string timezone = "00";
  terrama2::core::Filter filter;
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  if(!terrama2::core::isValidDataSetName(mask, filter, timezone, name, fileTimestamp))
    QFAIL("Should not be here!");
}

void TsUtility::testValidDataSetName2DigitsYear1900()
{
  std::string name = "file56-04-19153726.file";
  std::string mask = "file%YY-%MM-%DD%hh%mm%ss.file";
  std::string timezone = "00";
  terrama2::core::Filter filter;
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  if(!terrama2::core::isValidDataSetName(mask, filter, timezone, name, fileTimestamp))
    QFAIL("Should not be here!");
}

void TsUtility::testIgnoreArchiveExtension()
{
  std::string name = "file56-04-19153726.file";
  std::string mask = "file%YY-%MM-%DD%hh%mm%ss";
  std::string timezone = "00";
  terrama2::core::Filter filter;
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  if(!terrama2::core::isValidDataSetName(mask, filter, timezone, name, fileTimestamp))
    QFAIL("Should not be here!");
}

void TsUtility::testSpecialCharacters()
{
  std::string name = "({[]})?$^|?&16-12-13092555.file";
  std::string mask = "({[]})?$^|?&%YY-%MM-%DD%hh%mm%ss.file";
  std::string timezone = "00";
  terrama2::core::Filter filter;
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  if(!terrama2::core::isValidDataSetName(mask, filter, timezone, name, fileTimestamp))
    QFAIL("Should not be here!");
}

