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
  \file terrama2/impl/DataAccessorDcpToa5.cpp

  \brief The DataAccessorDcpToa5 class is responsible for making the parser file in the format TOA5.

  \author Evandro Delatin
 */

//TerraMA2
#include "DataAccessorDcpToa5.hpp"
#include "../core/data-access/DataRetriever.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/FilterUtils.hpp"

//Terralib
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
#include <QDebug>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QTextStream>


terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const Filter& filter)
 : DataAccessor(dataProvider, dataSeries, filter),
   DataAccessorDcp(dataProvider, dataSeries, filter),
   DataAccessorFile(dataProvider, dataSeries, filter)
{
  if(dataSeries->semantics.code != "DCP-toa5")
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
  }

}

std::string terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5::getRecordPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "record_property");
}

std::string terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5::getStationPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "station_property");
}

std::string terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5::dataSourceType() const
{
  return "OGR";
}
std::string terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5::typePrefix() const
{
  return "CSV:";
}

te::dt::AbstractData* terrama2::core::DataAccessorDcpToa5::stringToTimestamp(te::da::DataSet* dataset,
                                                                             const std::vector<std::size_t>& indexes,
                                                                             int /*dstType*/,
                                                                             const std::string& timezone) const
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

void terrama2::core::DataAccessorDcpToa5::adapt(DataSetPtr dataset, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  //only one timestamp column
  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty("DateTime", te::dt::TIME_INSTANT_TZ);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);

    if (property->getName() == getRecordPropertyName(dataset))
    {
      te::dt::Property* property = properties.at(i);

      std::string name = property->getName();

      te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(name, te::dt::INT32_TYPE);
      converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToInt, this, _1, _2, _3));
    }
    else if (property->getName() == getStationPropertyName(dataset))
    {
      te::dt::Property* property = properties.at(i);

      converter->add(i, property->clone());
    }
    else if(property->getName() == getTimestampPropertyName(dataset))
    {
      // datetime column found
      converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorDcpToa5::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataset)));
    }
    else
    {
      // DCP-TOA5 dataset columns have the name of the dcp before every column,
      // remove the name and keep only the column name
      te::dt::Property* property = properties.at(i);

      std::string name = property->getName();

      te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(name, te::dt::DOUBLE_TYPE);
      converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
    }
  }
}

void terrama2::core::DataAccessorDcpToa5::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  // Don't add any columns here,
  // the converter will add columns
}

terrama2::core::DataSetSeries terrama2::core::DataAccessorDcpToa5::getSeries(const std::string& uri,
                                                                   const terrama2::core::Filter& filter,
                                                                   terrama2::core::DataSetPtr dataSet) const

{
  std::string mask = getMask(dataSet);
  std::string folder = getFolder(dataSet);

  QTemporaryDir tempBaseDir;
  if(!tempBaseDir.isValid())
  {
    QString errMsg = QObject::tr("Can't create temporary folder.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessException() << ErrorDescription(errMsg);
  }

  QDir tempDir(tempBaseDir.path());
  tempDir.mkdir(QString::fromStdString(folder));
  tempDir.cd(QString::fromStdString(folder));

  QUrl url((uri+"/"+folder+"/"+mask).c_str());
  QFileInfo originalInfo(url.path());

  QFile file(url.path());
  QFile tempFile(tempDir.path()+"/"+originalInfo.fileName());
  if(!file.open(QIODevice::ReadOnly))
  {
    QString errMsg = QObject::tr("Can't open file: dataset %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessException() << ErrorDescription(errMsg);
  }

  if(!tempFile.open(QIODevice::ReadWrite))
  {
    QString errMsg = QObject::tr("Can't open temporary file: dataset %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessException() << ErrorDescription(errMsg);
  }

  file.readLine();//ignore first line
  tempFile.write(file.readLine()); //headers line

  //ignore third and fourth lines
  file.readLine();
  file.readLine();

  //read all file
  tempFile.write(file.readAll()); //headers line

  //update file path
  std::string tempUri = "file://"+tempBaseDir.path().toStdString();

  file.close();
  tempFile.close();

  auto dataSeries = terrama2::core::DataAccessorFile::getSeries(tempUri, filter, dataSet);

  return dataSeries;
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorDcpToa5::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const Filter& filter)
{
  return std::make_shared<DataAccessorDcpToa5>(dataProvider, dataSeries, filter);
}
