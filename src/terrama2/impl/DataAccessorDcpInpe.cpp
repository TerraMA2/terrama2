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
  \file terrama2/core/data-access/DataAccessorDcpInpe.cpp

  \brief

  \author Jano Simas
 */

//TerraMA2
#include "DataAccessorDcpInpe.hpp"
#include "../core/data-access/DataRetriever.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/FilterUtils.hpp"

//terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>

//Qt
#include <QObject>
#include <QString>
#include <QDir>
#include <QUrl>
#include <QFileInfoList>

terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
 : DataAccessor(dataProvider, dataSeries),
   DataAccessorDcp(dataProvider, dataSeries),
   DataAccessorFile(dataProvider, dataSeries)
{
  if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::dataSourceType() const
{
  return "OGR";
}

std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::typePrefix() const
{
  return "CSV:";
}

te::dt::AbstractData* terrama2::core::DataAccessorDcpInpe::stringToTimestamp(te::da::DataSet* dataset,
                                                                             const std::vector<std::size_t>& indexes,
                                                                             int /*dstType*/,
                                                                             const std::string& timezone) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getAsString(indexes[0]);
    boost::posix_time::ptime boostDate;

    //mask to convert DateTime string to Boost::ptime
    std::locale format(std::locale(), new boost::posix_time::time_input_facet("%m/%d/%Y %H:%M:%S"));

    std::istringstream stream(dateTime);//create stream
    stream.imbue(format);//set format
    stream >> boostDate;//convert to boost::ptime

    assert(boostDate != boost::posix_time::ptime());

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

    te::dt::TimeInstantTZ* dt = new te::dt::TimeInstantTZ(date);

    return dt;
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error";
  }

  return nullptr;
}

void terrama2::core::DataAccessorDcpInpe::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  //only one timestamp column
  std::string timestampPropertyName = getInputTimestampPropertyName(dataSet);
  std::string outputTimestampPropertyName = getTimestampPropertyName(dataSet);

  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(outputTimestampPropertyName, te::dt::TIME_INSTANT_TZ);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == timestampPropertyName)
    {
      // datetime column found
      converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorDcpInpe::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet)));
    }
    else
    {
      // DCP-INPE dataset columns have the name of the dcp before every column,
      // remove the name and keep only the column name
      te::dt::Property* property = properties.at(i);

      std::string name = property->getName();
      size_t dotPos = name.find('.');

      if(dotPos != std::string::npos)
        name.erase(0,dotPos + 1);


      te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(name, te::dt::DOUBLE_TYPE);
      converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
    }
  }

}

void terrama2::core::DataAccessorDcpInpe::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> /*converter*/, const std::shared_ptr<te::da::DataSetType>& /*datasetType*/) const
{
  // Don't add any columns here,
  // the converter will add columns
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorDcpInpe::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
{
  return std::make_shared<DataAccessorDcpInpe>(dataProvider, dataSeries);
}
