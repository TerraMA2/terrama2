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

#include <cassert>
#include <boost/date_time/local_time/local_date_time.hpp>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/Property.h>
#include <terralib/datatype/SimpleData.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/core/uri/Utils.h>
#include <QObject>
#include <QString>
//STL
#include <algorithm>
#include <cmath>
#include <exception>
#include <stdexcept>

#include "../data-model/DataProvider.hpp"
#include "../data-model/Filter.hpp"
#include "../../Exception.hpp"
#include "../Exception.hpp"
#include "../data-model/DataSet.hpp"
#include "../utility/DataRetrieverFactory.hpp"
#include "../utility/Logger.hpp"
#include "../utility/Utils.hpp"
#include "DataAccessor.hpp"
#include "DataRetriever.hpp"

namespace terrama2 {
namespace core {
class FileRemover;
}  // namespace core
}  // namespace terrama2
namespace te {
namespace dt {
class AbstractData;
}  // namespace dt
}  // namespace te

terrama2::core::DataAccessor::DataAccessor(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
  : dataProvider_(dataProvider),
    dataSeries_(dataSeries)
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

    std::string strValue = dataset->getString(indexes[0]);

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
    TERRAMA2_LOG_ERROR() << QObject::tr("Invalid argument: %1").arg(e.what());
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

te::dt::AbstractData* terrama2::core::DataAccessor::stringToInt(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const
{
  assert(indexes.size() == 1);

  try
  {
    if(dataset->isNull(indexes[0]))
      return nullptr;

    std::string strValue = dataset->getString(indexes[0]);

    if(strValue.empty())
      return nullptr;

    try
    {
      boost::int32_t value = std::stoi(strValue);

      te::dt::SimpleData<boost::int32_t>* data = new te::dt::SimpleData<boost::int32_t>(value);
      return data;
    }
    catch (const std::invalid_argument&)
    {
      TERRAMA2_LOG_WARNING() << "Unable to convert value to integer: " << strValue;
      return nullptr;
    }
    catch (const std::out_of_range&)
    {
      TERRAMA2_LOG_ERROR() << "Value cannot be converted to integer type: " << strValue;
      return nullptr;
    }
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

std::shared_ptr<te::da::DataSetTypeConverter> terrama2::core::DataAccessor::getConverter(DataSetPtr dataset, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  if(!datasetType.get())
  {
    QString errMsg = QObject::tr("Invalid DataSetType.");

    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

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

std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries >
terrama2::core::DataAccessor::getSeries(const std::map<DataSetId, std::string> uriMap, const Filter& filter, std::shared_ptr<FileRemover> remover) const
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
    // inconsistent discard dates is checked in filter construction
    // here it means that there is no data available
    QString errMsg = QObject::tr("No data available.");

    TERRAMA2_LOG_WARNING() << errMsg.toStdString();
    throw terrama2::core::DataProviderException() << ErrorDescription(errMsg);
  }

  std::unordered_map<DataSetPtr,DataSetSeries> series;

  try
  {
    for(const auto& dataset : dataSeries_->datasetList)
    {
      //if the dataset is not active, continue to next.
      if(!dataset->active)
        continue;

      if(!intersects(dataset, filter))
        continue;

      try
      {
        te::core::URI decodedURI(te::core::URIDecode(uriMap.at(dataset->id)));
        DataSetSeries tempSeries = getSeries(uriMap.at(dataset->id), filter, dataset, remover);
        series.emplace(dataset, tempSeries);
      }
      catch(const std::out_of_range&)
      {
        // no data in the dataset
      }
      catch (const terrama2::core::NoDataException&)
      {
        // no data in the dataset
      }
    }//for each dataset

    if(series.empty())
    {
      QString errMsg = QObject::tr("No data in dataseries: %1.").arg(QString::fromStdString(dataSeries_->name));
      TERRAMA2_LOG_WARNING() << errMsg;
      throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
    }
  }
  catch(const terrama2::Exception&)
  {
    throw;//re-throw
  }
  catch(const boost::exception& e)
  {
    throw DataAccessorException() << ErrorDescription(boost::diagnostic_information(e).c_str());
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    throw DataAccessorException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Unknown exception occurred");
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  return series;
}

std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries >
terrama2::core::DataAccessor::getSeries(const Filter& filter, std::shared_ptr<FileRemover> remover) const
{
  auto uriMap = getFiles(filter, remover);
  return getSeries(uriMap, filter, remover);
}

std::map<DataSetId, std::string> terrama2::core::DataAccessor::getFiles(const Filter& filter, std::shared_ptr<FileRemover> remover) const
{
  auto& retrieverFactory = DataRetrieverFactory::getInstance();
  DataRetrieverPtr dataRetriever = retrieverFactory.make(dataProvider_);

  std::map<DataSetId, std::string> uriMap;
  for(const auto& dataset : dataSeries_->datasetList)
  {
    if(!dataset->active)
      continue;

    // if this data retriever is a remote server that allows to retrieve data to a file,
    // download the file to a temporary location
    // if not, just get the DataProvider uri
    std::string uri;
    if(dataRetriever->isRetrivable())
    {
      uri = retrieveData(dataRetriever, dataset, filter, remover);
    }
    else
    {
      uri = dataProvider_->uri;
    }

    uriMap.emplace(dataset->id, uri);
  }

  return uriMap;
}

void terrama2::core::DataAccessor::getSeriesCallback(const terrama2::core::Filter& filter,
                                                     std::shared_ptr<terrama2::core::FileRemover> remover,
                                                     std::function<void(const DataSetId&, const std::string& /*uri*/)> processFile) const
{
  auto& retrieverFactory = DataRetrieverFactory::getInstance();
  DataRetrieverPtr dataRetriever = retrieverFactory.make(dataProvider_);

  for(const auto& dataset : dataSeries_->datasetList)
  {
    if(!dataset->active)
      continue;

    // if this data retriever is a remote server that allows to retrieve data to a file,
    // download the file to a temporary location
    // if not, just get the DataProvider uri
    if(dataRetriever->isRetrivable())
    {
      auto dataSetId = dataset->id;
      retrieveDataCallback(dataRetriever, dataset, filter, remover, [&dataSetId, processFile](const std::string& uri, const std::string& folderMatched){processFile(dataSetId, uri); });
    }
    else
    {
      processFile(dataset->id, dataProvider_->uri);
    }
  }
}

void terrama2::core::DataAccessor::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  for(std::size_t i = 0, size = datasetType->size(); i < size; ++i)
  {
    te::dt::Property* p = datasetType->getProperty(i);

    converter->add(i,p->clone());
  }
}

Srid terrama2::core::DataAccessor::getSrid(DataSetPtr dataSet, bool logErrors) const
{
  return static_cast<Srid>(std::stoi(getProperty(dataSet, dataSeries_, "srid", logErrors)));
}

std::string terrama2::core::DataAccessor::getTimestampPropertyName(DataSetPtr dataSet, const bool logErrors) const
{
  return getProperty(dataSet, dataSeries_, "timestamp_property", logErrors);
}

std::string terrama2::core::DataAccessor::getInputTimestampPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "input_timestamp_property");
}

std::string terrama2::core::DataAccessor::getGeometryPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "geometry_property");
}

std::string terrama2::core::DataAccessor::getInputGeometryPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "input_geometry_property");
}

std::string terrama2::core::DataAccessor::getTimeZone(DataSetPtr dataSet, bool logErrors) const
{
  return getProperty(dataSet, dataSeries_, "timezone", logErrors);
}
