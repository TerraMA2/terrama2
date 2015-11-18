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
  \file terrama2/collector/ParserPcdToa5.cpp

  \brief Parser of PCD TOA5 file

  \author Jano Simas
  \author Evandro Delatin
*/


#include "ParserOGR.hpp"
#include "ParserPcdToa5.hpp"
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
#include <terralib/datatype/TimeInstant.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype.h>


te::dt::AbstractData* terrama2::collector::ParserPcdToa5::StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType)
{
  assert(indexes.size() == 1);

  std::string dateTime = dataset->getAsString(indexes[0]);

  te::dt::DateTime* dt = new te::dt::TimeInstant(boost::posix_time::ptime(boost::posix_time::time_from_string(dateTime)));

  return dt;
}

// Change the string 2014-01-02 17:13:00 - PCD TOA5 format for timestamp
void terrama2::collector::ParserPcdToa5::adapt(te::da::DataSetTypeConverter&converter)
{
  converter.remove("TIMESTAMP");

  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty("DateTime", te::dt::TIME_INSTANT_TZ);

  converter.add(0, dtProperty, boost::bind(&terrama2::collector::ParserPcdToa5::StringToTimestamp, this, _1, _2, _3));
}
