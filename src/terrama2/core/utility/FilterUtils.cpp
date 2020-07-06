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
  \file terrama2/core/utility/FilterUtils.cpp

  \brief Filter information of a given DataSetItem.

  \author Jano Simas
*/

// TerraMA2
#include "FilterUtils.hpp"
#include "TimeUtils.hpp"
#include "Logger.hpp"
#include "../Exception.hpp"

// Boost
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Qt
#include <string>
#include <QDate>
#include <QString>
#include <QObject>
#include <QRegExp>
#include <QStringList>

// TODO: This method is being call for every check of the mask, this should only be called once to avoid a costy operation.
std::string terrama2::core::terramaMask2Regex(const std::string& mask)
{
  auto temp = QString::fromStdString(mask);
  auto list = temp.split(QRegExp("\\%\\(|\\)\\%"), QString::SkipEmptyParts);
  std::function<bool(int)> isRegex;
  if(temp.startsWith("%("))
    isRegex = [](int i){ return i%2 == 0; };
  else
    isRegex = [](int i){ return i%2 != 0; };

  for(int i = 0; i < list.size(); ++i)
  {
    auto& m = list[i];
    if(isRegex(i))
    {
      m = "("+m+")";
      continue;
    }

    // escape regex metacharacters
    m.replace("+", "\\+");
    m.replace("(", "\\(");
    m.replace(")", "\\)");
    m.replace("[", "\\[");
    m.replace("]", "\\]");
    m.replace("{", "\\{");
    m.replace("}", "\\}");
    m.replace("^", "\\^");
    m.replace("$", "\\$");
    m.replace("&", "\\&");
    m.replace("|", "\\|");
    m.replace("?", "\\?");
    m.replace(".", "\\.");

    /*
     *
      YYYY  year with 4 digits        [0-9]{4}
      YY    year with 2 digits        [0-9]{2}
      MM    month with 2 digits       0[1-9]|1[012]
      DD    day with 2 digits         0[1-9]|[12][0-9]|3[01]
      hh    hout with 2 digits        [0-1][0-9]|2[0-4]
      mm    minutes with 2 digits     [0-5][0-9]
      ss    seconds with 2 digits     [0-5][0-9]
      *    any character, any times  .*
      */

    m.replace("%YYYY", "(?<YEAR>[0-9]{4})");
    m.replace("%YY", "(?<YEAR2DIGITS>[0-9]{2})");
    m.replace("%MM", "(?<MONTH>0[1-9]|1[012])");
    m.replace("%DD", "(?<DAY>0[1-9]|[12][0-9]|3[01])");
    m.replace("%JJJ", "(?<JULIAN_DAY>\\d{3})");
    m.replace("%hh", "(?<HOUR>[0-1][0-9]|2[0-4])");
    m.replace("%mm", "(?<MINUTES>[0-5][0-9])");
    m.replace("%ss", "(?<SECONDS>[0-5][0-9])");
    m.replace("*", ".*");
    // add a extension validation in case of the name has it
    m += "(?<EXTENSIONS>(\\.(gz|zip|rar|7z|tar))+)?$";
  }

  return list.join("").toStdString();
}

std::shared_ptr<te::dt::TimeInstantTZ> terrama2::core::getFileTimestamp(const std::string& mask,
                                                          const std::string& timezone,
                                                          const std::string& name)
{
  std::shared_ptr<te::dt::TimeInstantTZ> timeStamp;

  auto regexString = terramaMask2Regex(mask);

  boost::regex expression(regexString);
  boost::match_results<std::string::const_iterator> match;

  if (!boost::regex_match(name, match, expression, boost::match_default))
    return timeStamp;

  if ((match["YEAR"] != "" || match["YEAR2DIGITS"] != "") &&
      (match["JULIAN_DAY"] != "" || (match["MONTH"] != "" && match["DAY"] != "")))
  {
    std::string ts;

    if (match["YEAR"] != "")
    {
      ts = match["YEAR"].str();
    } else
    {
      int year = std::stoi(match["YEAR2DIGITS"].str());

      if (year < 80)
        ts = "20" + match["YEAR2DIGITS"].str();
      else
        ts = "19" + match["YEAR2DIGITS"].str();
    }

    if (match["JULIAN_DAY"] != "")
    {
      int julianDay = std::stoi(match["JULIAN_DAY"].str());

      if (julianDay == 0 || julianDay > 366)
      {
        QString errMsg = QObject::tr("Invalid julian day. \"%1\"").arg(julianDay);
        TERRAMA2_LOG_ERROR() << errMsg;
        throw terrama2::core::UtilityException() << ErrorDescription(errMsg);
      }

      // Creating a date starting from january, 1, year.
      QDate tempDate(std::stoi(ts), 1, 1);
      // After that, move julian days to temporary date
      tempDate = tempDate.addDays(julianDay - 1);
      // Retrieve month and day from converted date
      ts += tempDate.toString("MMdd").toStdString();
    }
    else
    {
      ts += match["MONTH"].str();
      ts += match["DAY"].str();
    }

    // if the name has only date part, it presumes that time is 23:59:59
    ts += "T";
    ts += (match["HOUR"] == "" ? "00" : match["HOUR"].str());
    ts += (match["MINUTES"] == "" ? "00" : match["MINUTES"].str());
    ts += (match["SECONDS"] == "" ? "00" : match["SECONDS"].str());

    boost::posix_time::ptime boostDate(boost::posix_time::from_iso_string(ts));
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

    timeStamp.reset(new te::dt::TimeInstantTZ(date));

  }

  return timeStamp;
}

bool terrama2::core::isValidDataSetName(const std::string& mask,
                                        const Filter& filter,
                                        const std::string& timezone,
                                        const std::string& name,
                                        std::shared_ptr< te::dt::TimeInstantTZ >& fileTimestamp)
{
  if(!isValidDatedMask(mask))
  {
    QString errMsg = QObject::tr("The mask don't have the minimal needed parameters of a date!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::UtilityException() << ErrorDescription(errMsg);
  }

  auto regexString = terramaMask2Regex(mask);

  boost::regex expression(regexString);
  boost::match_results< std::string::const_iterator > match;

  if(!boost::regex_match(name, match, expression, boost::match_default))
    return false;

  fileTimestamp = getFileTimestamp(mask, timezone, name);
  if(!fileTimestamp)
    return true;

  if(!isValidTimestamp(filter, fileTimestamp))
    return false;

  return true;
}


bool terrama2::core::terramaMaskMatch(const std::string& mask, const std::string& string)
{
  auto regexString = terramaMask2Regex(mask);

  boost::regex expression(regexString);
  boost::match_results< std::string::const_iterator > match;

  return boost::regex_match(string, match, expression, boost::match_default);
}


bool terrama2::core::isValidTimestamp(const Filter& filter, const std::shared_ptr< te::dt::TimeInstantTZ >& fileTimestamp)
{
  std::string fileTimestampUtcString = terrama2::core::TimeUtils::getISOString(fileTimestamp);
  if(filter.discardBefore)
  {
    if(filter.hasBeforeFilter)
    {
      std::string discarBeforeUtcString = terrama2::core::TimeUtils::getISOString(filter.discardBefore);
      // For reprocessing historical data
      if (filter.isReprocessingHistoricalData)
      {
        if(fileTimestampUtcString != discarBeforeUtcString)
        {
          if(*fileTimestamp < *filter.discardBefore)
            return false;
        }
        else
        {
          if(filter.lastFileTimestamp)
            return false;
        }
      }
      else
      {
        if(fileTimestampUtcString != discarBeforeUtcString)
        {
          if(!(*fileTimestamp > *filter.discardBefore))
            return false;
        }
        else
        {
          if(filter.lastFileTimestamp)
            return false;
        }
      }
    }
    else
    {
      // For reprocessing historical data
      if (filter.isReprocessingHistoricalData)
      {
        if(*fileTimestamp < *filter.discardBefore)
          return false;
      }
    }
  }
  if(filter.discardAfter)
  {
    if(filter.hasAfterFilter)
    {
      auto discarAfterUtcString = terrama2::core::TimeUtils::getISOString(filter.discardAfter);
      if (filter.isReprocessingHistoricalData)
      {
        if(fileTimestampUtcString != discarAfterUtcString)
        {
          if(*fileTimestamp > *filter.discardAfter)
            return false;
        }
        else
        {
          if(filter.lastFileTimestamp)
            return false;
        }
      }
      else
      {
        if(fileTimestampUtcString != discarAfterUtcString)
        {
          if(!(*fileTimestamp < *filter.discardAfter))
            return false;
        }
        else
        {
          if(filter.lastFileTimestamp)
          {
            auto lastFileTimestampUtcString = terrama2::core::TimeUtils::getISOString(filter.lastFileTimestamp);
            if(discarAfterUtcString != lastFileTimestampUtcString)
            {
              if(*filter.lastFileTimestamp > *filter.discardAfter)
              {
                return false;
              }
            }
            else
            {
              return false;
            }
          }
        }
      }
    }
    else
    {
      if (filter.isReprocessingHistoricalData)
      {
        if(*fileTimestamp > *filter.discardAfter)
          return false;
      }
      else
      {
        if(!(*fileTimestamp < *filter.discardAfter))
          return false;
      }
    }
  }

  return true;
}

bool terrama2::core::isValidDatedMask(const std::string& mask)
{
  bool day = mask.find("%DD") != std::string::npos;
  bool month = mask.find("%MM") != std::string::npos;
  bool year = mask.find("%YYYY") != std::string::npos || mask.find("%YY") != std::string::npos;
  bool isJulianDay = mask.find("%JJJ") != std::string::npos;

  // When julian day supplied, year is required
  if (isJulianDay)
  {
    return year;
  }
  // If has one of them, must have the others too
  else if(day || month || year)
  {
    if(!(day && month && year))
    {
      return false;
    }
  }

  return true;
}
