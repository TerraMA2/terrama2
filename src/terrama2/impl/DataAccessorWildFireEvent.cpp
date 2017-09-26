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
  \file terrama2/core/impl/DataAccessorWildFireEvent.cpp

  \brief DataAccessor class for static data accessed via OGR driver.

  \author Paulo R. M. Oliveira
 */

//TerraMA2
#include "DataAccessorWildFireEvent.hpp"

#include "../core/Exception.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"

#include <terralib/datatype/DateTimeProperty.h>

// QT
#include <QObject>

#include <boost/algorithm/string/replace.hpp>

terrama2::core::DataAccessorWildFireEvent::DataAccessorWildFireEvent(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
: DataAccessor(dataProvider, dataSeries, false),
  DataAccessorGeometricObjectOGR(dataProvider, dataSeries, false)
{
 if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
 {
   QString errMsg = QObject::tr("Wrong DataSeries semantics.");
   TERRAMA2_LOG_ERROR() << errMsg;
   throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
 }
}

void terrama2::core::DataAccessorWildFireEvent::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  //only one timestamp column
  std::string timestampPropertyName = getTimestampPropertyName(dataSet);
  std::string outputTimestampPropertyName = getOutputTimestampPropertyName(dataSet);

  std::string geometryPropertyName = getGeometryPropertyName(dataSet);
  std::string outputGeometryPropertyName = getOutputGeometryPropertyName(dataSet);

  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(outputTimestampPropertyName, te::dt::TIME_INSTANT_TZ);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == timestampPropertyName)
    {
      // datetime column found
      converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorWildFireEvent::numberToTimestamp, this, _1, _2, _3, DataAccessorFile::getTimeZone(dataSet)));
    }
    else if(property->getName() == geometryPropertyName)
    {
      auto geomProperty = property->clone();
      geomProperty->setName(outputGeometryPropertyName);
      converter->add(i, geomProperty);
    }
    else
    {
      // future date field, not currently used
      if(property->getName() == "dt")
        continue;

      converter->add(i,property->clone());
    }
  }
}

void terrama2::core::DataAccessorWildFireEvent::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> /*converter*/, const std::shared_ptr<te::da::DataSetType>& /*datasetType*/) const
{
  //columns add by the adapt method
}

te::dt::AbstractData* terrama2::core::DataAccessorWildFireEvent::numberToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/, const std::string& timezone) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getAsString(indexes[0]);
    boost::posix_time::ptime boostDate;

    //mask to convert DateTime string to Boost::ptime
    std::locale format(std::locale(), new boost::posix_time::time_input_facet("%Y%m%d"));

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

std::string terrama2::core::DataAccessorWildFireEvent::retrieveData(const DataRetrieverPtr dataRetriever,
                                                                    DataSetPtr dataSet,
                                                                    const Filter& filter,
                                                                    std::shared_ptr<FileRemover> remover) const
{
  std::string mask = getFileMask(dataSet);
  std::string folderPath = getFolderMask(dataSet);

  std::string timezone = "";
  try
  {
    timezone = DataAccessorFile::getTimeZone(dataSet);
  }
  catch(UndefinedTagException& /*e*/)
  {
    // Do nothing
  }

//download shp files
  auto tempFolder =  dataRetriever->retrieveData(mask, filter, timezone, remover, "", folderPath);

//download auxiliary files
  std::string dbfFile = std::string{mask.cbegin(), mask.cend()-3}+"dbf";
  std::string prjFile = std::string{mask.cbegin(), mask.cend()-3}+"prj";
  std::string shxFile = std::string{mask.cbegin(), mask.cend()-3}+"shx";

  dataRetriever->retrieveData(dbfFile, filter, timezone, remover, tempFolder, folderPath);
  dataRetriever->retrieveData(prjFile, filter, timezone, remover, tempFolder, folderPath);
  dataRetriever->retrieveData(shxFile, filter, timezone, remover, tempFolder, folderPath);

  return tempFolder;
}
