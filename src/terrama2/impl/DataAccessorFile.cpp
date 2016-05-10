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
  \file terrama2/core/data-access/DataAccessorFile.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorFile.hpp"
#include "../core/utility/FilterUtils.hpp"
#include "../core/utility/TimeUtils.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Raii.hpp"

//STL
#include <algorithm>

//QT
#include <QUrl>
#include <QDir>

//terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/memory/DataSetItem.h>


std::string terrama2::core::DataAccessorFile::getMask(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("mask");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined mask in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorFile::retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataset, const Filter& filter) const
{
  std::string mask = getMask(dataset);
  return dataRetriever->retrieveData(mask, filter);
}

std::shared_ptr<te::da::DataSet> terrama2::core::DataAccessorFile::createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const
{
  return std::make_shared<te::mem::DataSet>(dataSetType.get());
}

void terrama2::core::DataAccessorFile::filterDataSet(std::shared_ptr<te::da::DataSet> completeDataSet, const Filter& filter) const
{
  auto dataSet = std::dynamic_pointer_cast<te::mem::DataSet>(completeDataSet);
  int propertiesNumber = dataSet->getNumProperties();

  int dateColumn = -1;
  int geomColumn = -1;
  int rasterColumn = -1;
  for(int i = 0; i < propertiesNumber; ++i)
  {
    if(dateColumn < 0 && dataSet->getPropertyDataType(i) == te::dt::DATETIME_TYPE)
    {
      dateColumn = i;
      continue;
    }

    if(geomColumn < 0 && dataSet->getPropertyDataType(i) == te::dt::GEOMETRY_TYPE)
    {
      geomColumn = i;
      continue;
    }

    if(rasterColumn < 0 && dataSet->getPropertyDataType(i) == te::dt::RASTER_TYPE)
    {
      geomColumn = i;
      continue;
    }
  }

  int size = dataSet->size();
  int i = 0;

  while(i < size)
  {
    dataSet->move(i);
    if(!isValidTimestamp(dataSet, filter, dateColumn)
        || !isValidGeometry(dataSet, filter, geomColumn)
        || !isValidRaster(dataSet, filter, rasterColumn))
    {
      dataSet->remove();
      --size;
      continue;
    }

    ++i;
  }
}

bool terrama2::core::DataAccessorFile::isValidTimestamp(std::shared_ptr<te::mem::DataSet> dataSet, const Filter& filter, int dateColumn) const
{
  if(dateColumn < 0 || (!filter.discardBefore.get() && !filter.discardAfter.get()))
    return true;

  if(dataSet->isNull(dateColumn))
  {
    QString errMsg = QObject::tr("Null date/time attribute.");
    TERRAMA2_LOG_WARNING() << errMsg;
    return true;
  }

  std::shared_ptr< te::dt::DateTime > dateTime(dataSet->getDateTime(dateColumn));
  auto timesIntant = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(dateTime);

  if(filter.discardBefore.get() && (*timesIntant) < (*filter.discardBefore))
    return false;

  if(filter.discardAfter.get() && (*timesIntant) > (*filter.discardAfter))
    return false;

  return true;
}

bool terrama2::core::DataAccessorFile::isValidGeometry(std::shared_ptr<te::mem::DataSet> dataSet, const Filter& filter, int geomColumn) const
{
  if(geomColumn < 0 || !filter.region.get())
    return true;

  if(dataSet->isNull(geomColumn))
  {
    QString errMsg = QObject::tr("Null geometry attribute.");
    TERRAMA2_LOG_WARNING() << errMsg;
    return true;
  }

  std::shared_ptr< te::gm::Geometry > region(dataSet->getGeometry(geomColumn));

  if(!region->intersects(filter.region.get()))
    return false;

  return true;
}

bool terrama2::core::DataAccessorFile::isValidRaster(std::shared_ptr<te::mem::DataSet> dataSet, const Filter&  filter, int rasterColumn) const
{
  if(rasterColumn < 0 || !filter.region.get())
    return true;

  if(dataSet->isNull(rasterColumn))
  {
    QString errMsg = QObject::tr("Null raster attribute.");
    TERRAMA2_LOG_WARNING() << errMsg;
    return true;
  }

  std::shared_ptr< te::rst::Raster > raster(dataSet->getRaster(rasterColumn));

  std::unique_ptr<const te::gm::Envelope> envelope(filter.region->getMBR());
  if(!raster->getExtent(filter.region->getSRID())->intersects(*envelope))
    return false;

  return true;
}

void terrama2::core::DataAccessorFile::addToCompleteDataSet(std::shared_ptr<te::da::DataSet> completeDataSet, std::shared_ptr<te::da::DataSet> dataSet) const
{
  auto complete = std::dynamic_pointer_cast<te::mem::DataSet>(completeDataSet);
  complete->copy(*dataSet);
}

std::shared_ptr<te::da::DataSet> terrama2::core::DataAccessorFile::getTerraLibDataSet(std::shared_ptr<te::da::DataSourceTransactor> transactor, const std::string& dataSetName, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  std::unique_ptr<te::da::DataSet> datasetOrig(transactor->getDataSet(dataSetName));
  return std::shared_ptr<te::da::DataSet>(te::da::CreateAdapter(datasetOrig.release(), converter.get(), true));
}

terrama2::core::Series terrama2::core::DataAccessorFile::getSeries(const std::string& uri,
                                                                   const terrama2::core::Filter& filter,
                                                                   terrama2::core::DataSetPtr dataSet) const
{
  QUrl url(uri.c_str());
  QDir dir(url.path());
  QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);
  if(fileInfoList.empty())
  {
    QString errMsg = QObject::tr("No file in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw NoDataException() << ErrorDescription(errMsg);
  }

  //return value
  Series series;
  series.dataSet = dataSet;

  std::shared_ptr<te::da::DataSet> completeDataset(nullptr);
  std::shared_ptr<te::da::DataSetTypeConverter> converter(nullptr);

  bool first = true;
  for(const auto& fileInfo : fileInfoList)
  {
    std::string name = fileInfo.fileName().toStdString();
    std::string baseName = fileInfo.baseName().toStdString();
    // Verify if the file name matches the mask
    std::string timezone;//TODO: get timezone
    std::shared_ptr< te::dt::TimeInstantTZ > timestamp;// FIXME: use timestamp
    if(!isValidDataSetName(getMask(dataSet), filter, timezone, name,timestamp))
      continue;

    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceType()));
    std::map<std::string, std::string> connInfo;

    connInfo["URI"] = typePrefix() + dir.absolutePath().toStdString() + "/" + name;
    datasource->setConnectionInfo(connInfo);

    //RAII for open/closing the datasource
    OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    if(!datasource->isOpened())
    {
      // Can't throw here, inside loop
      // just log and continue
      QString errMsg = QObject::tr("DataProvider could not be opened.");
      TERRAMA2_LOG_ERROR() << errMsg;

      continue;
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

    // Some drivers use tha base name and other use filename with extension
    std::string dataSetName;
    std::vector<std::string> dataSetNames = transactor->getDataSetNames();
    auto itBaseName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), baseName);
    auto itFileName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), name);
    if(itBaseName != dataSetNames.cend())
      dataSetName = baseName;
    else if(itFileName != dataSetNames.cend())
      dataSetName = name;
    //No valid dataset name found
    if(dataSetName.empty())
      continue;

    if(first)
    {
      //read and adapt all te:da::DataSet from terrama2::core::DataSet
      converter = getConverter(dataSet, std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(dataSetName)));
      series.teDataSetType.reset(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
      assert(series.teDataSetType.get());
      completeDataset = createCompleteDataSet(series.teDataSetType);
      first = false;
    }

    assert(converter);
    std::shared_ptr<te::da::DataSet> teDataSet = getTerraLibDataSet(transactor, dataSetName, converter);

    addToCompleteDataSet(completeDataset, teDataSet);

    if(completeDataset->isEmpty())
    {
      QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
      TERRAMA2_LOG_WARNING() << errMsg;
    }
  }// for each file

  if(!completeDataset.get())
  {
    QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
  }

  filterDataSet(completeDataset, filter);
  std::shared_ptr< te::dt::TimeInstantTZ > lastTimeStamp = getLastTimestamp(completeDataset);
  (*lastDateTime_) = *lastTimeStamp;//FIXME: compare with file name timestamp

  std::shared_ptr<SyncronizedDataSet> syncDataset(new SyncronizedDataSet(completeDataset));
  series.syncDataSet = syncDataset;
  return series;
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::DataAccessorFile::getLastTimestamp(std::shared_ptr<te::da::DataSet> dataSet) const
{
  int propertiesNumber = dataSet->getNumProperties();
  int dateColumn = -1;
  for(int i = 0; i < propertiesNumber; ++i)
  {
    if(dateColumn < 0 && dataSet->getPropertyDataType(i) == te::dt::DATETIME_TYPE)
    {
      dateColumn = i;
      break;
    }
  }

  std::shared_ptr< te::dt::DateTime > lastDateTime;

  dataSet->moveBeforeFirst();
  while(dataSet->moveNext())
  {
    std::shared_ptr< te::dt::DateTime > dateTime(dataSet->getDateTime(dateColumn));
    if(!lastDateTime.get() || *lastDateTime < *dateTime)
      lastDateTime = dateTime;
  }

  std::shared_ptr< te::dt::TimeInstantTZ > lastDateTimeTz;
  if(lastDateTime->getDateTimeType() == te::dt::TIME_INSTANT)
  {
    //NOTE: Depends on te::dt::TimeInstant toString implementation, it's doc is wrong
    std::string dateString = lastDateTime->toString();
    lastDateTimeTz = terrama2::core::TimeUtils::stringToTimestamp(dateString, "%Y-%m-%d %H:%M:%S");
    //FIXME: add terrama2::DataSet timezone
    //FIXME: not sure this works, need to test
  }
  else if(lastDateTime->getDateTimeType() == te::dt::TIME_INSTANT_TZ)
  {
    lastDateTimeTz = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(lastDateTime);
  }
  else
  {
    QString errMsg = QObject::tr("Unknown date format.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  return lastDateTimeTz;
}
