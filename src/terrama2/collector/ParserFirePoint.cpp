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
  \file terrama2/collector/ParserFirePoint.cpp

  \brief Parser of occurrences of fires file

  \author Jano Simas
  \author Evandro Delatin
*/

// QT
#include <QDir>
#include <QDebug>

// STL
#include <memory>

// Boost
#include <boost/format/exceptions.hpp>

// TerraMA2
#include "ParserOGR.hpp"
#include "ParserFirePoint.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "../core/Logger.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/common/Exception.h>

#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/AttributeConverters.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/geometry.h>

te::dt::AbstractData* XYTo4326PointConverter(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType)
{
  assert(dataset);
  assert(indexes.size() == 2);

  te::dt::AbstractData* point = te::da::XYToPointConverter(dataset, indexes, dstType);
//FIXME: Use datasetItem projection
  static_cast<te::gm::Point*>(point)->setSRID(4326);

  return point;
}
// int dstType
te::dt::AbstractData* terrama2::collector::ParserFirePoint::StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/)
{
  assert(indexes.size() == 1);

  try {
    std::string dateTime = dataset->getAsString(indexes[0]);

    boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(dataSetItem_.timezone()));
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

void terrama2::collector::ParserFirePoint::adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter)
{

  int latPos = -1, lonPos = -1, dataPos = -1;
  std::string lat("lat"), lon("lon"), data("data_pas");

  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(int i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    std::string name = property->getName();

    if(name == lat)
      latPos = i;

    if(name == lon)
      lonPos = i;

    if(name == data)
      dataPos = i;
  }

  assert(latPos != -1);
  assert(lonPos != -1);
  assert(dataPos != -1);

  // Generates a point through the x and y coordinates
  te::gm::GeometryProperty* gm = new te::gm::GeometryProperty("geom", 4326, te::gm::PointType);

  std::vector<size_t> latLonAttributes;
  latLonAttributes.push_back(lonPos);
  latLonAttributes.push_back(latPos);
  converter->add(latLonAttributes ,gm, XYTo4326PointConverter);  
  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty("DateTime", te::dt::TIME_INSTANT_TZ);
  converter->add(dataPos, dtProperty, boost::bind(&terrama2::collector::ParserFirePoint::StringToTimestamp, this, _1, _2, _3));

  converter->remove(data);
  converter->remove(lat);
  converter->remove(lon);
}
