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
#include "Exception.hpp"
#include "../core/Logger.hpp"

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

std::vector<std::string> terrama2::collector::DataFilter::filterNames(const std::vector<std::string>& namesList)
{
  std::vector<std::string> matchesList;
  for(const std::string &name : namesList)
  {
    if(filterName(name))
      matchesList.push_back(name);
  }

  return matchesList;
}

bool terrama2::collector::DataFilter::filterName(const std::string& name)
{
  //get list of matching regex (only string format, no date value comparison
  bool res = boost::regex_match(name, maskData.regex);
  if (!res)
    return false;

  unsigned short year  = 0;
  unsigned short month = 0;
  unsigned short day   = 0;

  int hours   = -1;
  int minutes = -1;
  int seconds  = -1;

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
    seconds = std::stoi(name.substr(maskData.secondPos, 2));

  //****************************

  try
  {
    boost::gregorian::date bDate(year, month, day);
    boost::posix_time::time_duration bTime(hours == -1 ? 0 : hours, minutes == -1 ? 0 : minutes, seconds == -1 ? 0 : seconds);
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(datasetItem_.timezone()));
    boost::local_time::local_date_time time(bDate, bTime, zone, true);

    if((discardBefore_ && time < discardBefore_->getTimeInstantTZ()) || (discardAfter_ && time > discardAfter_->getTimeInstantTZ()))
      return false;

    updateLastDateTimeCollected(time);
  }
  catch(boost::exception& /*e*/)
  {
    //invalid date or time, go on and compare as possible....

    try
    {
      if(discardBefore_)
      {
        boost::local_time::local_date_time boostDiscardBefore = discardBefore_->getTimeInstantTZ();
        boost::gregorian::date discardBeforeDate = boostDiscardBefore.date();
        boost::posix_time::time_duration discardBeforeTime = boostDiscardBefore.time_of_day();

        //discard if outside valid date limits
        if(!isAfterDiscardBeforeDate(year, month, day, discardBeforeDate))
          return false;

        //if no date or same date: check time
        if((year  == 0 || year  == discardBeforeDate.year())
           && (month == 0 || month == discardBeforeDate.month().as_number())
           && (day   == 0 || day   == discardBeforeDate.day().as_number()))
        {
          //check time
          if(!isAfterDiscardBeforeTime(hours, minutes, seconds, discardBeforeTime))
            return false;
        }
      }

      if(discardAfter_)
      {
        boost::local_time::local_date_time boostDiscardAfter = discardAfter_->getTimeInstantTZ();
        boost::gregorian::date discardAfterDate = boostDiscardAfter.date();
        boost::posix_time::time_duration discardAfterTime = boostDiscardAfter.time_of_day();

        //discard if outside valid date limits
        if(!isBeforeDiscardAfterDate(year, month, day, discardAfterDate))
          return false;

        //if no date or same date: check time
        if((year  == 0 || year  == discardAfterDate.year())
           && (month == 0 || month == discardAfterDate.month().as_number())
           && (day   == 0 || day   == discardAfterDate.day().as_number()))
        {
          //check time
          if(!isBeforeDiscardAfterTime(hours, minutes, seconds, discardAfterTime))
            return false;
        }
      }
    }
    catch(boost::exception& e)
    {
      TERRAMA2_LOG_ERROR() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
      return false;
    }

  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    return false;
  }

  return true;
}

void terrama2::collector::DataFilter::updateLastDateTimeCollected(boost::local_time::local_date_time boostTime)
{
  if(currentData_)
    currentData_->dateData.reset(new te::dt::TimeInstantTZ(boostTime));
}

bool terrama2::collector::DataFilter::validateAndUpdateDate(int dateColumn, const std::shared_ptr<te::da::DataSet> &dataSet, terrama2::collector::TransferenceData& transferenceData)
{
  //discard out of valid range dates
  std::unique_ptr<te::dt::DateTime> dateTime(dataSet->getDateTime(dateColumn));
  te::dt::DateTimeType type = dateTime->getDateTimeType();
  std::shared_ptr<te::dt::TimeInstantTZ> date_data;

    switch (type) {
      case te::dt::TIME_INSTANT_TZ:
      {
        std::shared_ptr<te::dt::TimeInstantTZ> timeInstantTz(dynamic_cast<te::dt::TimeInstantTZ*>(dateTime.release()));
        assert(timeInstantTz);

        date_data = timeInstantTz;

        break;
      }
      case te::dt::TIME_INSTANT:
      {
        //Uses user set timezone local time for comparisons

        std::unique_ptr<te::dt::TimeInstant> timeInstant(dynamic_cast<te::dt::TimeInstant*>(dateTime.release()));
        assert(timeInstant);

        boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone(datasetItem_.timezone()));
        boost::local_time::local_date_time boostTime(timeInstant->getDate().getDate(), timeInstant->getTime().getTimeDuration(), zone, true);

        te::dt::TimeInstantTZ dateTimeTZ(boostTime);
        date_data = std::make_shared< te::dt::TimeInstantTZ > (dateTimeTZ);

        break;
      }
      case te::dt::DATE:
      {
        std::unique_ptr<te::dt::Date> date(dynamic_cast<te::dt::Date*>(dateTime.release()));
        assert(date);

        boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone(datasetItem_.timezone()));
        boost::local_time::local_date_time boostTime(date->getDate(), boost::posix_time::time_duration(0,0,0,0), zone, true);

        te::dt::TimeInstantTZ dateTimeTZ(boostTime);
        date_data = std::make_shared< te::dt::TimeInstantTZ > (dateTimeTZ);

        break;
      }
      default:
        break;
    }

    if(discardBefore_ && *discardBefore_ > *date_data)
      return false;
    if(discardAfter_ && *discardAfter_ < *date_data)
      return false;

    //Valid Date!!!
    //update lastDateTime
    updateLastDateTimeCollected(date_data->getTimeInstantTZ());

    transferenceData.dateData = date_data;

    return true;
}

bool terrama2::collector::DataFilter::validateGeometry(int geometryColumn, const std::shared_ptr<te::da::DataSet>& dataSet)
{
  if(!datasetItem_.filter().geometry())
    return true;

  std::unique_ptr<te::gm::Geometry> geometry(dataSet->getGeometry(geometryColumn));
  if(datasetItem_.filter().geometry()->intersects(geometry.get()))
    return true;
  else
    return false;
}

bool terrama2::collector::DataFilter::isAfterDiscardBeforeTime(int hours, int minutes, int seconds, const boost::posix_time::time_duration& discardBeforeTime) const
{
  if(hours > -1
     && hours > discardBeforeTime.hours())
    return true;
  if(minutes > -1
     && (hours == -1 || hours == discardBeforeTime.hours())
     && minutes > discardBeforeTime.minutes())
    return true;
  if(seconds > -1
     && (hours == -1   || hours == discardBeforeTime.hours())
     && (minutes == -1 || minutes == discardBeforeTime.minutes())
     && (seconds == -1 || seconds > discardBeforeTime.seconds()))
    return true;

  return false;
}

bool terrama2::collector::DataFilter::isAfterDiscardBeforeDate(unsigned int year, unsigned int month, unsigned int day, const boost::gregorian::date& discarBeforeDate) const
{
  return isAfterDiscardBeforeValue(year, discarBeforeDate.year())
      && isAfterDiscardBeforeValue(month, discarBeforeDate.month().as_number())
      && isAfterDiscardBeforeValue(day, discarBeforeDate.day().as_number());
}

bool terrama2::collector::DataFilter::isAfterDiscardBeforeValue(unsigned int value, unsigned int discardBeforeValue) const
{
  if(!value)
    return true;

  return value >= discardBeforeValue;
}

bool terrama2::collector::DataFilter::isBeforeDiscardAfterTime(int hours, int minutes, int seconds, const boost::posix_time::time_duration& discardAfterTime) const
{
  if(hours > -1
     && hours < discardAfterTime.hours())
    return true;
  if(minutes > -1
     && (hours == -1 || hours == discardAfterTime.hours())
     && minutes < discardAfterTime.minutes())
    return true;
  if(seconds > -1
     && (hours   == -1 || hours == discardAfterTime.hours())
     && (minutes == -1 || minutes == discardAfterTime.minutes())
     && (seconds == -1 || seconds < discardAfterTime.seconds()))
    return true;

  return false;
}

bool terrama2::collector::DataFilter::isBeforeDiscardAfterDate(unsigned int year, unsigned int month, unsigned int day, const boost::gregorian::date& discardAfterDate) const
{
  return isBeforeDiscardAfterValue(year, discardAfterDate.year())
      && isBeforeDiscardAfterValue(month, discardAfterDate.month().as_number())
      && isBeforeDiscardAfterValue(day, discardAfterDate.day().as_number());
}

bool terrama2::collector::DataFilter::isBeforeDiscardAfterValue(unsigned int value, unsigned int discardAfterValue) const
{
  if(!value)
    return true;

  return value <= discardAfterValue;
}

void terrama2::collector::DataFilter::filterDataSet(terrama2::collector::TransferenceData& transferenceData)
{
  class RaiiData
  {
  public:
    RaiiData(terrama2::collector::TransferenceData*& currentData, terrama2::collector::TransferenceData& transferenceData)
      : currentData_(currentData)
    {
      currentData_ = &transferenceData;
    }
    ~RaiiData()
    {
      currentData_ = nullptr;
    }

    terrama2::collector::TransferenceData*& currentData_;
  };

  RaiiData raiiData(currentData_, transferenceData);

  const std::shared_ptr<te::da::DataSet> &dataSet = transferenceData.teDataSet;
  const std::shared_ptr<te::da::DataSetType>& datasetType = transferenceData.teDataSetType;

  //Find DateTime column
  int dateColumn = -1;
  for(int i = 0, size = dataSet->getNumProperties(); i < size; ++i)
  {
    if( dataSet->getPropertyDataType(i) == te::dt::DATETIME_TYPE)
    {
      dateColumn = i;
      break;
    }
  }

  //Find Geometry column
  int geomColumn = -1;
  for(int i = 0, size = dataSet->getNumProperties(); i < size; ++i)
  {
    if( dataSet->getPropertyDataType(i) == te::dt::GEOMETRY_TYPE)
    {
      geomColumn = i;
      break;
    }
  }

  //If there is no DateTime or geometry column, nothing to be done
  if(dateColumn < 0 && geomColumn < 0)
    return;

  //Copy dataset to an in-memory dataset filtering the data
  auto memDataSet = std::make_shared<te::mem::DataSet>(datasetType.get());
  while(dataSet->moveNext())
  {
    if(dateColumn >= 0)
    {
      //Filter Time if has a dateTime column
      if(!validateAndUpdateDate(dateColumn, dataSet, transferenceData))
        continue;
    }

    if(geomColumn >= 0)
    {
      //Filter geometry if has a geometry column
      if(!validateGeometry(geomColumn, dataSet))
        continue;
    }

    te::mem::DataSetItem* dataItem = new te::mem::DataSetItem(dataSet.get());
    //Copy each property
    for(uint i = 0, size = dataSet->getNumProperties(); i < size; ++i)
      dataItem->setValue(i, dataSet->getValue(i).release());
    //add item to the new dataset
    memDataSet->add(dataItem);
  }

  transferenceData.teDataSet = memDataSet;
}

terrama2::collector::DataFilter::DataFilter(const core::DataSetItem& datasetItem, std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime)
  : datasetItem_(datasetItem),
    currentData_(nullptr)
{
  //recover last collection time logged
  const core::Filter& filter = datasetItem_.filter();

  if(!filter.discardBefore())
    discardBefore_ = lastLogTime;
  else if(!lastLogTime)
    discardBefore_.reset(static_cast<te::dt::TimeInstantTZ*>(filter.discardBefore()->clone()));
  else if(*filter.discardBefore() < *lastLogTime || *filter.discardBefore() == *lastLogTime)
    discardBefore_ = lastLogTime;
  else
    discardBefore_.reset(static_cast<te::dt::TimeInstantTZ*>(filter.discardBefore()->clone()));

  if(filter.discardAfter())
    discardAfter_.reset(static_cast<te::dt::TimeInstantTZ*>(filter.discardAfter()->clone()));

  //prepare mask data
  processMask();
}

terrama2::collector::DataFilter::~DataFilter()
{

}


void terrama2::collector::DataFilter::processMask()
{
  std::string mask = datasetItem_.mask();
  if(mask.empty())
  {
    QString errMsg = QObject::tr("Filter mask is empty.");

    TERRAMA2_LOG_ERROR() << errMsg;
    throw EmptyMaskException() << terrama2::ErrorDescription(errMsg);
  }

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




















