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


#include "ParserOGR.hpp"
#include "ParserFirePoint.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"

//QT
#include <QDir>
#include <QDebug>

//STD
#include <memory>

#include <boost/format/exceptions.hpp>

//terralib
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
#include <terralib/datatype/TimeInstant.h>
#include <terralib/geometry.h>

te::dt::AbstractData* XYTo4326PointConverter(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType)
{
  assert(dataset);
  assert(indexes.size() == 2);

  te::dt::AbstractData* point = te::da::XYToPointConverter(dataset, indexes, dstType);

  static_cast<te::gm::Point*>(point)->setSRID(4326);

  return point;
}

te::dt::AbstractData* StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/)
{
  assert(indexes.size() == 1);

  std::string dateTime = dataset->getAsString(indexes[0]);

  te::dt::TimeInstant* dt = new te::dt::TimeInstant(boost::posix_time::ptime(boost::posix_time::time_from_string(dateTime)));

  return dt;
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
  converter->add(dataPos, dtProperty, StringToTimestamp);

  converter->remove(data);
  converter->remove(lat);
  converter->remove(lon);
}
