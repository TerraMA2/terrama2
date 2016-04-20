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

// Boost
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Qt
#include <string>
#include <QString>

bool terrama2::core::isValidDataSetName(const std::string& mask, const Filter& filter, const std::string& name, std::shared_ptr< te::dt::TimeInstantTZ >& fileTimestamp)
{
  if(isValidMask(mask))
  {

    /*
    yyyy  year with 4 digits        [0-9]{4}
    yy    year with 2 digits        [0-9]{2}
    MM    month with 2 digits       0[1-9]|1[012]
    dd    day with 2 digits         0[1-9]|[12][0-9]|3[01]
    hh    hout with 2 digits        [0-1][0-9]|2[0-4]
    mm    minutes with 2 digits     [0-5][0-9]
    ss    seconds with 2 digits     [0-5][0-9]
    */

    QString m(mask.c_str());

    m.replace("yyyy", "(?<YEAR>[0-9]{4})");
    m.replace("yy", "(?<YEAR2DIGITS>[0-9]{2})");
    m.replace("MM", "(?<MONTH>0[1-9]|1[012])");
    m.replace("dd", "(?<DAY>0[1-9]|[12][0-9]|3[01])");
    m.replace("hh", "(?<HOUR>[0-1][0-9]|2[0-4])");
    m.replace("mm", "(?<MINUTES>[0-5][0-9])");
    m.replace("ss", "(?<SECONDS>[0-5][0-9])");

    boost::regex expression(m.toStdString());
    boost::match_results< std::string::const_iterator > match;

    if(!boost::regex_match(name, match, expression, boost::match_default))
      return false;

    if((match["YEAR"] != "" || match["YEAR2DIGITS"] != "") && match["MONTH"] != "" && match["DAY"] != "")
    {
      std::string ts;

      if(match["YEAR"] != "")
      {
        ts = match["YEAR"].str();
      }
      else
      {
        int year = std::stoi(match["YEAR2DIGITS"].str());

        if(year < 50)
          ts = "20" + match["YEAR2DIGITS"].str();
        else
          ts = "19" + match["YEAR2DIGITS"].str();
      }

      ts += match["MONTH"].str();
      ts += match["DAY"].str();

      ts += "T";
      ts += (match["HOUR"] == "" ? "00" : match["HOUR"].str());
      ts += (match["MINUTES"] == "" ? "00" : match["MINUTES"].str());
      ts += (match["SECONDS"] == "" ? "00" : match["SECONDS"].str());

      boost::posix_time::ptime boostDate(boost::posix_time::from_iso_string(ts));
      // VINICIUS: get timezone from data source/series
      std::string timezone = "00";
      boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
      boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

      fileTimestamp.reset(new te::dt::TimeInstantTZ(date));
//      fileTimestamp = new te::dt::TimeInstantTZ(date);
      std::cout << fileTimestamp->toString() << std::endl;
    }
  }
  return true;
}

bool terrama2::core::isValidMask(const std::string& mask)
{
  bool day = false,
      month = false,
      year = false,
      hour = false,
      minute = false,
      second = false;

  second = mask.find("ss") != std::string::npos;
  minute = mask.find("mm") != std::string::npos;
  hour = mask.find("hh") != std::string::npos;

  day = mask.find("dd") != std::string::npos;
  month = mask.find("MM") != std::string::npos;
  year = mask.find("yyyy") != std::string::npos || mask.find("yy") != std::string::npos;

  if(day || month || year)
    if(!(day && month && year))
    {
      return false;
    }

  if(second || minute || hour)
    if(!(second && minute && hour))
    {
      return false;
    }

  return true;
}
