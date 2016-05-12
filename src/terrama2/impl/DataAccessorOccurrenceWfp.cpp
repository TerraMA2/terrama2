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
  \file terrama2/core/data-access/DataAccessorOccurrenceWfp.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorOccurrenceWfp.hpp"
#include "../core/data-access/DataRetriever.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/FilterUtils.hpp"

// terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/geometry/GeometryProperty.h>

// Qt
#include <QUrl>
#include <QDir>

terrama2::core::DataAccessorOccurrenceWfp::DataAccessorOccurrenceWfp(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const Filter& filter)
  : DataAccessor(dataProvider, dataSeries, filter), DataAccessorOccurrence(dataProvider, dataSeries, filter),
    DataAccessorFile(dataProvider, dataSeries, filter)
{
  if(dataSeries->semantics.code != "OCCURRENCE-wfp")
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorOccurrenceWfp::dataSourceType() const
{
  return "OGR";
}
std::string terrama2::core::DataAccessorOccurrenceWfp::typePrefix() const
{
  return "CSV:";
}

void terrama2::core::DataAccessorOccurrenceWfp::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  // only one timestamp column
  int lonPos = -1;
  int latPos = -1;

  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(timestampPropertyName(), te::dt::TIME_INSTANT_TZ);

  // Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == timestampPropertyName())
    {
      // datetime column found
      converter->add(i, dtProperty,
                     boost::bind(&terrama2::core::DataAccessorOccurrenceWfp::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet)));
    }
    else if(property->getName() == latitudePropertyName() || property->getName() == longitudePropertyName())
    {
      // update latitude column index
      if(property->getName() == latitudePropertyName())
        latPos = i;

      // update longitude column index
      if(property->getName() == longitudePropertyName())
        lonPos = i;

      if(latPos == -1 || lonPos == -1)
        continue;

      // geometry columns found
      Srid srid = getSrid(dataSet);

      std::vector<size_t> latLonAttributes;
      latLonAttributes.push_back(lonPos);
      latLonAttributes.push_back(latPos);

      te::gm::GeometryProperty* newProperty = new te::gm::GeometryProperty(geometryPropertyName(), srid, te::gm::PointType);
      converter->add(latLonAttributes, newProperty, boost::bind(&terrama2::core::DataAccessorOccurrenceWfp::stringToPoint, this, _1, _2, _3, srid));
    }
    else
    {
      // the only other columns is the satellite name
      te::dt::Property* p = converter->getConvertee()->getProperty(i);
      converter->add(i, p->clone());
    }
  }
}

void terrama2::core::DataAccessorOccurrenceWfp::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter,
    const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  // Don't add any columns here,
  // the converter will add columns
}

Srid terrama2::core::DataAccessorOccurrenceWfp::getSrid(DataSetPtr dataSet) const
{
  try
  {
    Srid srid = std::stoi(dataSet->format.at("srid"));
    return srid;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined srid in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorOccurrenceWfp::timestampPropertyName() const
{
  return "data_pas";
}

std::string terrama2::core::DataAccessorOccurrenceWfp::latitudePropertyName() const
{
  return "lat";
}

std::string terrama2::core::DataAccessorOccurrenceWfp::longitudePropertyName() const
{
  return "lon";
}

std::string terrama2::core::DataAccessorOccurrenceWfp::geometryPropertyName() const
{
  return "position";
}

te::dt::AbstractData* terrama2::core::DataAccessorOccurrenceWfp::stringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes,
    int /*dstType*/, const std::string& timezone) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getAsString(indexes[0]);

    boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

    te::dt::TimeInstantTZ* dt = new te::dt::TimeInstantTZ(date);

    return dt;
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error";
  }

  return nullptr;
}

te::dt::AbstractData* terrama2::core::DataAccessorOccurrenceWfp::stringToPoint(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes,
    int dstType, const Srid& srid) const
{
  assert(dataset);
  assert(indexes.size() == 2);

  te::dt::AbstractData* point = te::da::XYToPointConverter(dataset, indexes, dstType);
  static_cast<te::gm::Point*>(point)->setSRID(srid);

  return point;
}
