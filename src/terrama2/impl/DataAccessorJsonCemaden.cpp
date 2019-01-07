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
  \file terrama2/core/data-access/DataAccessorJsonCemaden.cpp

  \brief

  \author Jano Simas
 */


 #include "DataAccessorJsonCemaden.hpp"
 #include "../core/utility/DataRetrieverFactory.hpp"
 #include "../core/utility/Utils.hpp"
 #include "../core/utility/TimeUtils.hpp"
 #include "../core/Exception.hpp"

 #include <terralib/core/uri/URI.h>
 #include <terralib/datatype/DateTimeProperty.h>
 #include <terralib/datatype/SimpleProperty.h>
 #include <terralib/memory/DataSet.h>
 #include <terralib/memory/DataSetItem.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

 std::map<DataSetId, std::string> terrama2::core::DataAccessorJsonCemaden::getFiles(const Filter& filter, std::shared_ptr<FileRemover> remover) const
 {
   auto& retrieverFactory = DataRetrieverFactory::getInstance();
   DataRetrieverPtr dataRetriever = retrieverFactory.make(dataProvider_);

   // Cemaden has only one service for all DCPs
   // all information is replicated in all DCPs
   auto dataset = dataSeries_->datasetList.front();
   auto uri = retrieveData(dataRetriever, dataset, filter, remover);

   // map all dataset to the same uri
   std::map<DataSetId, std::string> uriMap;
   for(const auto& dataset : dataSeries_->datasetList)
   {
     uriMap.emplace(dataset->id, uri);
   }

   return uriMap;
 }

 void terrama2::core::DataAccessorJsonCemaden::getSeriesCallback( const Filter& filter,
                                                                  std::shared_ptr<FileRemover> remover,
                                                                  std::function<void(const DataSetId&, const std::string& /*uri*/)> processFile) const
 {
   // Cemaden has a webservice for all files
   // only one download is needed
   auto datasetMap = getFiles(filter, remover);
   for (const auto& it : datasetMap)
   {
     processFile(it.first, it.second);
   }
 }

std::string terrama2::core::DataAccessorJsonCemaden::getDCPCode(DataSetPtr dataset) const
{
  auto property = getCodePropertyName(dataset);
  return getProperty(dataset, dataSeries_, property);
}

std::string terrama2::core::DataAccessorJsonCemaden::getCodePropertyName(DataSetPtr dataset) const
{
  return getProperty(dataset, dataSeries_, "dcp_code_property");
}

std::string terrama2::core::DataAccessorJsonCemaden::getStaticDataProperties(DataSetPtr dataset) const
{
  return getProperty(dataset, dataSeries_, "static_properties");
}

std::string terrama2::core::DataAccessorJsonCemaden::getDataMask(DataSetPtr dataSet) const
{
  auto mask = getProperty(dataSet, dataSeries_, "dcp_data_mask");
  
  std::string fileMask;
  auto pos = mask.find_last_of("\\/");
  if(pos != std::string::npos)
  {
    fileMask = mask.substr(pos+1);
  }
  else
  {
    fileMask = mask;
  }

  boost::replace_all(fileMask, "%UF", getUf(dataSet));
  boost::replace_all(fileMask, "%ID", getStationTypeId(dataSet));
  return fileMask;
}

std::string terrama2::core::DataAccessorJsonCemaden::getUf(DataSetPtr dataset) const
{
  auto property = getProperty(dataset, dataSeries_, "uf_property");
  return getProperty(dataset, dataSeries_, property);
}

std::string terrama2::core::DataAccessorJsonCemaden::getStationTypeId(DataSetPtr dataset) const
{
  auto property = getProperty(dataset, dataSeries_, "station_type_id_property");
  return getProperty(dataset, dataSeries_, property);
}

terrama2::core::DataSetSeries terrama2::core::DataAccessorJsonCemaden::getSeries( const std::string& uri,
                                                                                  const Filter& filter,
                                                                                  DataSetPtr dataSet,
                                                                                  std::shared_ptr<terrama2::core::FileRemover> /*remover*/) const
{
  QString codestacao = QString::fromStdString(getCodePropertyName(dataSet));
  QString inputTimestampProperty = QString::fromStdString(getInputTimestampPropertyName(dataSet));
  std::string timestampProperty = getTimestampPropertyName(dataSet);
  QStringList staticData = QString::fromStdString(getStaticDataProperties(dataSet)).split(',');

  QString code = QString::fromStdString(getDCPCode(dataSet));

  auto dataSetType =  std::make_shared<te::da::DataSetType>(code.toStdString());
  dataSetType->add(new te::dt::DateTimeProperty(timestampProperty, te::dt::TIME_INSTANT_TZ, true));

  auto teDataSet = std::make_shared<te::mem::DataSet>(dataSetType.get());

  te::core::URI teUri(uri);
  std::string filePath = getFolderMask(dataSet) + "/" + getDataMask(dataSet);

  auto jsonStr = readFileContents(teUri.path()+filePath);
  QJsonDocument doc = QJsonDocument::fromJson(jsonStr.c_str());
  auto readingsArray = doc.object()["cemaden"].toArray();
  // iterate over readings
  // and populate the dataset
  for(auto reading : readingsArray)
  {
    auto obj = reading.toObject();
    //check if this is the DCP we want
    if(obj[codestacao].toString() != code)
      continue;

    auto timestampStr = obj[inputTimestampProperty].toString().toStdString();
    auto timestamp = terrama2::core::TimeUtils::stringToTimestamp(timestampStr, "%Y-%m-%d %H:%M:%S%F");
    // filter by timestamp
    if((filter.discardBefore && (*filter.discardBefore > *timestamp))
        || (filter.discardAfter && (*filter.discardAfter < *timestamp)))
        continue;

    // create new item
    auto item = std::unique_ptr<te::mem::DataSetItem>(new te::mem::DataSetItem(teDataSet.get()));
    // add item to the end of the dataset
    teDataSet->moveLast();
    teDataSet->add(item.get());
    // check if the item has data
    bool hasData = false;

    // add timestamp to dataset
    item->setDateTime(timestampProperty, static_cast<te::dt::TimeInstantTZ*>(timestamp->clone()));

    for(auto val = obj.begin(); val != obj.end(); ++val) {
      auto key = val.key();
      // ignore static data
      if(staticData.contains(key)
          || key == codestacao
          || key.toStdString() == timestampProperty)
        continue;

      // property name
      auto keyStr = key.toStdString();

      // check if the property is already registered in the dataset
      auto properties = dataSetType->getProperties();
      auto it = std::find_if(properties.cbegin(), properties.cend(), [&keyStr](te::dt::Property* property){ return property->getName() == keyStr; });
      if(it == properties.cend())
      {
        // update dataSetType
        te::dt::SimpleProperty* property = new te::dt::SimpleProperty(keyStr, te::dt::DOUBLE_TYPE, false);
        dataSetType->add(property);
        // update memory dataset structure
        teDataSet->add(keyStr, te::dt::DOUBLE_TYPE);
      }

      // add property value
      item->setDouble(keyStr, val.value().toDouble());
      hasData = true;
    }
    if(!hasData)
    {
      // remove the item if no data was inserted
      teDataSet->remove(item.get());
    }
    item.release();
  }

  terrama2::core::DataSetSeries serie;
  serie.dataSet = dataSet;
  serie.syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(teDataSet);
  serie.teDataSetType = dataSetType;

  return serie;
}

std::string terrama2::core::DataAccessorJsonCemaden::getFolderMask(DataSetPtr dataSet) const
{
  auto mask = getProperty(dataSet, dataSeries_, "dcp_data_mask");

  std::string folderMask;
  auto pos = mask.find_last_of("\\/");
  if(pos != std::string::npos)
  {
    for(size_t i = 0; i < pos; ++i)
      folderMask +=mask.at(i);
  }

  return folderMask;
}

std::string terrama2::core::DataAccessorJsonCemaden::retrieveData(const DataRetrieverPtr dataRetriever,
                                                                  DataSetPtr dataSet,
                                                                  const Filter& filter,
                                                                  std::shared_ptr<FileRemover> remover) const
{
  std::string mask = getDataMask(dataSet);
  std::string folderPath = getFolderMask(dataSet);

  std::string timezone;
  try
  {
    timezone = getTimeZone(dataSet);
  }
  catch(const UndefinedTagException& /*e*/)
  {
    timezone = "UTC+00";
  }

  return dataRetriever->retrieveData(mask, filter, timezone, remover, "", folderPath);
}

void terrama2::core::DataAccessorJsonCemaden::retrieveDataCallback(const DataRetrieverPtr /*dataRetriever*/,
                                                                    DataSetPtr /*dataSet*/,
                                                                    const Filter& /*filter*/,
                                                                    std::shared_ptr<FileRemover> /*remover*/,
                                                                    std::function<void(const std::string &, const std::string &)> /*processFile*/) const
{
  QString errMsg = QObject::tr("Invalid method call.");
  throw NotRetrivableException() << ErrorDescription(errMsg);
}

std::string terrama2::core::DataAccessorJsonCemaden::dataSourceType() const
{
  QString errMsg = QObject::tr("Invalid method call.");
  throw Exception() << ErrorDescription(errMsg);
}
