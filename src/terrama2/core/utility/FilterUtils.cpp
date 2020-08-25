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

//Terralib
#include <terralib/datatype/TimeInstant.h>

// Qt
#include <string>
#include <QDate>
#include <QString>
#include <QObject>
#include <QRegExp>
#include <QStringList>

#include <time.h>

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

int terrama2::core::getMaxDay(int month, int year) {
    if(month == 0 || month == 2 || month == 4 || month == 6 || month == 7 || month == 9 || month == 11)
        return 31;
    else if(month == 3 || month == 5 || month == 8 || month == 10)
        return 30;
    else {
        if(year % 4 == 0) {
            if(year % 100 == 0) {
                if(year % 400 == 0)
                    return 29;
                return 28;
            }
            return 29;
        }
        return 28;
    }
}

std::shared_ptr<te::dt::TimeInstantTZ> terrama2::core::gridSampleVerifyMask(std::string mask, std::shared_ptr<te::dt::TimeInstantTZ> actualDate)
{
    std::shared_ptr<te::dt::TimeInstantTZ> dateReturn;
    auto temp = QString::fromStdString(mask);
    auto list = temp.split("%", QString::SkipEmptyParts);

    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);

    int day = aTime->tm_mday;
    int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    int year = aTime->tm_year + 1900; // Year is # years since 1900
    
    std::vector<std::string> options;
    options.push_back("YYYY");
    options.push_back("MM");
    options.push_back("DD");
    options.push_back("hh");
    options.push_back("mm");
    options.push_back("ss");
    
    int optIndice = 0;

    std::map<std::string, std::string> mapDate;
    
    for(int i = 0; i < list.size(); ++i)
    {
      auto& parts = list[i];
      auto listParts = parts.split("$", QString::SkipEmptyParts);

      auto& part = listParts[0];
      std::string partStr = part.toUtf8().constData();

      auto& partContent = listParts[1];
      std::string partContentStr = partContent.toUtf8().constData();

      for(int y=optIndice; y < options.size(); ++y)
      {
        if(partStr.compare(options[y]) == 0)
        {
          switch (y) {
            case 0:
              mapDate.insert({"year", partContentStr});
              break;
            case 1:
              mapDate.insert({"month", partContentStr});
              break;
            case 2:
              mapDate.insert({"day", partContentStr});
              break;
            case 3:
              mapDate.insert({"hour", partContentStr});
              break;
            case 4:
              mapDate.insert({"minute", partContentStr});
              break;
            case 5:
              mapDate.insert({"second", partContentStr});
              break;
            default:
              std::cout << "DEFAULT" << std::endl;
           }

          optIndice++;
          if((i+1) == list.size())
          {
            continue;
          }
          break;
        }
        else
        {
          int yearAux = 0;
          int monthAux = 0;
          switch (y) {
            case 0:
              mapDate.insert({"year", std::to_string(year)});
              break;
            case 1:
              if(month < 10)
              {
                mapDate.insert({"month", "0"+std::to_string(month)});
              }
              else
              {
                mapDate.insert({"month", std::to_string(month)});
              }
              break;
            case 2:
              yearAux = std::stoi(mapDate.find("year")->second);
              monthAux = std::stoi(mapDate.find("month")->second) - 1;
              day = getMaxDay(monthAux,yearAux);
              mapDate.insert({"day", std::to_string(day)});
              break;
            case 3:
              mapDate.insert({"hour", "00"});
              break;
            case 4:
              mapDate.insert({"minute", "00"});
              break;
            case 5:
              mapDate.insert({"second", "00"});
              break;
            default:
              std::cout << "DEFAULT" << std::endl;
           }
          optIndice++;
        }
      }
    }
    
    if(!mapDate.empty())
    {

      auto mapyear = mapDate.find("year");
      auto mapmonth = mapDate.find("month");
      auto mapday = mapDate.find("day");
      auto maphour = mapDate.find("hour");
      auto mapminute = mapDate.find("minute");
      auto mapsecond = mapDate.find("second");

      std::string finalDate = mapyear->second+"-";
      finalDate = finalDate + mapmonth->second+"-";
      finalDate = finalDate + mapday->second;
      finalDate = finalDate + "T";
      finalDate = finalDate + maphour->second+":";
      finalDate = finalDate + mapminute->second+":";
      finalDate = finalDate + mapsecond->second;
      finalDate = finalDate + "-00";

      dateReturn = terrama2::core::TimeUtils::stringToTimestamp(finalDate, terrama2::core::TimeUtils::webgui_timefacet);

      return dateReturn;
    }
    else
    {
      QString errMsg = QObject::tr("The mask don't have the minimal needed parameters of a date!");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::UtilityException() << ErrorDescription(errMsg);  
    }
    return nullptr;
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
    std::string discarBeforeUtcString = terrama2::core::TimeUtils::getISOString(filter.discardBefore);
    if(filter.isCollector)
    {
      if(filter.hasBeforeFilter)
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
    else if (filter.isPythonAnalysis)
    {
      if(fileTimestampUtcString != discarBeforeUtcString){
        if(!(*fileTimestamp > *filter.discardBefore))
          return false;
      }
      else
      {
          return false;
      }
    }
    else
    {
      if (filter.isReprocessingHistoricalData)
      {
        if(fileTimestampUtcString != discarBeforeUtcString)
        {
          if(*fileTimestamp < *filter.discardBefore)
            return false;
        }
      }
      else
      {
        if(!(*fileTimestamp > *filter.discardBefore))
          return false;
      }
    }
  }
  if(filter.discardAfter)
  {
    auto discarAfterUtcString = terrama2::core::TimeUtils::getISOString(filter.discardAfter);
    if(filter.isCollector)
    {
      if(filter.hasAfterFilter)
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
    else if (filter.isPythonAnalysis)
    {
      if(fileTimestampUtcString != discarAfterUtcString)
      {
        if(!(*fileTimestamp < *filter.discardAfter))
          return false;
      }
    }
    else
    {
      if (filter.isReprocessingHistoricalData)
      {
        if(fileTimestampUtcString != discarAfterUtcString)
        {
          if(*fileTimestamp > *filter.discardAfter)
            return false;
        }
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
