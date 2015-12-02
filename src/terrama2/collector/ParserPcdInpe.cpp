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
  \file terrama2/collector/ParserPcdInpe.cpp

  \brief Parser PCD INPE file

  \author Jano Simas
  \author Evandro Delatin
*/


#include "ParserOGR.hpp"
#include "ParserPcdInpe.hpp"
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

#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/geometry.h>
#include <terralib/geometry.h>
#include <terralib/datatype/TimeInstant.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype.h>


#include <iostream>

te::dt::AbstractData* terrama2::collector::ParserPcdInpe::StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType)
{
  assert(indexes.size() == 1);

  std::string dateTime = dataset->getAsString(indexes[0]);
  boost::posix_time::ptime boostDate;

  //mask to convert DateTime string to Boost::ptime
  std::locale format(std::locale::classic(), new boost::posix_time::time_input_facet("%d/%m/%Y %H:%M:%S"));

  std::istringstream stream(dateTime);//create stream
  stream.imbue(format);//set format
  stream >> boostDate;//convert to boost::ptime

  te::dt::DateTime* dt = new te::dt::TimeInstant(boostDate);

  return dt;
}

// Change the string 07/21/2015 17:13:00 - PCD INPE format for timestamp
void terrama2::collector::ParserPcdInpe::adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter)
{
  std::string timestampName = "N/A";
  converter->remove(timestampName);

  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty("DateTime", te::dt::TIME_INSTANT_TZ);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(int i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == timestampName)
    {
      //column found
      converter->add(i, dtProperty, boost::bind(&terrama2::collector::ParserPcdInpe::StringToTimestamp, this, _1, _2, _3));
      break;
    }
  }

}
