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

// TerraMA2
#include "ParserOGR.hpp"
#include "ParserPcdInpe.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "../core/Logger.hpp"

// QT
#include <QDir>
#include <QDebug>

// STL
#include <memory>
#include <algorithm>
#include <iostream>

// Boost
#include <boost/date_time/local_time/local_time_types.hpp>
#include <boost/date_time/local_time/posix_time_zone.hpp>
#include <boost/date_time/time_zone_base.hpp>
#include <boost/format/exceptions.hpp>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/common/Exception.h>

#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/TimeInstantTZ.h>

te::dt::AbstractData* terrama2::collector::ParserPcdInpe::StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType)
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getAsString(indexes[0]);
    boost::posix_time::ptime boostDate;

    //mask to convert DateTime string to Boost::ptime
    std::locale format(std::locale::classic(), new boost::posix_time::time_input_facet("%m/%d/%Y %H:%M:%S"));

    std::istringstream stream(dateTime);//create stream
    stream.imbue(format);//set format
    stream >> boostDate;//convert to boost::ptime

    assert(boostDate != boost::posix_time::ptime());

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
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error at terrama2::collector::ParserPcdInpe::StringToTimestamp";
  }

  return nullptr;
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

void terrama2::collector::ParserPcdInpe::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType)
{
  for(std::size_t i = 0, size = datasetType->size(); i < size; ++i)
  {
    te::dt::Property* p = datasetType->getProperty(i);
    te::dt::Property* propertyClone = p->clone();

    std::string name = propertyClone->getName();
    size_t dotPos = name.find('.');

    if(dotPos != std::string::npos)
    {
      name.erase(std::remove_if(name.begin(), name.begin()+dotPos, &isdigit), name.begin()+dotPos);
      name.erase(0,1);

      propertyClone->setName(name);
    }

    converter->add(i, propertyClone);
  }
}

