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
  \file terrama2/core/data-access/DataStoragerTiff.cpp

  \brief

  \author Jano Simas
 */

#include "DataStoragerTiff.hpp"
#include "../core/utility/TimeUtils.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/data-model/DataProvider.hpp"

//terralib
#include <terralib/rp/Functions.h>
#include <terralib/datatype/TimeInstant.h>
#include <terralib/dataaccess/utils/Utils.h>

//Qt
#include <QUrl>

terrama2::core::DataStorager* terrama2::core::DataStoragerTiff::make(DataProviderPtr dataProvider)
{
  return new DataStoragerTiff(dataProvider);
}

std::string terrama2::core::DataStoragerTiff::getMask(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("mask");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined mask in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataStoragerTiff::getTimezone(DataSetPtr dataSet, bool logError) const
{
  try
  {
    return dataSet->format.at("timezone");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined timezone in dataset: %1.").arg(dataSet->id);
    if(logError)
      TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataStoragerTiff::zeroPadNumber(long num, int size) const
{
  std::ostringstream ss;
  ss << std::setw(size) << std::setfill('0') << num;
  return ss.str();
}

std::string terrama2::core::DataStoragerTiff::replaceMask(const std::string& mask,
    std::shared_ptr<te::dt::DateTime> timestamp,
    terrama2::core::DataSetPtr dataSet) const
{
  if(!timestamp.get())
    return mask;

  long year = 0;
  long month = 0;
  long day = 0;
  long hour = 0;
  long minutes = 0;
  long seconds = 0;

  if(timestamp->getDateTimeType() == te::dt::TIME_INSTANT)
  {
    auto dateTime = std::dynamic_pointer_cast<te::dt::TimeInstant>(timestamp);
    //invalid date type
    if(dateTime->getTimeInstant().is_not_a_date_time())
      return mask;


    auto date = dateTime->getDate();
    year = date.getYear();
    month = date.getMonth().as_number();
    day = date.getDay().as_number();

    auto time = dateTime->getTime();
    hour = time.getHours();
    minutes = time.getMinutes();
    seconds = time.getSeconds();
  }
  else if(timestamp->getDateTimeType() == te::dt::TIME_INSTANT_TZ)
  {
    auto dateTime = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(timestamp);
    auto boostLocalTime = dateTime->getTimeInstantTZ();
    //invalid date type
    if(boostLocalTime.is_not_a_date_time())
      return mask;

    std::string timezone;
    try
    {
      //get dataset timezone
      timezone = getTimezone(dataSet, false);
    }
    catch(const terrama2::core::UndefinedTagException&)
    {
      //if no timezone is set use UTC
      timezone = "UTC+00";
    }

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    auto localTime = boostLocalTime.local_time_in(zone);
    auto date = localTime.date();
    year = date.year();
    month = date.month().as_number();
    day = date.day();

    auto time = localTime.time_of_day();
    hour = time.hours();
    minutes = time.minutes();
    seconds = time.seconds();
  }
  else
  {
    //This method expects a valid Date/Time, other formats are not valid.
    QString errMsg = QObject::tr("Unknown date format.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  //replace wildcards in mask
  std::string fileName = mask;
  size_t pos = fileName.find("yyyy");
  if(pos != std::string::npos)
    fileName.replace(pos, 4, zeroPadNumber(year, 4));

  pos = fileName.find("yy");
  if(pos != std::string::npos)
    fileName.replace(pos, 2, zeroPadNumber(year, 2));

  pos = fileName.find("MM");
  if(pos != std::string::npos)
    fileName.replace(pos, 2, zeroPadNumber(month, 2));

  pos = fileName.find("dd");
  if(pos != std::string::npos)
    fileName.replace(pos, 2, zeroPadNumber(day, 2));

  pos = fileName.find("hh");
  if(pos != std::string::npos)
    fileName.replace(pos, 2, zeroPadNumber(hour, 2));

  pos = fileName.find("mm");
  if(pos != std::string::npos)
    fileName.replace(pos, 2, zeroPadNumber(minutes, 2));

  pos = fileName.find("ss");
  if(pos != std::string::npos)
    fileName.replace(pos, 2, zeroPadNumber(seconds, 2));

  //if no extension in the mask, add extension
  pos = fileName.find(".tif");
  if(pos != std::string::npos)
    fileName += ".tif";
  return fileName;
}

void terrama2::core::DataStoragerTiff::store(DataSetSeries series, DataSetPtr outputDataSet) const
{
  if(!outputDataSet.get() || !series.syncDataSet.get())
  {
    QString errMsg = QObject::tr("Mandatory parameters not provided.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  QUrl uri(QString::fromStdString(dataProvider_->uri));
  auto path = uri.path().toStdString();
  try
  {
    std::string mask = getMask(outputDataSet);

    auto dataset = series.syncDataSet->dataset();
    size_t rasterColumn = te::da::GetFirstPropertyPos(dataset.get(), te::dt::RASTER_TYPE);
    if(!isValidColumn(rasterColumn))
    {
      QString errMsg = QObject::tr("No raster attribute.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataStoragerException() << ErrorDescription(errMsg);
    }

    size_t timestampColumn = te::da::GetFirstPropertyPos(dataset.get(), te::dt::DATETIME_TYPE);

    dataset->moveBeforeFirst();
    while(dataset->moveNext())
    {
      std::shared_ptr<te::rst::Raster> raster(dataset->isNull(rasterColumn) ? nullptr : dataset->getRaster(rasterColumn).release());
      std::shared_ptr<te::dt::DateTime> timestamp;
      if(!isValidColumn(timestampColumn) || dataset->isNull(timestampColumn))
        timestamp = nullptr;
      else
        timestamp.reset(dataset->getDateTime(timestampColumn).release());

      if(!raster.get())
      {
        QString errMsg = QObject::tr("Null raster found.");
        TERRAMA2_LOG_ERROR() << errMsg;
        continue;
      }

      std::string filename = replaceMask(mask, timestamp, outputDataSet);

      std::string output = path + "/" + filename;
      te::rp::Copy2DiskRaster(*raster, output);
    }
  }
  catch(const DataStoragerException&)
  {
    throw;
  }
  catch(...)
  {
    //TODO: fix DataStoragerTiff catch
  }
}
