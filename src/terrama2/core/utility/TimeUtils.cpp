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
#include "LocalTimeUtils.hpp"
#include "Logger.hpp"
#include "Verify.hpp"
#include "../../Exception.hpp"

// TerraLib
#include <terralib/common/UnitsOfMeasureManager.h>

// Boost
#include <boost/algorithm/string.hpp>

// Qt
#include <QString>
#include <QObject>

std::shared_ptr<te::dt::TimeInstantTZ> terrama2::core::TimeUtils::stringToTimestamp(const std::string& dateTime, const std::string& mask)
{
  boost::local_time::local_date_time ldt = stringToBoostLocalTime(dateTime, mask);
  verify::date(ldt);

  return std::make_shared<te::dt::TimeInstantTZ>(ldt);
}

boost::local_time::local_date_time terrama2::core::TimeUtils::stringToBoostLocalTime(const std::string& dateTime, const std::string& mask)
{
  std::istringstream ss(dateTime);
  ss.exceptions(std::ios_base::failbit);
  boost::local_time::local_time_input_facet* facet = new boost::local_time::local_time_input_facet(mask);
  ss.imbue(std::locale(ss.getloc(), facet));

  boost::local_time::local_date_time ldt(boost::local_time::not_a_date_time);
  ss >> ldt; // do the parse

  if(!ldt.zone())
  {
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("UTC+00"));
    ldt = boost::local_time::local_date_time(ldt.local_time(), zone);
  }

  verify::date(ldt);

  return ldt;
}

std::string terrama2::core::TimeUtils::boostLocalTimeToString(const boost::local_time::local_date_time& dateTime, const std::string& mask)
{
  std::stringstream ss;
  ss.exceptions(std::ios_base::failbit);
  boost::local_time::local_time_facet* facet = new boost::local_time::local_time_facet();
  facet->format(mask.c_str());
  ss.imbue(std::locale(ss.getloc(), facet));

  ss << dateTime; // do the parse

  return ss.str();
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::TimeUtils::nowUTC()
{
  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("UTC+00"));
  boost::local_time::local_date_time ldt = boost::local_time::local_microsec_clock::local_time(zone);

  return std::make_shared< te::dt::TimeInstantTZ >(ldt);
}

boost::local_time::local_date_time terrama2::core::TimeUtils::nowBoostLocal()
{
  auto t = g2::localtime(g2::systemtime_now());
  auto tz = g2::put_time(&t, "%Z");

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(tz));
  return boost::local_time::local_microsec_clock::local_time(zone);
}

void terrama2::core::TimeUtils::addMonth(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, int32_t months)
{
  boost::local_time::local_date_time t = timeInstant->getTimeInstantTZ();

  t += boost::gregorian::months(months);

  te::dt::TimeInstantTZ temp(t);
  (*timeInstant) = temp;
}

void terrama2::core::TimeUtils::addDay(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, int32_t days)
{
  boost::local_time::local_date_time t = timeInstant->getTimeInstantTZ();

  t += boost::gregorian::days(days);

  te::dt::TimeInstantTZ temp(t);
  (*timeInstant) = temp;
}

void terrama2::core::TimeUtils::addYear(std::shared_ptr< te::dt::TimeInstantTZ > timeInstant, int32_t years)
{
  boost::local_time::local_date_time t = timeInstant->getTimeInstantTZ();

  t += boost::gregorian::years(years);

  te::dt::TimeInstantTZ temp(t);
  (*timeInstant) = temp;
}

std::unique_ptr<te::dt::TimeInstantTZ> terrama2::core::TimeUtils::timeFromStringInterval(std::shared_ptr<te::dt::TimeInstantTZ> baseTime, std::string timeString)
{
  if(timeString.empty())
    return nullptr;

  // check if the end value should start at 0h
  bool untilZeroHour = false;
  if(timeString.back() == '+')
  {
    untilZeroHour = true;
    // remove '+' paremeter from the string
    timeString.pop_back();
  }

  boost::local_time::local_date_time ldt = baseTime->getTimeInstantTZ();
  double seconds = terrama2::core::TimeUtils::convertTimeString(timeString, "SECOND", "H");
  ldt -= boost::posix_time::milliseconds(static_cast<long>(seconds*1000));

  if(untilZeroHour)
  {
    // '+' parameter means the time interval is valid from 0h
    boost::posix_time::ptime p(ldt.date(), boost::posix_time::hours(0));
    ldt = boost::local_time::local_date_time(p, ldt.zone());
  }

  return std::unique_ptr<te::dt::TimeInstantTZ>(new te::dt::TimeInstantTZ(ldt));
}

double terrama2::core::TimeUtils::convertTimeString(const std::string& time, std::string unitName, const std::string& defaultUnit)
{
  if(time.empty())
    return 0;

  std::string timeStr = boost::to_upper_copy(time);

  double result = 0;

  auto& unitsManager = te::common::UnitsOfMeasureManager::getInstance();
  auto it = unitsManager.begin();
  if(it == unitsManager.end())
  {
    QString msg(QObject::tr("There is no UnitOfMeasure registered."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InitializationException() << terrama2::ErrorDescription(msg);
  }

  // split the input time string in numeric value and unit
  std::string numberStr;
  std::string unitStr;
  std::partition_copy(std::begin(timeStr),
                      std::end(timeStr),
                      std::back_inserter(unitStr),
                      std::back_inserter(numberStr),
                      [](const char& ch) {return std::isalpha(ch);} );

  // if no unit was given, use defaultUnit
  if(unitStr.empty()) unitStr = defaultUnit;
  boost::shared_ptr<te::common::UnitOfMeasure> uom;
  bool found = false;

  // search for the input unit
  for(;it != unitsManager.end(); ++it)
  {
    uom = it->second;
    // we only want time units
    if(uom->getType() != te::common::MeasureType::Time)
      continue;

    std::vector<std::string> vecNames;
    unitsManager.getNames(uom, vecNames);
    auto itName = std::find(std::begin(vecNames), std::end(vecNames), unitStr);
    //did we found our unit?
    if(itName != std::end(vecNames))
    {
      // update unit name to standard name
      unitStr = *itName;
      found = true;
      break;
    }
  }

  if(found)
  {
    // if we found our unit, convert the value to seconds
    double ivalue = std::stod(numberStr);
    std::string uomName = uom->getName();
    if(uomName == "SECOND")
      result = ivalue;
    else
    {
      result = unitsManager.getConversion(unitStr, "SECOND") * ivalue;
    }
  }
  else
  {
    QString msg(QObject::tr("Could not find any known unit of measure in the given string: %1.").arg(QString::fromStdString(timeStr)));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);
  }

  if(unitName != "SECOND")
    result = unitsManager.getConversion("SECOND", unitName) * result;

  return result;
}


double terrama2::core::TimeUtils::frequencySeconds(const Schedule& dataSchedule)
{
  if(dataSchedule.frequencyUnit.empty())
    return 0.;
  te::common::UnitOfMeasurePtr uom = te::common::UnitsOfMeasureManager::getInstance().find(dataSchedule.frequencyUnit);

  if(!uom)
  {
    QString errMsg = QObject::tr("Invalid unit frequency.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  double secondsFrequency = dataSchedule.frequency * te::common::UnitsOfMeasureManager::getInstance().getConversion(dataSchedule.frequencyUnit,"second");

  return secondsFrequency;
}

double terrama2::core::TimeUtils::scheduleSeconds(const Schedule& dataSchedule, std::shared_ptr < te::dt::TimeInstantTZ > baseTime)
{
  if(dataSchedule.scheduleUnit.empty())
    return 0.;

  te::common::UnitOfMeasurePtr uom = te::common::UnitsOfMeasureManager::getInstance().find(dataSchedule.scheduleUnit);

  if(!uom)
  {
    QString errMsg = QObject::tr("Invalid schedule unit.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  if(uom->getName() == "WEEK")
  {
    if(!baseTime)
      baseTime = terrama2::core::TimeUtils::nowUTC();

    boost::gregorian::greg_weekday gw(dataSchedule.schedule);
    boost::gregorian::date d(boost::date_time::next_weekday(baseTime->getTimeInstantTZ().date(), gw));
    boost::posix_time::time_duration td(boost::posix_time::duration_from_string(dataSchedule.scheduleTime));
    boost::posix_time::ptime pt(d, td);
    boost::local_time::local_date_time dt(pt, baseTime->getTimeInstantTZ().zone());
    std::shared_ptr<te::dt::TimeInstantTZ> day(new te::dt::TimeInstantTZ(dt));

    if(baseTime->getTimeInstantTZ().date() == day->getTimeInstantTZ().date())
    {
      // If the answer is the same  day, add a week to date
      addDay(day, 7);
    }

    return *day.get() - *baseTime.get();
  }
  else
  {
    QString errMsg = QObject::tr("Invalid unit for schedule.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }
}

std::string terrama2::core::TimeUtils::terramaDateMask2BoostFormat(const std::string& mask)
{
  QString m(mask.c_str());

  /*
    YYYY  year with 4 digits        %Y
    YY    year with 2 digits        %y
    MM    month with 2 digits       %m
    DD    day with 2 digits         %d
    hh    hout with 2 digits        %H
    mm    minutes with 2 digits     %M
    ss    seconds with 2 digits     %S
    */

  m.replace("%YYYY", "%Y");
  m.replace("%YY", "%y");
  m.replace("%MM", "%m");
  m.replace("%DD", "%d");
  m.replace("%hh", "%H");
  m.replace("%mm", "%M");
  m.replace("%ss", "%S");

  return m.toStdString();
}

std::string terrama2::core::TimeUtils::getISOString(std::shared_ptr<te::dt::TimeInstantTZ> timeinstant)
{
  if(!TimeUtils::isValid(timeinstant))
    return "";

  auto localTime = timeinstant->getTimeInstantTZ();
  return boost::posix_time::to_iso_extended_string(localTime.utc_time())+"Z";
}

bool terrama2::core::TimeUtils::isValid(std::shared_ptr<te::dt::TimeInstantTZ> timeinstant) {
  if(!timeinstant) {
    return false;
  }

  const auto& boostTime = timeinstant->getTimeInstantTZ();
  if(boostTime.is_special()) {
    return false;
  }

  if(boostTime.is_not_a_date_time()) {
    return false;
  }

  return true;
}
