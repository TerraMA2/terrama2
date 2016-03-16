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
#include "../core/utility/FilterMask.hpp"

//terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/SimpleData.h>

//Qt
#include <QObject>
#include <QString>
#include <QDir>
#include <QUrl>
#include <QFileInfoList>

std::string terrama2::core::DataAccessorDcpInpe::retrieveData(const DataRetrieverPtr dataRetriever, const DataSetDcp& dataset, const Filter& filter) const
{
  std::string mask = getMask(dataset);
  return dataRetriever->retrieveData(mask, filter);
}

std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::getMask(const DataSetDcp& dataset) const
{
  try
  {
    return dataset.format.at("mask");
  }
  catch (...)
  {
    //TODO: log this
    //TODO: throw UndefinedTag
    throw;
  }
}

std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::getTimeZone(const DataSetDcp& dataset) const
{
  try
  {
    return dataset.format.at("timezone");
  }
  catch (...)
  {
    //TODO: log this
    //TODO: throw UndefinedTag
    throw;
  }
}

std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::dataSourceTye() const
{
  return "OGR";
}
std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::typePrefix() const
{
  return "CSV:";
}

std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorDcpInpe::getDataSet(const std::string& uri, const Filter& filter, const DataSetDcp& datasetDcp) const
{
  QUrl url(uri.c_str());
  QDir dir(url.path());
  QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);

  bool first = true;
  std::shared_ptr<te::mem::DataSet> completeDataset(nullptr);
  std::shared_ptr<te::da::DataSetTypeConverter> converter(nullptr);
  for(const auto& fileInfo : fileInfoList)
  {
    std::string name = fileInfo.fileName().toStdString();
    std::string baseName = fileInfo.baseName().toStdString();
    // Verify if the file name matches the mask
    if(!isValidDataSetName(getMask(datasetDcp), filter, name))
      continue;

    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceTye()));
    std::map<std::string, std::string> connInfo;

    connInfo["URI"] = typePrefix() + dir.absolutePath().toStdString() + "/" + name;
    datasource->setConnectionInfo(connInfo);

    //RAII for open/closing the datasource
    OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    if(!datasource->isOpened())
    {
      QString errMsg = QObject::tr("DataProvider could not be opened.");
      TERRAMA2_LOG_ERROR() << errMsg;

      continue;
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

    if(first)
    {
    //read and adapt all te:da::DataSet from terrama2::core::DataSet
      converter = getConverter(datasetDcp, std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(baseName)));
      std::shared_ptr<te::da::DataSetType> datasetType(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
      assert(datasetType);
      completeDataset = std::make_shared<te::mem::DataSet>(datasetType.get());
      first = false;
    }

    assert(converter);
    std::unique_ptr<te::da::DataSet> datasetOrig(transactor->getDataSet(baseName));
    assert(datasetOrig);
    std::shared_ptr<te::da::DataSet> dataset(te::da::CreateAdapter(datasetOrig.release(), converter.get(), true));

    //TODO:.. filter and join te::da::dataset from each dataset
    //TODO: join dataset
    completeDataset->copy(*dataset);
  }

  return completeDataset;
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
    std::locale format(std::locale::classic(), new boost::posix_time::time_input_facet("%m/%d/%Y %H:%M:%S"));

    std::istringstream stream(dateTime);//create stream
    stream.imbue(format);//set format
    stream >> boostDate;//convert to boost::ptime

    assert(boostDate != boost::posix_time::ptime());

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

te::dt::AbstractData* terrama2::core::DataAccessorDcpInpe::stringToDouble(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string strValue = dataset->getAsString(indexes[0]);

    if(strValue.empty())
    {
      return nullptr;
    }
    else
    {
      double value = 0;
      std::istringstream stream(strValue);//create stream
      stream >> value;

      te::dt::SimpleData<double>* data = new te::dt::SimpleData<double>(value);

      return data;
    }
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

void terrama2::core::DataAccessorDcpInpe::adapt(const DataSetDcp& datasetDcp, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  //only one timestamp column
  std::string timestampName = "N/A";
  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty("DateTime", te::dt::TIME_INSTANT_TZ);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == timestampName)
    {
      // datetime column found
      converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorDcpInpe::stringToTimestamp, this, _1, _2, _3, getTimeZone(datasetDcp)));
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
      converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessorDcpInpe::stringToDouble, this, _1, _2, _3));
    }
  }

}

void terrama2::core::DataAccessorDcpInpe::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  // Don't add any columns here,
  // the converter will add columns
}
