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


terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const Filter& filter)
 : DataAccessor(dataProvider, dataSeries, filter),
   DataAccessorDcp(dataProvider, dataSeries, filter),
   DataAccessorFile(dataProvider, dataSeries, filter)
{
  if(dataSeries->semantics.name != "PCD-TOA5")
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);;
  }

}

std::string terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5::timestampColumn() const
{
  return "TIMESTAMP";
}

std::string terrama2::core::DataAccessorDcpToa5::DataAccessorDcpToa5::getTimeZone(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("timezone");
  }
  catch (...)
  {
    QString errMsg = QObject::tr("Undefined timezone in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
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
    if(property->getName() == timestampColumn())
    {
      // datetime column found
      converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorDcpToa5::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataset)));
      break;
    }
  }
}

void terrama2::core::DataAccessorDcpToa5::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  // Don't add any columns here,
  // the converter will add columns
}

void terrama2::core::DataAccessorDcpToa5::getDataSet(const std::string& uri, const Filter& filter, DataSetPtr dataSet,
                                                                                  std::shared_ptr<te::mem::DataSet>& teDataSet,
                                                                                  std::shared_ptr<te::da::DataSetType>& teDataSetType) const
{
  std::string mask = getMask(dataSet);

  QTemporaryDir tempDir;
  if(!tempDir.isValid())
  {
    QString errMsg = QObject::tr("Can't create temporary folder.");
    TERRAMA2_LOG_ERROR() << errMsg;
  }

  QUrl url((uri+"/"+mask).c_str());
  QFileInfo originalInfo(url.path());
  QFile file(url.path());
  file.open(QIODevice::ReadWrite);

  QFile tempFile(tempDir.path()+"/"+originalInfo.fileName());
  tempFile.open(QIODevice::ReadWrite);
  file.readLine();//ignore first line
  tempFile.write(file.readLine()); //headers line

  //ignore third and fourth lines
  file.readLine();
  file.readLine();

  //read all file
  tempFile.write(file.readAll()); //headers line

  file.close();
  tempFile.close();

  // Overwrite file.
  file.open(QIODevice::WriteOnly);
  tempFile.open(QIODevice::ReadOnly);
  file.write(tempFile.readAll());

  file.close();
  tempFile.close();

  return terrama2::core::DataAccessorFile::getDataSet(uri, filter, dataSet, teDataSet, teDataSetType);
}
