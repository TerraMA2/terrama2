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


terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
 : DataAccessor(dataProvider, dataSeries, false),
   DataAccessorDcp(dataProvider, dataSeries, false),
   DataAccessorFile(dataProvider, dataSeries, false)
{
  if(checkSemantics && dataSeries->semantics.code != dataAccessorType())
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
                                                                             terrama2::core::DataSetPtr dataSet,
                                                                             std::shared_ptr<FileRemover> remover) const
{
  QUrl url(QString::fromStdString(uri));

  //get timezone of the dataset
  std::string timezone;
  try
  {
    timezone = getTimeZone(dataSet);
  }
  catch(const terrama2::core::UndefinedTagException& /*e*/)
  {
    //if timezone is not defined
    timezone = "UTC+00";
  }

  std::string folderMask;
  try
  {
    folderMask = getFolderMask(dataSet, dataSeries_);
  }
  catch(const terrama2::core::UndefinedTagException& /*e*/)
  {
    folderMask = "";
  }

  QFileInfoList pathList;
  pathList.append(url.toString(QUrl::RemoveScheme));

  if(!folderMask.empty())
  {
    QFileInfoList foldersList = getFoldersList(pathList, folderMask);

    if(foldersList.empty())
    {
      QString errMsg = QObject::tr("No files found for dataset: %1.").arg(dataSet->id);
      TERRAMA2_LOG_WARNING() << errMsg;
      throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
    }

    pathList = foldersList;
  }

  QFileInfoList newFileInfoList;

  //fill file list
  for(auto& folderURI : pathList)
  {
    newFileInfoList.append(getDataFileInfoList(folderURI.absoluteFilePath().toStdString(), getMask(dataSet), timezone, filter, remover));
  }

  if(newFileInfoList.empty())
  {
    QString errMsg = QObject::tr("No files found for dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
  }

  QTemporaryDir tempBaseDir;

  if(!tempBaseDir.isValid())
  {
    QString errMsg = QObject::tr("Can't create temporary folder.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessException() << ErrorDescription(errMsg);
  }

  // create formated TOA 5 files with the same folders structure
  for(auto& file : newFileInfoList)
  {
    QUrl fileUrl("file://" + file.absolutePath());
    QString saveDir = fileUrl.toString(QUrl::RemoveScheme);

    saveDir.replace(url.toString(QUrl::RemoveScheme), tempBaseDir.path());

    readTOA5file(file, saveDir.toStdString());
  }

  return DataAccessorFile::getSeries(tempBaseDir.path().toStdString(), filter, dataSet, remover);
}

void terrama2::core::DataAccessorDcpToa5::readTOA5file(const QFileInfo& fileInfo,
                                                       const std::string& saveUri) const
{
  QDir dir(QString::fromStdString(saveUri));
  if(!dir.exists())
    dir.mkpath(QString::fromStdString(saveUri));

  QFile file(fileInfo.absoluteFilePath());
  QFile tempFile(dir.path()+"/"+fileInfo.fileName());
  if(!file.open(QIODevice::ReadOnly))
  {
    QString errMsg = QObject::tr("Can't open TOA5 file: %1").arg(fileInfo.absoluteFilePath());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessException() << ErrorDescription(errMsg);
  }

  if(!tempFile.open(QIODevice::ReadWrite))
  {
    QString errMsg = QObject::tr("Can't open TOA5 file: %1").arg(dir.path()+"/"+fileInfo.fileName());
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

  file.close();
  tempFile.close();
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorDcpToa5::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
{
  return std::make_shared<DataAccessorDcpToa5>(dataProvider, dataSeries);
}
