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
  \file terrama2/collector/Filter.cpp

  \brief Filters data.

  \author Jano Simas
  \author Evandro Delatin
*/

// TerraMA2
#include "../core/DataSetItem.hpp"
#include "../core/Filter.hpp"
#include "DataFilter.hpp"
#include "Log.hpp"

// STL
#include <iostream>
#include <iterator>
#include <string>

// BOOST
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time_types.hpp>
#include <boost/date_time/local_time/local_date_time.hpp>

//terralib
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/datatype/TimeDuration.h>
#include <terralib/datatype/TimeInstant.h>
#include <terralib/datatype/Date.h>

#include <terralib/memory/DataSetItem.h>
#include <terralib/memory/DataSet.h>
#include <terralib/datatype/Enums.h>
#include <terralib/datatype/Date.h>

//Qt
#include <QDebug>

std::vector<std::string> terrama2::collector::DataFilter::filterNames(const std::vector<std::string>& namesList) const
{
  //get list of matching regex (only string format, no date valeu comparison
  std::vector<std::string> matchesList;
  for(const std::string &name : namesList)
  {
    bool res = boost::regex_match(name, maskData.regex);
    if (res)
      matchesList.push_back(name);
  }

  int year  = 0;
  int month = 0;
  int day   = 0;

  int hours   = 0;
  int minutes = 0;
  int second  = 0;

  std::vector<std::string> matchesList2;
  for(const auto& name : matchesList)
  {
    //**********************
    //get date values from names

    //get year value
    if(maskData.year4Pos != -1)
      year = std::stoi(name.substr(maskData.year4Pos, 4));
    if(maskData.year2Pos != -1)
      year = std::stoi(name.substr(maskData.year2Pos, 2));
    //get month value
    if(maskData.monthPos != -1)
      month = std::stoi(name.substr(maskData.monthPos, 2));
    //get day value
    if(maskData.dayPos != -1)
      day = std::stoi(name.substr(maskData.dayPos, 2));

    //get hour value
    if(maskData.hourPos != -1)
      hours = std::stoi(name.substr(maskData.hourPos, 2));
    //get minute value
    if(maskData.minutePos != -1)
      minutes = std::stoi(name.substr(maskData.minutePos, 2));
    //get second value
    if(maskData.secondPos != -1)
      second = std::stoi(name.substr(maskData.secondPos, 2));

    //****************************


    try
    {
      /*
       * FIXME: what to do if not a valid date?
       * is not wildcard is given an invalid date is build.
         what happens if only year is given?
      */
//      boost::gregorian::date date(year, month, day);
//      te::dt::TimeDuration time(hours, minutes,second);
//      te::dt::TimeInstantTZ tDate(date, time);

//      //discard if outside valid date limits
//      if(discardBefore_ && tDate < *discardBefore_)
//        continue;
//      if(discardAfter_ && tDate > *discardAfter_)
//        continue;

//      //Valid dates !!!!
//      matchesList2.push_back(name);

//      //update lastDateTime
//      if(!dataSetLastDateTime_ || *dataSetLastDateTime_ < tDate)
//        *dataSetLastDateTime_ = tDate;
    }
    catch(std::out_of_range& e)
    {
      //TODO: log not a date
      qDebug() << "terrama2::collector::DataFilter::filterNames: not a date";
      qDebug() << e.what();

    }
  }

  return matchesList2;
}

bool terrama2::collector::DataFilter::validateDate(int dateColumn, const std::shared_ptr<te::da::DataSet> &dataSet)
{
  //discard out of valid range dates
  std::unique_ptr<te::dt::DateTime> dateTime(dataSet->getDateTime(dateColumn).release());
  te::dt::DateTimeType type = dateTime->getDateTimeType();

  if(discardAfter_ || discardBefore_)
  {
    switch (type) {
      case te::dt::TIME_INSTANT_TZ:
      {
        std::unique_ptr<te::dt::TimeInstantTZ> timeInstantTz(dynamic_cast<te::dt::TimeInstantTZ*>(dateTime.release()));
        assert(timeInstantTz);
        if(discardBefore_ && *discardBefore_ > *timeInstantTz)
          return false;
        if(discardAfter_ && *discardAfter_ < *timeInstantTz)
          return false;

        //Valid Date!!!
        //update lastDateTime
        if(!dataSetLastDateTime_ || *dataSetLastDateTime_ < *timeInstantTz)
          dataSetLastDateTime_.reset(timeInstantTz.release());

        break;
      }
      case te::dt::TIME_INSTANT:
      {
        std::unique_ptr<te::dt::TimeInstant> timeInstant(dynamic_cast<te::dt::TimeInstant*>(dateTime.release()));
        assert(timeInstant);
        if(discardBefore_)
        {
          if(discardBefore_->getTimeInstantTZ().date() > timeInstant->getDate().getDate()
             || (discardBefore_->getTimeInstantTZ().date() == timeInstant->getDate().getDate() && discardBefore_->getTimeInstantTZ().local_time() > timeInstant->getTimeInstant()))
            return false;
        }
        if(discardAfter_)
        {
          if(discardAfter_->getTimeInstantTZ().date() < timeInstant->getDate().getDate()
             || (discardAfter_->getTimeInstantTZ().date() == timeInstant->getDate().getDate() && discardAfter_->getTimeInstantTZ().local_time() < timeInstant->getTimeInstant()))
            return false;
        }

        //Valid Date!!!
        //update lastDateTime
        if(!dataSetLastDateTime_
           || (dataSetLastDateTime_->getTimeInstantTZ().date() < timeInstant->getDate().getDate()
               || (dataSetLastDateTime_->getTimeInstantTZ().date() == timeInstant->getDate().getDate() && dataSetLastDateTime_->getTimeInstantTZ().local_time() < timeInstant->getTimeInstant())))
        {
          boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone(datasetItem_.timezone()));
          boost::local_time::local_date_time boostTime(timeInstant->getDate().getDate(), timeInstant->getTime().getTimeDuration(), zone, boost::local_time::local_date_time::NOT_DATE_TIME_ON_ERROR);
          dataSetLastDateTime_.reset(new te::dt::TimeInstantTZ(boostTime));
        }

        break;
      }
      case te::dt::DATE:
      {
        std::unique_ptr<te::dt::Date> date(dynamic_cast<te::dt::Date*>(dateTime.release()));
        assert(date);
        if(discardBefore_)
        {
          if(discardBefore_->getTimeInstantTZ().date() > date->getDate())
            return false;
        }
        if(discardAfter_)
        {
          if(discardAfter_->getTimeInstantTZ().date() < date->getDate())
            return false;
        }

        //Valid Date!!!
        //update lastDateTime
        if(!dataSetLastDateTime_ || *dataSetLastDateTime_ < *dateTime)
        {
          boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone(datasetItem_.timezone()));
          boost::local_time::local_date_time boostTime(date->getDate(), boost::posix_time::time_duration(0,0,0,0), zone, boost::local_time::local_date_time::NOT_DATE_TIME_ON_ERROR);
          dataSetLastDateTime_.reset(new te::dt::TimeInstantTZ(boostTime));
        }

        break;
      }
      default:
        break;
    }
  }

  return true;
}

std::shared_ptr<te::da::DataSet> terrama2::collector::DataFilter::filterDataSet(const std::shared_ptr<te::da::DataSet> &dataSet, const std::shared_ptr<te::da::DataSetType>& datasetType)
{
  //Find DateTime column
  int dateColumn = -1;
  for(uint i = 0, size = dataSet->getNumProperties(); i < size; ++i)
  {
    if( dataSet->getPropertyDataType(i) == te::dt::DATETIME_TYPE)
    {
      dateColumn = i;
      break;
    }
  }

  //Find Geometry column
  int geomColumn = -1;
  for(uint i = 0, size = dataSet->getNumProperties(); i < size; ++i)
  {
    if( dataSet->getPropertyDataType(i) == te::dt::GEOMETRY_TYPE)
    {
      geomColumn = i;
      break;
    }
  }

  //If there is no DateTime or geometry column, nothing to be done
  if(dateColumn < 0 && geomColumn < 0)
    return dataSet;

  //Copy dataset to an in-memory dataset filtering the data
  auto memDataSet = std::make_shared<te::mem::DataSet>(datasetType.get());
  while(dataSet->moveNext())
  {
    if(dateColumn > 0)
    {
      //Filter Time if has a dateTime column
      if(!validateDate(dateColumn, dataSet))
        continue;
    }

    te::mem::DataSetItem* dataItem = new te::mem::DataSetItem(dataSet.get());
    //Copy each property
    for(uint i = 0, size = dataSet->getNumProperties(); i < size; ++i)
      dataItem->setValue(i, dataSet->getValue(i).release());
    //add item to the new dataset
    memDataSet->add(dataItem);
  }

  //TODO: Implement filter geometry
  return memDataSet;
}


te::dt::TimeInstantTZ* terrama2::collector::DataFilter::getDataSetLastDateTime() const
{
  return dataSetLastDateTime_.get();
}

terrama2::collector::DataFilter::DataFilter(const core::DataSetItem& datasetItem)
  : datasetItem_(datasetItem),
    dataSetLastDateTime_(nullptr)
{
  //recover last collection time logged
  std::shared_ptr<te::dt::TimeInstantTZ> logTime = Log::getDataSetItemLastDateTime(datasetItem.id());
  const core::Filter& filter = datasetItem_.filter();

  if(!filter.discardBefore())
     discardBefore_ = logTime;
  else if(!logTime)
    discardBefore_ = std::shared_ptr<te::dt::TimeInstantTZ>(static_cast<te::dt::TimeInstantTZ*>(filter.discardBefore()->clone()));
  else if(*filter.discardBefore() < *logTime )
  {
    discardBefore_ = logTime;
  }
  else
    assert(0); //TODO: exception here

  if(filter.discardAfter())
    discardAfter_ = std::shared_ptr<te::dt::TimeInstantTZ>(static_cast<te::dt::TimeInstantTZ*>(filter.discardAfter()->clone()));

  //prepare mask data
  processMask();
}

terrama2::collector::DataFilter::~DataFilter()
{

}


void terrama2::collector::DataFilter::processMask()
{
  std::string mask = datasetItem_.mask();

  //used to fix the position of wildcards where real values have different size from wild cards.
  int distance = 0;

  std::string::const_iterator it = mask.begin();
  //wild card use two characters
  //should not go until the end
  for(; it != mask.end()-1; ++it)
  {
    //get wildCards positions

    if(*it == '%')
    {
      char character = *(it+1);
      if(character == 'A')
      {
        maskData.year4Pos = it - mask.begin()+distance;
        //two char for wild cards for four char year
        distance +=2;
      }
      else if(character == 'a')
      {
        maskData.year2Pos = it - mask.begin()+distance;
      }
      else if(character == 'M')
      {
        maskData.monthPos = it - mask.begin()+distance;
      }
      else if(character == 'd')
      {
        maskData.dayPos = it - mask.begin()+distance;
      }
      else if(character == 'h')
      {
        maskData.hourPos = it - mask.begin()+distance;
      }
      else if(character == 'm')
      {
        maskData.minutePos = it - mask.begin()+distance;
      }
      else if(character == 's')
      {
        maskData.secondPos = it - mask.begin()+distance;
      }
      else if(character == '.')
      {
        maskData.wildCharPos = it - mask.begin()+distance;
        //two char for wild cards for only one char
        --distance;
      }
    }
  }

  //prepare boost regex  wildcards
  boost::replace_all(mask, maskData.year4Str,    "(\\d{4})"); //("%A - ano com quatro digitos"))
  boost::replace_all(mask, maskData.year2Str,    "(\\d{2})"); //("%a - ano com dois digitos"))
  boost::replace_all(mask, maskData.dayStr,      "(\\d{2})"); //("%d - dia com dois digitos"))
  boost::replace_all(mask, maskData.monthStr,    "(\\d{2})"); //("%M - mes com dois digitos"))
  boost::replace_all(mask, maskData.hourStr,     "(\\d{2})"); //("%h - hora com dois digitos"))
  boost::replace_all(mask, maskData.minuteStr,   "(\\d{2})"); //("%m - minuto com dois digitos"))
  boost::replace_all(mask, maskData.secondStr,   "(\\d{2})"); //("%s - segundo com dois digitos"))
  boost::replace_all(mask, maskData.wildCharStr, "(\\w{1})");  //("%. - um caracter qualquer"))
  //boost regex
  maskData.regex = boost::regex(mask);
}




















