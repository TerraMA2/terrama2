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
  \file terrama2/core/data-access/DataAccessor.cpp

  \brief

  \author Jano Simas
  \author Evandro Delatin
 */

#include "DataAccessor.hpp"
#include "../Exception.hpp"
#include "../utility/Logger.hpp"
#include "../utility/DataRetrieverFactory.hpp"

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/SimpleData.h>
#include <terralib/datatype/Property.h>


//QT
#include <QUrl>
#include <QDir>
#include <QObject>

//STL
#include <algorithm>

terrama2::core::DataAccessor::DataAccessor(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, Filter filter)
  : dataProvider_(dataProvider),
    dataSeries_(dataSeries),
    filter_(filter)
{
  if(!dataProvider_.get() || ! dataSeries_.get())
  {
    QString errMsg = QObject::tr("Mandatory parameters not provided.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  if(dataProvider_->id != dataSeries_->dataProviderId)
  {
    QString errMsg = QObject::tr("Input DataProvider different from DataSeries' DataProvider.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  boost::local_time::local_date_time boostTime(boost::posix_time::not_a_date_time);
  lastDateTime_ = std::make_shared<te::dt::TimeInstantTZ>(boostTime);
}

te::dt::AbstractData* terrama2::core::DataAccessor::stringToDouble(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const
{
  assert(indexes.size() == 1);

  try
  {
    if(dataset->isNull(indexes[0]))
      return nullptr;

    std::string strValue = dataset->getAsString(indexes[0]);

    if(strValue.empty())
      return nullptr;

    double value = std::stod(strValue);

    if(!std::isnormal(value) && (value != 0.0))
      return nullptr;

    te::dt::SimpleData<double>* data = new te::dt::SimpleData<double>(value);

    return data;
  }
  catch(const std::invalid_argument& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }

  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error";
  }

  return nullptr;
}

te::dt::AbstractData* terrama2::core::DataAccessor::stringToInt(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const
{
  assert(indexes.size() == 1);

  try
  {
    if(dataset->isNull(indexes[0]))
      return nullptr;

    std::string strValue = dataset->getAsString(indexes[0]);

    if(strValue.empty())
      return nullptr;

    boost::int32_t value = std::stoi(strValue);

    if(!std::isnormal(value) && (value != 0))
      return nullptr;

    te::dt::SimpleData<boost::int32_t>* data = new te::dt::SimpleData<boost::int32_t>(value);

    return data;
  }

  catch(const std::invalid_argument& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }

  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error";
  }

  return nullptr;
}



std::shared_ptr<te::da::DataSetTypeConverter> terrama2::core::DataAccessor::getConverter(DataSetPtr dataset, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  std::shared_ptr<te::da::DataSetTypeConverter> converter(new te::da::DataSetTypeConverter(datasetType.get()));

  addColumns(converter, datasetType);

  adapt(dataset, converter);
  std::string id = "FID";
  const std::vector< te::dt::Property* >& propertyList = converter->getResult()->getProperties();
  auto it = std::find_if(propertyList.cbegin(), propertyList.cend(), [id](te::dt::Property *property)
  {
    return property->getName() == id;
  });
  if(it != propertyList.cend())
    converter->remove(id);

  return converter;
}

std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > terrama2::core::DataAccessor::getSeries(const Filter& filter) const
{

  //if data provider is not active, nothing to do
  if(!dataProvider_->active)
  {
    QString errMsg = QObject::tr("Disabled data provider (Should not arrive here!)");

    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw DataProviderException() << ErrorDescription(errMsg);
  }

  if(filter.discardAfter.get() && filter.discardBefore.get()
      && (*filter.discardAfter) < (*filter.discardBefore))
  {
    QString errMsg = QObject::tr("Empty filter time range.");

    TERRAMA2_LOG_WARNING() << errMsg.toStdString();
    throw DataProviderException() << ErrorDescription(errMsg);
  }

  std::map<DataSetPtr,DataSetSeries> series;

  try
  {
    DataRetrieverPtr dataRetriever = DataRetrieverFactory::getInstance().make(dataProvider_);
    for(const auto& dataset : dataSeries_->datasetList)
    {
      //if the dataset is not active, continue to next.
      if(!dataset->active)
        continue;

      if(!intersects(dataset, filter))
        continue;

      bool removeFolder = false;
      // if this data retriever is a remote server that allows to retrieve data to a file,
      // download the file to a tmeporary location
      // if not, just get the DataProvider uri
      std::string uri;
      if(dataRetriever->isRetrivable())
      {
        uri = retrieveData(dataRetriever, dataset, filter);
        removeFolder = true;
      }
      else
        uri = dataProvider_->uri;

      //TODO: Set last date collected in filter
      DataSetSeries tempSeries = getSeries(uri, filter, dataset);
      series.emplace(dataset, tempSeries);


      if(removeFolder)
      {
        QUrl url(uri.c_str());
        QDir dir(url.path());
        if(!dir.removeRecursively())
        {
          QString errMsg = QObject::tr("Data folder could not be removed.\n%1").arg(url.path());
          TERRAMA2_LOG_ERROR() << errMsg.toStdString();
        }
      }
    }//for each dataset
  }
  catch(const terrama2::Exception&)
  {

  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    //TODO: catch cannot open DataProvider, log here
  }

  return series;
}


void terrama2::core::DataAccessor::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  for(std::size_t i = 0, size = datasetType->size(); i < size; ++i)
  {
    te::dt::Property* p = datasetType->getProperty(i);

    converter->add(i,p->clone());
  }
}

Srid terrama2::core::DataAccessor::getSrid(DataSetPtr dataSet) const
{
  try
  {
    Srid srid = std::stoi(dataSet->format.at("srid"));
    return srid;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined srid in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessor::getProperty(DataSetPtr dataSet, std::string tag, bool logErrors) const
{
  std::string property;
  try
  {
    auto semantics = dataSeries_->semantics;
    property = semantics.metadata.at(tag);
  }
  catch(...)  //exceptions will be treated after
  {
  }

  if(property.empty())
  {
    try
    {
      property = dataSet->format.at(tag);
    }
    catch(...)  //exceptions will be treated after
    {
    }
  }

  if(property.empty())
  {
    QString errMsg = QObject::tr("Undefined %2 in dataset: %1.").arg(dataSet->id).arg(QString::fromStdString(tag));
    if(logErrors)
      TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }

  return property;
}

std::string terrama2::core::DataAccessor::getTimestampPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, "timestamp_property");
}

std::string terrama2::core::DataAccessor::getGeometryPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, "geometry_property");
}
