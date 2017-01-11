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
  time_t ts = 0;
  struct tm t;
  char buf[16];
  ::localtime_r(&ts, &t);
  ::strftime(buf, sizeof(buf), "%Z", &t);

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(buf));
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

double terrama2::core::TimeUtils::convertTimeString(const std::string& time, std::string unitName, const std::string& defaultUnit)
{
  std::string timeStr = boost::to_upper_copy(time);
  double result = 0;
  auto it = te::common::UnitsOfMeasureManager::getInstance().begin();

  if(it == te::common::UnitsOfMeasureManager::getInstance().end())
  {
    QString msg(QObject::tr("There is no UnitOfMeasure registered."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InitializationException() << terrama2::ErrorDescription(msg);
  }

  bool found = false;
  while(it != te::common::UnitsOfMeasureManager::getInstance().end())
  {
    auto uom = it->second;
    if(uom->getType() != te::common::MeasureType::Time)
    {
      ++it;
      continue;
    }

    std::vector<std::string> vecNames;
    te::common::UnitsOfMeasureManager::getInstance().getNames(it->second, vecNames);


    for(std::string& name : vecNames)
    {
      size_t foundPos = timeStr.find(name);
      if (foundPos != std::string::npos)
      {
        found = true;
        std::string value;
        size_t lastNumericPos = timeStr.find_last_not_of("0123456789", foundPos - 1);
        if(lastNumericPos != std::string::npos)
          value = timeStr.substr(lastNumericPos + 1, foundPos - lastNumericPos - 1 );
        else
          value = timeStr.substr(0, foundPos);

        int ivalue = std::stoi(value);
        std::string uomName = uom->getName();
        if(uomName == "SECOND")
          result += ivalue;
        else
        {
          result += te::common::UnitsOfMeasureManager::getInstance().getConversion(name, "SECOND") * ivalue;
        }
        break;
      }
    }
    it++;
  }

  if(!found && defaultUnit.empty())
  {
    QString msg(QObject::tr("Could not find any known unit of measure in the given string: %1.").arg(QString::fromStdString(time)));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);
  }
  else if(!found)
  {
    try
    {
      int value = std::stoi(time);
      result = te::common::UnitsOfMeasureManager::getInstance().getConversion(defaultUnit, "SECOND") * value;
    }
    catch(...)
    {
      QString msg(QObject::tr("Could not find any known unit of measure in the given string: %1.").arg(QString::fromStdString(time)));
      TERRAMA2_LOG_ERROR() << msg;
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);
    }
  }

  if(unitName != "SECOND")
    result = te::common::UnitsOfMeasureManager::getInstance().getConversion("SECOND", unitName) * result;

  if(timeStr.front() == '-')
    return -result;
  else
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
