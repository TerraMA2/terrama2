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
  \file terrama2/impl/DataAccessorCSV.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "DataAccessorCSV.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/TimeUtils.hpp"

// TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/geometry/GeometryProperty.h>

//QT
#include <QUrl>
#include <QDir>
#include <QSet>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Boost
#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>

// STL
#include <fstream>


std::shared_ptr<te::dt::TimeInstantTZ> terrama2::core::DataAccessorCSV::readFile(DataSetSeries& series, std::shared_ptr<te::mem::DataSet>& completeDataset, std::shared_ptr<te::da::DataSetTypeConverter>& converter, QFileInfo fileInfo, const std::string& mask, terrama2::core::DataSetPtr dataSet) const
{
  checkFields(dataSet);

  QTemporaryFile tempFile(fileInfo.baseName());

  if(!tempFile.open())
  {
    QString errMsg = QObject::tr("Could not open temporary file!");
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  QFileInfo filteredFileInfo = filterTxt(fileInfo, tempFile, dataSet);

  return DataAccessorFile::readFile(series, completeDataset, converter, filteredFileInfo, mask, dataSet);
}

QFileInfo terrama2::core::DataAccessorCSV::filterTxt(QFileInfo& fileInfo, QTemporaryFile& tempFile, terrama2::core::DataSetPtr dataSet) const
{
  int header = 0 ;
  int columnsLine = 0;

  if(!dataSet->format.at("header_size").empty())
    header = std::stoi(dataSet->format.at("header_size"));

  if(!dataSet->format.at("columns_line").empty())
    columnsLine = std::stoi(dataSet->format.at("columns_line"));

  if((header == 1 && columnsLine == 1)||
     (header == 0 && columnsLine == 0))
  {
    return fileInfo;
  }

  std::ifstream file(fileInfo.absoluteFilePath().toStdString());

  if(!file.is_open())
  {
    QString errMsg = QObject::tr("Could not open file!");
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  std::ofstream outputFile(tempFile.fileName().toStdString());

  if(!outputFile.is_open())
  {
    QString errMsg = QObject::tr("Could not open temporary file!");
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  std::string line = "";
  int lineNumber = 1;

  while(std::getline(file, line))
  {
    if(lineNumber <= header && lineNumber != columnsLine)
    {
      lineNumber++;
      continue;
    }

    outputFile << line << "\n";

    if(!outputFile)
    {
      QString errMsg = QObject::tr("Could not write to temporary file!");
      TERRAMA2_LOG_WARNING() << errMsg;
      throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
    }

    lineNumber++;
  }

  outputFile.close();

  return QFileInfo(tempFile.fileName());
}


te::dt::AbstractData* terrama2::core::DataAccessorCSV::stringToTimestamp(te::da::DataSet* dataset,
                                                                             const std::vector<std::size_t>& indexes,
                                                                             int /*dstType*/,
                                                                             const std::string& timezone,
                                                                             std::string& dateTimeFormat) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getAsString(indexes[0]);
    boost::posix_time::ptime boostDate;

    std::string boostFormat = TimeUtils::terramaDateMask2BoostFormat(dateTimeFormat);

    //mask to convert DateTime string to Boost::ptime
    std::locale format(std::locale(), new boost::posix_time::time_input_facet(boostFormat));

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

bool terrama2::core::DataAccessorCSV::getConvertAll(DataSetPtr dataSet) const
{
  std::string convert = getProperty(dataSet, dataSeries_, "convert_all");

  boost::trim(convert);

  std::transform(convert.begin(), convert.end(), convert.begin(), ::tolower);

  return (convert == "true" ? true : false);
}

QJsonArray terrama2::core::DataAccessorCSV::getFields(DataSetPtr dataSet) const
{
  const QJsonDocument& doc = QJsonDocument::fromJson(getProperty(dataSet, dataSeries_, "fields").c_str());

  const QJsonObject& obj = doc.object();

  if(!obj.contains("fields"))
  {
    QString errMsg = QObject::tr("Invalid JSON document!");
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  return obj.value("fields").toArray();
}


te::dt::AbstractData* terrama2::core::DataAccessorCSV::stringToPoint(te::da::DataSet* dataset,
                                                                         const std::vector<std::size_t>& indexes,
                                                                         int dstType,
                                                                         const Srid& srid) const
{
  assert(dataset);
  assert(indexes.size() == 2);

  te::dt::AbstractData* point = te::da::XYToPointConverter(dataset, indexes, dstType);
  static_cast<te::gm::Point*>(point)->setSRID(srid);

  return point;
}


QJsonObject terrama2::core::DataAccessorCSV::getFieldObj(const QJsonArray& array,
                                                             const std::string& fieldName) const
{
  for(const auto& item : array)
  {
    const QJsonObject& obj = item.toObject();

    std::string type = obj.value("type").toString().toStdString();

    if(dataTypes.at(type) == te::dt::GEOMETRY_TYPE)
    {
      std::string latitude = obj.value("latitude").toString().toStdString();
      std::string longitude = obj.value("longitude").toString().toStdString();

      if(latitude == fieldName || longitude == fieldName)
      {
        return item.toObject();
      }

      continue;
    }

    std::string column = obj.value("column").toString().toStdString();

    if(column == fieldName)
    {
      return item.toObject();
    }
  }

  return QJsonObject();
}

void terrama2::core::DataAccessorCSV::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  bool convertAll = getConvertAll(dataSet);

  QJsonObject fieldGeomObj;

  QJsonArray fieldsArray = getFields(dataSet);

  checkOriginFields(converter, fieldsArray);

  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);

    QJsonObject fieldObj = getFieldObj(fieldsArray, property->getName());

    if(fieldObj.empty())
    {
      if(convertAll)
      {
        std::string alias = simplifyString(property->getName());

        if(std::isdigit(alias.at(0)))
          alias ="_" + alias;

        std::string defaultType = getProperty(dataSet, dataSeries_, "default_type");

        if(dataTypes.at(defaultType) != te::dt::STRING_TYPE)
        {
          te::dt::SimpleProperty* defaultProperty = new te::dt::SimpleProperty(alias, dataTypes.at(defaultType));

          converter->add(i,defaultProperty);
        }
        else
        {
          te::dt::Property* defaultProperty = property->clone();
          defaultProperty->setName(alias);

          converter->add(i,defaultProperty);
        }
      }

      converter->remove(property->getName());

      continue; // for(size_t i = 0, size = properties.size(); i < size; ++i)
    }

    int type = dataTypes.at(fieldObj.value("type").toString().toStdString());

    if(type == te::dt::GEOMETRY_TYPE)
    {
      fieldGeomObj = fieldObj;
      continue;
    }

    std::string alias = fieldObj.value("alias").toString().toStdString();

    if(alias.empty())
    {
      alias = simplifyString(property->getName());

      if(std::isdigit(alias.at(0)))
        alias ="_" + alias;
    }

    switch (type)
    {
      case te::dt::DOUBLE_TYPE:
      {
        te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(alias, type);

        converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
        break;
      }
      case te::dt::INT32_TYPE:
      {
        te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(alias, type);

        converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToInt, this, _1, _2, _3));
        break;
      }
      case te::dt::DATETIME_TYPE:
      {
        std::string format = TimeUtils::terramaDateMask2BoostFormat(fieldObj.value("format").toString().toStdString());

        te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(alias, te::dt::TIME_INSTANT_TZ);
        converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorCSV::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet), format));

        break;
      }
      default:
      {
        te::dt::Property* defaultProperty = property->clone();
        defaultProperty->setName(alias);

        converter->add(i,defaultProperty);
        break;
      }
    }

    converter->remove(property->getName());
  }

  // Create geometry column
  if(!fieldGeomObj.empty())
  {
    Srid srid = getSrid(dataSet);
    std::string alias = fieldGeomObj.value("alias").toString().toStdString();

    te::gm::GeometryProperty* geomProperty = new te::gm::GeometryProperty(alias, srid, te::gm::PointType);

    if(fieldGeomObj.value("column").isUndefined())
    {
      size_t longPos = std::numeric_limits<size_t>::max();
      size_t latPos = std::numeric_limits<size_t>::max();

      std::string longProperty = fieldGeomObj.value("longitude").toString().toStdString();
      std::string latProperty = fieldGeomObj.value("latitude").toString().toStdString();

      longPos = converter->getConvertee()->getPropertyPosition(longProperty);
      latPos = converter->getConvertee()->getPropertyPosition(latProperty);

      if(longPos == std::numeric_limits<size_t>::max() ||
         latPos == std::numeric_limits<size_t>::max())
      {
        QString errMsg = QObject::tr("Could not find the point complete information!");
        TERRAMA2_LOG_WARNING() << errMsg;
        throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
      }

      std::vector<size_t> latLonAttributes;
      latLonAttributes.push_back(longPos);
      latLonAttributes.push_back(latPos);

      converter->add(latLonAttributes, geomProperty, boost::bind(&terrama2::core::DataAccessorCSV::stringToPoint, this, _1, _2, _3, srid));

      converter->remove(longProperty);
      converter->remove(latProperty);
    }
    else
    {
      // TODO: WKT
    }
  }

  // Check if all fields were created
  for(const auto& item : fieldsArray)
  {
    auto field = item.toObject();

    try
    {
      checkProperty(converter->getResult(), field.value("alias").toString().toStdString());
    }
    catch(DataAccessorException& e)
    {
      QString errMsg = QObject::tr("Invalid generated file: %1").arg(*boost::get_error_info<terrama2::ErrorDescription>(e));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
    }
  }
}


void terrama2::core::DataAccessorCSV::checkOriginFields(std::shared_ptr<te::da::DataSetTypeConverter> converter,
                                                          const QJsonArray& fieldsArray) const
{
  for(const auto& item : fieldsArray)
  {
    auto field = item.toObject();

    if(field.value("type").toString() == "GEOMETRY_POINT")
    {
      checkProperty(converter->getConvertee(), field.value("latitude").toString().toStdString());
      checkProperty(converter->getConvertee(), field.value("longitude").toString().toStdString());
    }
    else
    {
      checkProperty(converter->getConvertee(), field.value("column").toString().toStdString());
    }
  }
}

void terrama2::core::DataAccessorCSV::checkProperty(te::da::DataSetType* dataSetType,
                                                       std::string property) const
{
  size_t pos = std::numeric_limits<size_t>::max();

  pos = dataSetType->getPropertyPosition(property);

  if(pos == std::numeric_limits<size_t>::max())
  {
    QString errMsg = QObject::tr("Could not find the '%1' property!").arg(QString::fromStdString(property));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }
}
