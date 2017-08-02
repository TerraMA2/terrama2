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
#include "../core/utility/Utils.hpp"
#include "../core/utility/Unpack.hpp"
#include "../core/utility/Verify.hpp"
#include "../core/utility/DataAccessorFactory.hpp"

//STL
#include <algorithm>
#include <limits>

//QT
#include <QUrl>
#include <QDir>
#include <QSet>

//terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/geometry/Utils.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/geometry/Envelope.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Band.h>


std::string terrama2::core::DataAccessorFile::retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataset, const Filter& filter, std::shared_ptr<terrama2::core::FileRemover> remover) const
{
  std::string mask = getFileMask(dataset);
  std::string folderPath = getFolderMask(dataset);

  std::string timezone = "";
  try
  {
    timezone = getTimeZone(dataset);
  }
  catch(UndefinedTagException& /*e*/)
  {
    // Do nothing
  }

  return dataRetriever->retrieveData(mask, filter, timezone, remover, "", folderPath);
}

std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorFile::createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const
{
  return internalCreateCompleteDataSet(dataSetType, false, false);
}

std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorFile::internalCreateCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType, bool enableFileName, bool enableFileTimestamp) const
{
  if(enableFileName)
  {
    te::dt::Property* filename = new te::dt::SimpleProperty("filename", te::dt::STRING);
    dataSetType->add(filename);
  }

  if(enableFileTimestamp)
  {
    te::dt::Property* timestamp = new te::dt::DateTimeProperty("file_timestamp", te::dt::TIME_INSTANT_TZ);
    dataSetType->add(timestamp);
  }

  return std::make_shared<te::mem::DataSet>(dataSetType.get());
}

void terrama2::core::DataAccessorFile::filterDataSet(std::shared_ptr<te::mem::DataSet> completeDataSet, const Filter& filter) const
{
  size_t dateColumn = te::da::GetFirstPropertyPos(completeDataSet.get(), te::dt::DATETIME_TYPE);
  size_t geomColumn = te::da::GetFirstPropertyPos(completeDataSet.get(), te::dt::GEOMETRY_TYPE);
  size_t rasterColumn = te::da::GetFirstPropertyPos(completeDataSet.get(), te::dt::RASTER_TYPE);

  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > seriesStaticData;
  // Apply filter by static data
  if(filter.dataProvider && filter.dataSeries)
  {
    auto dataAccesor = DataAccessorFactory::getInstance().make(filter.dataProvider, filter.dataSeries);

    terrama2::core::Filter emptyFilter;
    seriesStaticData = dataAccesor->getSeries(emptyFilter, nullptr);

    if(seriesStaticData.empty())
    {
      QString errMsg = QObject::tr("No data available for data series '%1'.").arg(filter.dataSeries->name.c_str());
      TERRAMA2_LOG_WARNING() << errMsg;
      throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
    }
  }

  size_t size = completeDataSet->size();
  size_t i = 0;

  while(i < size)
  {
    completeDataSet->move(i);
    if(!isValidTimestamp(completeDataSet, filter, dateColumn)
        || !isValidGeometry(completeDataSet, filter, geomColumn, seriesStaticData)
        || !isValidRaster(completeDataSet, filter, rasterColumn, seriesStaticData))
    {
      completeDataSet->remove();
      --size;
      continue;
    }

    ++i;
  }
}

void terrama2::core::DataAccessorFile::filterDataSetByLastValues(std::shared_ptr<te::mem::DataSet> completeDataSet,
    const Filter& filter) const
{
  if(!filter.lastValues)
    return;

  auto propertiesNumber = completeDataSet->getNumProperties();

  size_t dateColumn = std::numeric_limits<size_t>::max();
  for(size_t i = 0; i < propertiesNumber; ++i)
  {
    if(!isValidColumn(dateColumn) && completeDataSet->getPropertyDataType(i) == te::dt::DATETIME_TYPE)
    {
      dateColumn = i;
      continue;
    }
  }

  if(!isValidColumn(dateColumn))
    return;

  std::vector<std::shared_ptr< te::dt::TimeInstantTZ > > vecLastValues;

  // Retrieve all dates of dataset
  completeDataSet->moveBeforeFirst();

  while(completeDataSet->moveNext())
  {
    if(completeDataSet->isNull(dateColumn))
    {
      QString errMsg = QObject::tr("Null date/time attribute.");
      TERRAMA2_LOG_WARNING() << errMsg;
      continue;
    }

    std::shared_ptr< te::dt::DateTime > dateTime(completeDataSet->getDateTime(dateColumn));
    auto timeInstant = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(dateTime);

    // Order ASC
    auto it = std::lower_bound(vecLastValues.begin(), vecLastValues.end(), timeInstant,
                               [&](std::shared_ptr< te::dt::TimeInstantTZ > const& first, std::shared_ptr<te::dt::TimeInstantTZ > const& second)
                               {
                                 return *first < *second;
                               });

    if(it != vecLastValues.end() && **it == *dateTime)
      continue;

    vecLastValues.insert(it, timeInstant);
  }

  // Remove uneccessary oldest dates
  auto filterLastValues = *filter.lastValues.get();

  if(vecLastValues.size() > filterLastValues)
    vecLastValues = {vecLastValues.rbegin(), vecLastValues.rbegin()+filterLastValues};

  size_t size = completeDataSet->size();
  size_t i = 0;

  while(i < size)
  {
    completeDataSet->move(i);

    if(completeDataSet->isNull(dateColumn))
    {
      QString errMsg = QObject::tr("Null date/time attribute.");
      TERRAMA2_LOG_WARNING() << errMsg;
      ++i;
      continue;
    }


    std::shared_ptr< te::dt::DateTime > dateTime(completeDataSet->getDateTime(dateColumn));
    auto timesIntant = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(dateTime);

    bool found = false;
    for(size_t j =0; j < *filter.lastValues.get() && j < vecLastValues.size(); ++j)
    {
      auto value = vecLastValues[j];

      if (*timesIntant == *value)
      {
        found = true;
      }
    }

    if(!found)
    {
      completeDataSet->remove();
      --size;
      continue;
    }

    ++i;
  }
}

bool terrama2::core::DataAccessorFile::isValidTimestamp(std::shared_ptr<te::mem::DataSet> dataSet, const Filter& filter, size_t dateColumn) const
{
  if(!isValidColumn(dateColumn) || (!filter.discardBefore.get() && !filter.discardAfter.get()))
    return true;

  if(dataSet->isNull(dateColumn))
  {
    QString errMsg = QObject::tr("Null date/time attribute.");
    TERRAMA2_LOG_WARNING() << errMsg;
    return true;
  }

  std::shared_ptr< te::dt::DateTime > dateTime(dataSet->getDateTime(dateColumn));
  auto timesIntant = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(dateTime);

  if(filter.discardBefore.get() && !((*timesIntant) > (*filter.discardBefore)))
    return false;

  if(filter.discardAfter.get() && !((*timesIntant) < (*filter.discardAfter)))
    return false;

  return true;
}

bool terrama2::core::DataAccessorFile::isValidGeometry(std::shared_ptr<te::mem::DataSet> dataSet, const Filter& filter, size_t geomColumn, std::unordered_map<DataSetPtr, DataSetSeries>& seriesStaticData) const
{
  if(!isValidColumn(geomColumn))
    return true;

  if(dataSet->isNull(geomColumn))
  {
    QString errMsg = QObject::tr("Null geometry attribute.");
    TERRAMA2_LOG_WARNING() << errMsg;
    return true;
  }


  std::shared_ptr< te::gm::Geometry > region(dataSet->getGeometry(geomColumn));
  // Apply filter by area
  if(filter.region)
  {
    std::shared_ptr< te::gm::Geometry > filterRegion(static_cast<te::gm::Geometry*>(filter.region->clone()));
    filterRegion->transform(region->getSRID());
    if(!region->intersects(filterRegion.get()))
      return false;
  }


  if(!seriesStaticData.empty())
  {
    for(auto& it : seriesStaticData)
    {
      auto syncDs = it.second.syncDataSet;
      std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(syncDs->dataset().get(), te::dt::GEOMETRY_TYPE);
      te::gm::GeometryProperty* property = dynamic_cast<te::gm::GeometryProperty*>(it.second.teDataSetType->getProperty(geomPropertyPosition));

      auto extentDs = syncDs->getExtent(geomPropertyPosition);
      std::shared_ptr<te::gm::Geometry> envelopeGeom(te::gm::GetGeomFromEnvelope(extentDs.get(), property->getSRID()));
      region->transform(property->getSRID());
      if(region->intersects(envelopeGeom.get()))
      {
        for(unsigned int j = 0; j < syncDs->size(); ++j)
        {
          auto geom = syncDs->getGeometry(j, geomPropertyPosition);
          if(region->intersects(geom->getEnvelope()))
          {
            if (region->intersects(geom.get()))
            {
              return true;
            }
          }
        }
      }
    }

    return false;
  }
  return true;

}

bool terrama2::core::DataAccessorFile::isValidRaster(std::shared_ptr<te::mem::DataSet> dataSet, const Filter&  filter, size_t rasterColumn, std::unordered_map<DataSetPtr, DataSetSeries>& seriesStaticData) const
{
  if(!isValidColumn(rasterColumn))
    return true;

  if(dataSet->isNull(rasterColumn))
  {
    QString errMsg = QObject::tr("Null raster attribute.");
    TERRAMA2_LOG_WARNING() << errMsg;
    return true;
  }

  std::shared_ptr< te::rst::Raster > raster(dataSet->getRaster(rasterColumn));

  //if filter by regio/box
  if(filter.region.get())
  {
    auto envelope = filter.region->getMBR();
    if(!raster->getExtent(filter.region->getSRID())->intersects(*envelope))
      return false;

    if(filter.cropRaster)
    {
      //clip raster by box
      auto clipedRaster = raster->clip({filter.region.get()}, {}, "EXPANSIBLE");
      //update raster in the dataset
      dataSet->setRaster(rasterColumn, clipedRaster);
      //update raster for consistency in the function
      raster = std::shared_ptr< te::rst::Raster >(dataSet->getRaster(rasterColumn));
    }
  }

  //if filter by dataseries
  if(!seriesStaticData.empty())
  {
    for(auto& it : seriesStaticData)
    {
      std::shared_ptr<te::gm::Geometry> rasterBox(te::gm::GetGeomFromEnvelope(raster->getExtent(raster->getSRID()), raster->getSRID()));

      auto syncDs = it.second.syncDataSet;
      std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(syncDs->dataset().get(), te::dt::GEOMETRY_TYPE);

      auto teDataset = syncDs->dataset();
      if(teDataset->moveBeforeFirst())
      {
        //create a union of all geometries of the dataset
        std::shared_ptr<te::gm::Geometry> unitedGeom;
        bool first = true;
        while (teDataset->moveNext())
        {
          if(teDataset->isNull(geomPropertyPosition))
            continue;

          if(first)
          {
            unitedGeom = std::shared_ptr<te::gm::Geometry>(teDataset->getGeometry(geomPropertyPosition));
            first = false;
            continue;
          }

          auto geom(teDataset->getGeometry(geomPropertyPosition));
          unitedGeom = std::shared_ptr<te::gm::Geometry>(unitedGeom->Union(geom.release()));
        }

        unitedGeom->transform(rasterBox->getSRID());
        //check if the raster intersects the data
        if(!rasterBox->intersects(unitedGeom.get()))
          return false;

        //crop the raster using the dataset
        if(filter.cropRaster)
        {
          //clip raster by union
          auto clipedRaster = raster->clip({unitedGeom.get()}, {}, "EXPANSIBLE");
          //update raster in the dataset
          dataSet->setRaster(rasterColumn, clipedRaster);
          //update raster for consistency in the function
          raster = std::shared_ptr< te::rst::Raster >(dataSet->getRaster(rasterColumn));
        }

        return true;
      }
      else
      {
        QString errMsg = QObject::tr("Invalid filter dataseries.");
        TERRAMA2_LOG_WARNING() << errMsg;
        throw terrama2::core::DataAccessException() << ErrorDescription(errMsg);
      }
    }

    return false;
  }

  return true;
}


void terrama2::core::DataAccessorFile::addToCompleteDataSet(terrama2::core::DataSetPtr /*dataSet*/,
                                                            std::shared_ptr<te::mem::DataSet> completeDataSet,
                                                            std::shared_ptr<te::da::DataSet> teDataSet,
                                                            std::shared_ptr< te::dt::TimeInstantTZ > /*fileTimestamp*/,
                                                            const std::string& /*filename*/) const
{
  completeDataSet->copy(*teDataSet);
}

std::shared_ptr<te::da::DataSet> terrama2::core::DataAccessorFile::getTerraLibDataSet(std::shared_ptr<te::da::DataSourceTransactor> transactor,
    const std::string& dataSetName,
    std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  std::unique_ptr<te::da::DataSet> datasetOrig(transactor->getDataSet(dataSetName));
  return std::shared_ptr<te::da::DataSet>(te::da::CreateAdapter(datasetOrig.release(), converter.get(), true));
}


std::vector<std::string>  terrama2::core::DataAccessorFile::getFoldersList(const std::vector<std::string>& uris,
                                                                           const std::string& foldersMask) const
{
  std::vector<std::string> maskList = splitString(foldersMask, '/');

  if(maskList.empty())
    return uris;

  std::vector<std::string> folders = uris;

  for(const auto& mask : maskList)
  {
    if(!mask.empty())
      folders = checkSubfolders(folders, mask);
  }

  if(folders.empty())
  {
    QString errMsg = QObject::tr("No directory matches the mask.");
    TERRAMA2_LOG_ERROR() << errMsg;
    return {};
  }

  return folders;
}


std::vector<std::string> terrama2::core::DataAccessorFile::checkSubfolders(const std::vector<std::string>& baseURIs, const std::string& mask) const
{
  std::vector<std::string> folders;

  for(const auto& uri : baseURIs)
  {
    QDir dir(QString::fromStdString(uri));
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);
    if(fileInfoList.empty())
    {
      continue;
    }

    for(const auto& fileInfo : fileInfoList)
    {
      std::string folderPath = fileInfo.absoluteFilePath().toStdString();

      std::string folder = folderPath.substr(folderPath.find_last_of('/')+1);

      if(!terramaMaskMatch(mask, folder))
        continue;

      folders.push_back(fileInfo.absoluteFilePath().toStdString());
    }
  }

  return folders;
}


QFileInfoList terrama2::core::DataAccessorFile::getDataFileInfoList(const std::string& absoluteFolderPath,
                                                                    const std::string& mask,
                                                                    const std::string& timezone,
                                                                    const Filter& filter,
                                                                    std::shared_ptr<terrama2::core::FileRemover> remover) const
{
  QDir dir(QString::fromStdString(absoluteFolderPath));
  QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);
  if(fileInfoList.empty())
    return QFileInfoList();

  boost::local_time::local_date_time noTime(boost::local_time::not_a_date_time);

  QSet<QString> pathSet;
  std::string tempFolderPath;
  //fill file list
  QFileInfoList newFileInfoList;
  for(const auto& fileInfo : fileInfoList)
  {
    std::string name = fileInfo.fileName().toStdString();
    std::string folderPath = dir.absolutePath().toStdString();


    std::shared_ptr< te::dt::TimeInstantTZ > thisFileTimestamp = std::make_shared<te::dt::TimeInstantTZ>(noTime);
    // Verify if the file name matches the mask
    if(!isValidDataSetName(mask, filter, timezone, name, thisFileTimestamp))
      continue;

    if(terrama2::core::Unpack::isCompressed(folderPath+ "/" + name))
    {
      //unpack files
      tempFolderPath = terrama2::core::Unpack::decompress(folderPath+ "/" + name, remover, tempFolderPath);
      QDir tempDir(QString::fromStdString(tempFolderPath));
      QFileInfoList fileList = tempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);

      // TODO: verify if the uncompressed files matches the mask?
      for(const auto& fileI : fileList)
        pathSet.insert(fileI.absoluteFilePath());
    }
    else
    {
      pathSet.insert(fileInfo.absoluteFilePath());
    }
  }

  for(const auto& filePath : pathSet)
    newFileInfoList.append(QFileInfo(filePath));

  return newFileInfoList;
}

bool terrama2::core::DataAccessorFile::hasControlFile() const
{
  return false;
}

bool terrama2::core::DataAccessorFile::needToOpenConfigFile() const
{
  return false;
}

std::string terrama2::core::DataAccessorFile::getConfigFilename(terrama2::core::DataSetPtr /*dataSet*/, const std::string& /*binaryFilename*/) const
{
  QString errMsg = QObject::tr("Should be overridden in subclass.");
  throw terrama2::core::Exception() << ErrorDescription(errMsg);
}

terrama2::core::DataSetSeries terrama2::core::DataAccessorFile::getSeries(const std::string& uri,
                                                                          const terrama2::core::Filter& filter,
                                                                          terrama2::core::DataSetPtr dataSet,
                                                                          std::shared_ptr<terrama2::core::FileRemover> remover) const
{
  boost::local_time::local_date_time noTime(boost::local_time::not_a_date_time);
  std::shared_ptr< te::dt::TimeInstantTZ > lastFileTimestamp = std::make_shared<te::dt::TimeInstantTZ>(noTime);

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

  DataSetSeries series;
  series.dataSet = dataSet;

  std::shared_ptr<te::mem::DataSet> completeDataset = generateDataSet(uri, filter, dataSet, remover, timezone, series, lastFileTimestamp);
  if(!completeDataset.get() || completeDataset->isEmpty())
  {
    QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
  }

  applyFilters(filter, dataSet, completeDataset, lastFileTimestamp);
  if(!completeDataset.get() || completeDataset->isEmpty())
  {
    QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
  }


  std::shared_ptr<SynchronizedDataSet> syncDataset(new SynchronizedDataSet(completeDataset));
  series.syncDataSet = syncDataset;
  return series;
}



std::shared_ptr<te::dt::TimeInstantTZ>
terrama2::core::DataAccessorFile::readFile(DataSetSeries& series,
                                           std::shared_ptr<te::mem::DataSet>& completeDataset,
                                           std::shared_ptr<te::da::DataSetTypeConverter>& converter,
                                           QFileInfo fileInfo,
                                           const std::string& mask,
                                           terrama2::core::DataSetPtr dataSet) const
{

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

  std::string name = fileInfo.fileName().toStdString();
  std::string baseName = fileInfo.baseName().toStdString();
  std::string completeBaseName = fileInfo.completeBaseName().toStdString();

  std::shared_ptr<te::dt::TimeInstantTZ> thisFileTimestamp = getFileTimestamp(mask, timezone, name);

  if(needToOpenConfigFile())
  {
    std::string configFilename = getConfigFilename(dataSet, fileInfo.absoluteFilePath().toStdString());

    QFileInfo confiFileInfo(configFilename.c_str());
    name = confiFileInfo.fileName().toStdString();
  }


  std::string fileUri = "file://"+typePrefix() + fileInfo.absolutePath().toStdString() + "/" + name;

  // creates a DataSource to the data and filters the dataset,
  // also joins if the DCP comes from separated files
  std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceType(), fileUri));

  //RAII for open/closing the datasource
  OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

  if(!datasource->isOpened())
  {
    // Can't throw here, inside loop
    // just log and continue
    QString errMsg = QObject::tr("DataProvider could not be opened.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  // get a transactor to interact to the data source
  std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

  // Some drivers use the base name and other use filename with extension
  std::string dataSetName;
  std::vector<std::string> dataSetNames = transactor->getDataSetNames();

  auto itCompleteBaseName= std::find(dataSetNames.cbegin(), dataSetNames.cend(), completeBaseName);
  auto itBaseName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), baseName);
  auto itFileName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), name);
  if(itBaseName != dataSetNames.cend())
    dataSetName = baseName;
  else if(itCompleteBaseName != dataSetNames.cend())
    dataSetName = completeBaseName;
  else if(itFileName != dataSetNames.cend())
    dataSetName = name;
  else
    dataSetName = name;

  // TODO: Some raster files (.env) don't appear in the getDataSetNames()
  // but we can open directly with the file name.
  // should we check or just continue with the file name?


  if(!completeDataset)
  {
    //read and adapt all te:da::DataSet from terrama2::core::DataSet
    converter = getConverter(dataSet, std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(dataSetName)));
    series.teDataSetType.reset(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
    assert(series.teDataSetType.get());
    completeDataset = createCompleteDataSet(series.teDataSetType);
  }

  std::shared_ptr<te::da::DataSet> teDataSet = getTerraLibDataSet(transactor, dataSetName, converter);
  if(!teDataSet)
  {
    QString errMsg = QObject::tr("Could not read dataset: %1").arg(dataSetName.c_str());
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }


  addToCompleteDataSet(dataSet, completeDataset, teDataSet, thisFileTimestamp, fileInfo.absoluteFilePath().toStdString());


  return thisFileTimestamp;
}


std::shared_ptr<te::dt::TimeInstantTZ> terrama2::core::DataAccessorFile::readFilesAndAddToDataset(DataSetSeries& series, std::shared_ptr<te::mem::DataSet>& completeDataset, QFileInfoList fileList, const std::string& mask, terrama2::core::DataSetPtr dataSet) const
{
  if(fileList.empty())
  {
    QString errMsg = QObject::tr("No file found for DataSet %1.").arg(dataSet->id);
    TERRAMA2_LOG_WARNING() << errMsg;
  }
  std::shared_ptr<te::da::DataSetTypeConverter> converter(nullptr);

  boost::local_time::local_date_time noTime(boost::local_time::not_a_date_time);
  std::shared_ptr<te::dt::TimeInstantTZ> lastFileTimestamp = std::make_shared<te::dt::TimeInstantTZ>(noTime);

  for(const auto& fileInfo : fileList)
  {
    // Only access the env files, gdal access the hdr
    if(fileInfo.suffix() == "hdr")
      continue;

    std::shared_ptr<te::dt::TimeInstantTZ> thisFileTimestamp;
    try
    {
      thisFileTimestamp = readFile(series, completeDataset, converter, fileInfo, mask, dataSet);
    }
    catch(const terrama2::core::UndefinedTagException&)
    {
      throw;
    }
    catch(const terrama2::core::DataAccessorException&)
    {
      continue;
    }

    //update lastest file timestamp
    if(!lastFileTimestamp.get() || lastFileTimestamp->getTimeInstantTZ().is_special() || *lastFileTimestamp < *thisFileTimestamp)
      lastFileTimestamp = thisFileTimestamp;


  }// for each file

  return lastFileTimestamp;
}

void terrama2::core::DataAccessorFile::applyFilters(const terrama2::core::Filter &filter, const terrama2::core::DataSetPtr &dataSet,
                                    const std::shared_ptr<te::mem::DataSet> &completeDataset,
                                    std::shared_ptr<te::dt::TimeInstantTZ> &lastFileTimestamp) const
{
  filterDataSet(completeDataset, filter);

  //Get last data timestamp and compare with file name timestamp
  std::shared_ptr<te::dt::TimeInstantTZ> dataTimeStamp = getDataLastTimestamp(dataSet, completeDataset);

  filterDataSetByLastValues(completeDataset, filter);

  //if both dates are valid
  if((lastFileTimestamp.get() && !lastFileTimestamp->getTimeInstantTZ().is_special())
     && (dataTimeStamp.get() && !dataTimeStamp->getTimeInstantTZ().is_special()))
  {
    (*lastDateTime_) = *dataTimeStamp > *lastFileTimestamp ? *dataTimeStamp : *lastFileTimestamp;
  }
  else if(lastFileTimestamp.get() && !lastFileTimestamp->getTimeInstantTZ().is_special())
  {
    //if only fileTimestamp is valid
    (*lastDateTime_) = *lastFileTimestamp;
  }
  else if(dataTimeStamp.get() && !dataTimeStamp->getTimeInstantTZ().is_special())
  {
    //if only dataTimeStamp is valid
    (*lastDateTime_) = *dataTimeStamp;
  }
  else
  {
    boost::local_time::local_date_time noTime(boost::local_time::not_a_date_time);
    (*lastDateTime_) = te::dt::TimeInstantTZ(noTime);
  }
}


std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::DataAccessorFile::getDataLastTimestamp(DataSetPtr dataSet, std::shared_ptr<te::da::DataSet> teDataSet) const
{
  size_t propertiesNumber = teDataSet->getNumProperties();
  size_t dateColumn = std::numeric_limits<size_t>::max();
  for(size_t i = 0; i < propertiesNumber; ++i)
  {
    if(!isValidColumn(dateColumn) && teDataSet->getPropertyDataType(i) == te::dt::DATETIME_TYPE)
    {
      dateColumn = i;
      break;
    }
  }

  if(!isValidColumn(dateColumn))
  {
    boost::local_time::local_date_time boostTime(boost::posix_time::not_a_date_time);
    return std::make_shared<te::dt::TimeInstantTZ>(boostTime);
  }

  std::shared_ptr< te::dt::DateTime > lastDateTime;

  teDataSet->moveBeforeFirst();
  while(teDataSet->moveNext())
  {
    if(teDataSet->isNull(dateColumn))
      continue;

    std::shared_ptr< te::dt::DateTime > dateTime(teDataSet->getDateTime(dateColumn));
    if(!lastDateTime.get() || *lastDateTime < *dateTime)
      lastDateTime = dateTime;
  }

  if(!lastDateTime.get())
  {
    boost::local_time::local_date_time boostTime(boost::posix_time::not_a_date_time);
    return std::make_shared<te::dt::TimeInstantTZ>(boostTime);
  }

  std::shared_ptr< te::dt::TimeInstantTZ > lastDateTimeTz;
  if(lastDateTime->getDateTimeType() == te::dt::TIME_INSTANT)
  {
    //NOTE: Depends on te::dt::TimeInstant toString implementation, it's doc is wrong
    std::string dateString = lastDateTime->toString();

    boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateString));
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(getTimeZone(dataSet)));

    boost::local_time::local_date_time boostLocalTime(boostDate, zone);
    lastDateTimeTz = std::make_shared<te::dt::TimeInstantTZ>(boostLocalTime);
  }
  else if(lastDateTime->getDateTimeType() == te::dt::TIME_INSTANT_TZ)
  {
    lastDateTimeTz = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(lastDateTime);
  }
  else
  {
    //This method expects a valid Date/Time, other formats are not valid.
    QString errMsg = QObject::tr("Unknown date format.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  return lastDateTimeTz;
}

QFileInfoList terrama2::core::DataAccessorFile::getFilesList(const std::string& uri,
                                                             const std::string& fileMask,
                                                             const std::string& folderMask,
                                                             const Filter& filter,
                                                             const std::string& timezone,
                                                             std::shared_ptr<terrama2::core::FileRemover> remover) const
{
  QUrl url(QString::fromStdString(uri));

  std::vector<std::string> basePathList;
  basePathList.push_back(url.path().toStdString());

  if(!folderMask.empty())
  {
    std::vector<std::string> foldersList = getFoldersList(basePathList, folderMask);

    if(foldersList.empty())
    {
      QString errMsg = QObject::tr("No files found for dataset!");
      TERRAMA2_LOG_WARNING() << errMsg;
      throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
    }

    basePathList = foldersList;
  }

  QFileInfoList newFileInfoList;

  //fill file list
  for(auto& folderPath : basePathList)
  {
    newFileInfoList.append(getDataFileInfoList(folderPath,
                                               fileMask,
                                               timezone,
                                               filter,
                                               remover));
  }

  return newFileInfoList;
}

std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorFile::generateDataSet(const std::string& uri,
                                                                                    const terrama2::core::Filter& filter,
                                                                                    terrama2::core::DataSetPtr dataSet,
                                                                                    std::shared_ptr<terrama2::core::FileRemover> remover,
                                                                                    const std::string& timezone,
                                                                                    DataSetSeries& series,
                                                                                    std::shared_ptr< te::dt::TimeInstantTZ >& lastFileTimestamp) const
{
  std::shared_ptr<te::mem::DataSet> completeDataset(nullptr);

  std::string fileMask = getFileMask(dataSet);
  std::string folderMask = getFolderMask(dataSet);
  auto binaryFileList = getFilesList(uri, fileMask, folderMask, filter, timezone, remover);
  lastFileTimestamp = readFilesAndAddToDataset(series, completeDataset, binaryFileList, fileMask, dataSet);

  return completeDataset;
}
