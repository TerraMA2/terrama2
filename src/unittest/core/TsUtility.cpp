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
#include <terrama2/core/utility/Timer.hpp>

#include "TsUtility.hpp"

void TsUtility::testTimerNoFrequencyException()
{
  try
  {
    terrama2::core::Schedule schedule;
    schedule.id = 0;
    schedule.frequency = 0;
    schedule.frequencyUnit = "second";

    terrama2::core::Timer timer(schedule, 0);

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

    terrama2::core::Timer timer(schedule, 0);

    QFAIL("Should not be here!");
  }
  catch(...)
  {

  }
}

void TsUtility::testTimer()
{
  try
  {
    terrama2::core::Schedule schedule;

    // Schedule a timer in seconds
    schedule.id = 0;
    schedule.frequency = 800;
    schedule.frequencyUnit = "second";

    terrama2::core::Timer timerSecond1(schedule, 0);

    schedule.frequencyUnit = "ss";
    terrama2::core::Timer timerSecond2(schedule, 0);

    schedule.frequencyUnit = "s";
    terrama2::core::Timer timerSecond3(schedule, 0);

    schedule.frequencyUnit = "sec";
    terrama2::core::Timer timerSecond4(schedule, 0);

    // Schedule a timer in minutes
    schedule.frequency = 35;
    schedule.frequencyUnit = "minute";

    terrama2::core::Timer timerMinute1(schedule, 0);

    schedule.frequencyUnit = "min";
    terrama2::core::Timer timerMinute2(schedule, 0);

    schedule.frequencyUnit = "minutes";
    terrama2::core::Timer timerMinute3(schedule, 0);

    // Schedule a timer in hours
    schedule.frequency = 2;
    schedule.frequencyUnit = "hour";

    terrama2::core::Timer timerHour1(schedule, 0);

    schedule.frequencyUnit = "hh";

    terrama2::core::Timer timerHour2(schedule, 0);

    schedule.frequencyUnit = "h";

    terrama2::core::Timer timerHour3(schedule, 0);

    // Schedule a timer in days
    schedule.frequency = 3;
    schedule.frequencyUnit = "day";

    terrama2::core::Timer timerDay1(schedule, 0);

    schedule.frequencyUnit = "d";
    terrama2::core::Timer timerDay2(schedule, 0);

    schedule.frequencyUnit = "dd";
    terrama2::core::Timer timerDay3(schedule, 0);

  }
  catch(...)
  {
    QFAIL("Should not be here!");
  }
}
