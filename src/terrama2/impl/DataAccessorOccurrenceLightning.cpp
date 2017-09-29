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
  \file terrama2/core/data-access/DataAccessorOccurrenceLightning.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorOccurrenceLightning.hpp"
#include "../core/data-access/DataRetriever.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/FilterUtils.hpp"
#include "../core/utility/Utils.hpp"

// terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/TimeDuration.h>
#include <terralib/datatype/Date.h>
#include <terralib/geometry/GeometryProperty.h>

// Qt
#include <QUrl>
#include <QDir>

//STL
#include <limits>

terrama2::core::DataAccessorOccurrenceLightning::DataAccessorOccurrenceLightning(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
  : DataAccessor(dataProvider, dataSeries, false),
    DataAccessorOccurrence(dataProvider, dataSeries, false),
    DataAccessorFile(dataProvider, dataSeries, false)
{
  if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorOccurrenceLightning::dataSourceType() const
{
  return "OGR";
}
std::string terrama2::core::DataAccessorOccurrenceLightning::typePrefix() const
{
  return "CSV:";
}

void terrama2::core::DataAccessorOccurrenceLightning::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  // only one timestamp column
  size_t lonPos = std::numeric_limits<size_t>::max();
  size_t latPos = std::numeric_limits<size_t>::max();

  Srid srid = getSrid(dataSet);

  auto timestampPropertyName = terrama2::core::simplifyString(getTimestampPropertyName(dataSet));

  std::unique_ptr<te::dt::DateTimeProperty> timestampProperty(new te::dt::DateTimeProperty(timestampPropertyName, te::dt::TIME_INSTANT));
  std::unique_ptr<te::dt::SimpleProperty> latProperty(new te::dt::SimpleProperty(getLatitudePropertyName(dataSet), te::dt::DOUBLE_TYPE));
  std::unique_ptr<te::dt::SimpleProperty> lonProperty(new te::dt::SimpleProperty(getLongitudePropertyName(dataSet), te::dt::DOUBLE_TYPE));
  std::unique_ptr<te::gm::GeometryProperty> geomProperty(new te::gm::GeometryProperty("geom", srid, te::gm::PointType));

  // Find the right column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == getTimestampPropertyName(dataSet))
    {
      // datetime column found
      converter->add(i, timestampProperty.release(),
                     boost::bind(&terrama2::core::DataAccessorOccurrenceLightning::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet)));
    }
    else if(property->getName() == getLatitudePropertyName(dataSet) || property->getName() == getLongitudePropertyName(dataSet))
    {
      // update latitude column index
      if(property->getName() == getLatitudePropertyName(dataSet))
      {
        latPos = i;
        converter->add(i, latProperty.release(), boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
      }

      // update longitude column index
      if(property->getName() == getLongitudePropertyName(dataSet))
      {
        lonPos = i;
        converter->add(i, lonProperty.release(), boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
      }

      if(!isValidColumn(latPos) || !isValidColumn(lonPos))
        continue;


      std::vector<size_t> latLonAttributes;
      latLonAttributes.push_back(lonPos);
      latLonAttributes.push_back(latPos);

      converter->add(latLonAttributes, geomProperty.release(), boost::bind(&terrama2::core::DataAccessorOccurrenceLightning::stringToPoint, this, _1, _2, _3, srid));
    }
    else
    {
      auto newName = terrama2::core::simplifyString(converter->getConvertee()->getProperty(i)->getName());
      std::unique_ptr<te::dt::SimpleProperty> property(new te::dt::SimpleProperty(newName, te::dt::DOUBLE_TYPE));

      converter->add(i, property.release(), boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
    }
  }
}

void terrama2::core::DataAccessorOccurrenceLightning::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> /*converter*/,
    const std::shared_ptr<te::da::DataSetType>& /*datasetType*/) const
{
  // Don't add any columns here,
  // the converter will add columns
}

std::string terrama2::core::DataAccessorOccurrenceLightning::getLatitudePropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "latitude_property");
}

std::string terrama2::core::DataAccessorOccurrenceLightning::getLongitudePropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "longitude_property");
}

te::dt::AbstractData* terrama2::core::DataAccessorOccurrenceLightning::stringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes,
    int /*dstType*/, const std::string& timezone) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getString(indexes[0]);

    boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

    return new te::dt::TimeInstant(date.utc_time());
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error";
  }

  return nullptr;
}

te::dt::AbstractData* terrama2::core::DataAccessorOccurrenceLightning::stringToTimeDuration(te::da::DataSet *dataset,
                                                                                      const std::vector<std::size_t> &indexes,
                                                                                      int /*dstType*/,
                                                                                      const std::string &timezone) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getString(indexes[0]);

    boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

    boost::posix_time::time_duration td = date.time_of_day();
    return new te::dt::TimeDuration(td);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error";
  }

  return nullptr;
}

te::dt::AbstractData* terrama2::core::DataAccessorOccurrenceLightning::stringToDate(te::da::DataSet* dataset,
                                                                const std::vector<std::size_t>& indexes, int /*dstType*/,
                                                                const std::string& timezone) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getString(indexes[0]);
    if(dateTime.empty())
      return nullptr;

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));
    boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

    return new te::dt::Date(date.utc_time().date());
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error";
  }

  return nullptr;
}

te::dt::AbstractData* terrama2::core::DataAccessorOccurrenceLightning::stringToPoint(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes,
    int dstType, const Srid& srid) const
{
  assert(dataset);
  assert(indexes.size() == 2);

  te::dt::AbstractData* point = te::da::XYToPointConverter(dataset, indexes, dstType);
  static_cast<te::gm::Point*>(point)->setSRID(srid);

  return point;
}
