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
  \file terrama2/core/impl/DataAccessorGeometricObjectOGR.cpp

  \brief DataAccessor class for static data accessed via OGR driver.

  \author Paulo R. M. Oliveira
 */

//TerraMA2
#include "DataAccessorGeometricObjectOGR.hpp"

#include "../core/Exception.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"

// QT
#include <QObject>

terrama2::core::DataAccessorGeometricObjectOGR::DataAccessorGeometricObjectOGR(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
: DataAccessor(dataProvider, dataSeries),
  DataAccessorGeometricObject(dataProvider, dataSeries),
  DataAccessorFile(dataProvider, dataSeries)
{
 if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
 {
   QString errMsg = QObject::tr("Wrong DataSeries semantics.");
   TERRAMA2_LOG_ERROR() << errMsg;
   throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
 }
}

std::string terrama2::core::DataAccessorGeometricObjectOGR::dataSourceType() const
{
  return "OGR";
}

std::string terrama2::core::DataAccessorGeometricObjectOGR::getTimeZone(DataSetPtr dataSet, bool /*logErrors*/) const
{
  return terrama2::core::DataAccessorFile::getTimeZone(dataSet, false);
}

te::dt::AbstractData* terrama2::core::DataAccessorGeometricObjectOGR::numberToTimestamp(te::da::DataSet* dataset,
                                                                                        const std::vector<std::size_t>& indexes,
                                                                                        int /*dstType*/,
                                                                                        const std::string& timezone,
                                                                                        const std::string& timestampMask)
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getAsString(indexes[0]);
    boost::posix_time::ptime boostDate;

    //mask to convert DateTime string to Boost::ptime
    std::locale format(std::locale(), new boost::posix_time::time_input_facet(timestampMask));

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

std::string terrama2::core::DataAccessorGeometricObjectOGR::getTimestampMask(DataSetPtr dataSet, bool logErrors) const
{
  return getProperty(dataSet, dataSeries_, "timestamp_mask", logErrors);
}

void terrama2::core::DataAccessorGeometricObjectOGR::retrieveDataCallback (const DataRetrieverPtr dataRetriever,
                                                                      DataSetPtr dataset,
                                                                      const Filter& filter,
                                                                      std::shared_ptr<FileRemover> remover,
                                                                      std::function<void(const std::string& /*uri*/)> processFile) const
{
  std::string mask = getFileMask(dataset);
  std::string folderPath = getFolderMask(dataset);

  std::string timezone = "";
  try
  {
    timezone = DataAccessorFile::getTimeZone(dataset);
  }
  catch(UndefinedTagException& /*e*/)
  {
    // Do nothing
  }

  //download shp files
  dataRetriever->retrieveDataCallback(mask, filter, timezone, remover, "", folderPath, [&](const std::string& tempFolder, const std::string& filename) {
    //download auxiliary files
    std::string dbfFile = std::string{filename.cbegin(), filename.cend()-3}+"dbf";
    std::string prjFile = std::string{filename.cbegin(), filename.cend()-3}+"prj";
    std::string shxFile = std::string{filename.cbegin(), filename.cend()-3}+"shx";

    dataRetriever->retrieveDataCallback(dbfFile, filter, timezone, remover, tempFolder, folderPath, [](const std::string&){});
    dataRetriever->retrieveDataCallback(prjFile, filter, timezone, remover, tempFolder, folderPath, [](const std::string&){});
    dataRetriever->retrieveDataCallback(shxFile, filter, timezone, remover, tempFolder, folderPath, processFile);
  });
}

std::string terrama2::core::DataAccessorGeometricObjectOGR::retrieveData(const DataRetrieverPtr dataRetriever,
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
