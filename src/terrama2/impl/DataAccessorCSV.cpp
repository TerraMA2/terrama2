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

  if(!dataSet->format.at(JSON_HEADER_SIZE.toStdString()).empty())
    header = std::stoi(dataSet->format.at(JSON_HEADER_SIZE.toStdString()));

  if(!dataSet->format.at(JSON_PROPERTIES_NAMES_LINE.toStdString()).empty())
    columnsLine = std::stoi(dataSet->format.at(JSON_PROPERTIES_NAMES_LINE.toStdString()));

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

    if(boostDate == boost::posix_time::ptime())
    {
      QString errMsg = QObject::tr("Unable to parse string date");
      TERRAMA2_LOG_WARNING() << errMsg;
      return nullptr;
    }

    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(timezone));
    boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

    te::dt::TimeInstantTZ* dt = new te::dt::TimeInstantTZ(date);

    return dt;
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
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

bool terrama2::core::DataAccessorCSV::getConvertAll(DataSetPtr dataSet) const
{
  std::string convert = getProperty(dataSet, dataSeries_, JSON_CONVERT_ALL.toStdString());

  boost::trim(convert);

  std::transform(convert.begin(), convert.end(), convert.begin(), ::tolower);

  return (convert == "true" ? true : false);
}

QJsonArray terrama2::core::DataAccessorCSV::getFields(DataSetPtr dataSet) const
{
  const QJsonDocument& doc = QJsonDocument::fromJson(QString::fromStdString(getProperty(dataSet, dataSeries_, JSON_FIELDS.toStdString())).toUtf8());
  return doc.array();
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
                                                             const std::string& fieldName, const size_t position) const
{
  for(const auto& item : array)
    {
      const QJsonObject& obj = item.toObject();
      std::string type = obj.value(JSON_TYPE).toString().toStdString();

      if(dataTypes.at(type) == te::dt::GEOMETRY_TYPE)
      {
        std::string latitudePropertyName, longitudePropertyName;
        if(obj.contains(JSON_LATITUDE_PROPERTY_NAME))
          latitudePropertyName = obj.value(JSON_LATITUDE_PROPERTY_NAME).toString().toStdString();
        if(obj.contains(JSON_LONGITUDE_PROPERTY_NAME))
          longitudePropertyName = obj.value(JSON_LONGITUDE_PROPERTY_NAME).toString().toStdString();

        size_t latitudePropertyPosition = std::numeric_limits<size_t>::max(), longitudePropertyPosition = std::numeric_limits<size_t>::max();
        if(obj.contains(JSON_LATITUDE_PROPERTY_POSITION))
          latitudePropertyPosition = obj.value(JSON_LATITUDE_PROPERTY_POSITION).toInt();
        if(obj.contains(JSON_LONGITUDE_PROPERTY_POSITION))
          longitudePropertyPosition = obj.value(JSON_LONGITUDE_PROPERTY_POSITION).toInt();

        if(latitudePropertyName == fieldName || longitudePropertyName == fieldName
            || latitudePropertyPosition == position || longitudePropertyPosition == position)
          return obj;
      }
      else
      {
        std::string propertyName;
        size_t propertyPosition = std::numeric_limits<size_t>::max();
        if(obj.contains(JSON_PROPERTY_NAME))
          propertyName = obj.value(JSON_PROPERTY_NAME).toString().toStdString();
        if(obj.contains(JSON_PROPERTY_POSITION))
        {
          int temp = obj.value(JSON_PROPERTY_POSITION).toInt();
          if(temp != -1)
            propertyPosition = static_cast<size_t>(temp);
        }

        if(propertyName == fieldName || propertyPosition == position)
        {
          return obj;
        }
      }
    }

    return QJsonObject();
}

void terrama2::core::DataAccessorCSV::addPropertyAsDefaultType(te::dt::Property* property, std::shared_ptr<te::da::DataSetTypeConverter> converter, size_t i, DataSetPtr dataSet) const
{
  std::string alias = createValidPropertyName(property->getName());

  std::string defaultType = getProperty(dataSet, dataSeries_, JSON_DEFAULT_TYPE.toStdString());

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

void terrama2::core::DataAccessorCSV::addPropertyByType(std::shared_ptr<te::da::DataSetTypeConverter> converter, int type, DataSetPtr dataSet, te::dt::Property* property, QJsonObject fieldObj) const
{
  size_t pos = converter->getConvertee()->getPropertyPosition(property->getName());

  std::string alias = fieldObj.value(JSON_ALIAS).toString().toStdString();
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

      converter->add(pos, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
      break;
    }
    case te::dt::INT32_TYPE:
    {
      te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(alias, type);

      converter->add(pos, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToInt, this, _1, _2, _3));
      break;
    }
    case te::dt::DATETIME_TYPE:
    {
      std::string format = TimeUtils::terramaDateMask2BoostFormat(fieldObj.value(JSON_FORMAT).toString().toStdString());

      te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(alias, te::dt::TIME_INSTANT_TZ);
      converter->add(pos, dtProperty, boost::bind(&terrama2::core::DataAccessorCSV::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet), format));

      break;
    }
    default:
    {
      te::dt::Property* defaultProperty = property->clone();
      defaultProperty->setName(alias);

      converter->add(pos,defaultProperty);
      break;
    }
  }
}

void terrama2::core::DataAccessorCSV::addGeomProperty(QJsonObject fieldGeomObj, std::shared_ptr<te::da::DataSetTypeConverter> converter, DataSetPtr dataSet) const
{
  Srid srid = getSrid(dataSet);
  std::string alias = fieldGeomObj.value(JSON_ALIAS).toString().toStdString();

  auto geomProperty = std::unique_ptr<te::gm::GeometryProperty>(new te::gm::GeometryProperty(alias, srid, te::gm::PointType));

  // If there is no JSON_PROPERTY_NAME or JSON_PROPERTY_POSITION
  // then it's a point and need two columns, latitude and longitude
  if(fieldGeomObj.value(JSON_PROPERTY_NAME).isUndefined()
      && fieldGeomObj.value(JSON_PROPERTY_POSITION).isUndefined())
  {
    size_t longPos = std::numeric_limits<size_t>::max();
    size_t latPos = std::numeric_limits<size_t>::max();

    if(fieldGeomObj.contains(JSON_LONGITUDE_PROPERTY_NAME)
        && fieldGeomObj.contains(JSON_LATITUDE_PROPERTY_NAME))
    {
      //columns defined by name
      std::string longProperty = fieldGeomObj.value(JSON_LONGITUDE_PROPERTY_NAME).toString().toStdString();
      std::string latProperty = fieldGeomObj.value(JSON_LATITUDE_PROPERTY_NAME).toString().toStdString();

      longPos = converter->getConvertee()->getPropertyPosition(longProperty);
      latPos = converter->getConvertee()->getPropertyPosition(latProperty);
    }
    else if(fieldGeomObj.contains(JSON_LONGITUDE_PROPERTY_POSITION)
              && fieldGeomObj.contains(JSON_LATITUDE_PROPERTY_POSITION))
    {
      //columns from input dataset defined by position
      longPos = static_cast<size_t>(fieldGeomObj.value(JSON_LONGITUDE_PROPERTY_POSITION).toInt());
      latPos = static_cast<size_t>(fieldGeomObj.value(JSON_LATITUDE_PROPERTY_POSITION).toInt());
    }
    else
    {
      //columns not defined
      QString errMsg = QObject::tr("Could not find the point complete information!");
      TERRAMA2_LOG_WARNING() << errMsg;
      throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
    }

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

    converter->add(latLonAttributes, geomProperty.release(), boost::bind(&terrama2::core::DataAccessorCSV::stringToPoint, this, _1, _2, _3, srid));

    //Get property from input dataset
    auto oldLongProp = converter->getConvertee()->getProperty(longPos);
    auto oldLatProp = converter->getConvertee()->getProperty(latPos);

    // get columns from converted dataset
    auto longRemovePos = converter->getResult()->getPropertyPosition(oldLongProp->getName());
    auto latRemovePos = converter->getResult()->getPropertyPosition(oldLatProp->getName());

    //remove column from converted dataset
    converter->remove(longRemovePos);
    converter->remove(latRemovePos);
  }
  else
  {
    // TODO: WKT
    QString errMsg = QObject::tr("CSV with WKT property no implemented");
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }
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

    QJsonObject fieldObj = getFieldObj(fieldsArray, property->getName(), i);

    if(fieldObj.empty())
    {
      if(convertAll)
      {
        addPropertyAsDefaultType(property, converter, i, dataSet);
      }
    }
    else
    {
      int type = dataTypes.at(fieldObj.value(JSON_TYPE).toString().toStdString());

      if(type == te::dt::GEOMETRY_TYPE)
      {
        fieldGeomObj = fieldObj;
        continue;
      }

      addPropertyByType(converter, type, dataSet, property, fieldObj);
    }

    converter->remove(property->getName());
  }

  // Create geometry column
  if(!fieldGeomObj.empty())
  {
    //TODO: review add geometry outside loop.
    // Removing lat and long coluns changes the order of the properties expected in the properties loop
    addGeomProperty(fieldGeomObj, converter, dataSet);
  }

  // Check if all fields were created
  for(const auto& item : fieldsArray)
  {
    auto field = item.toObject();

    try
    {
      checkProperty(converter->getResult(), field.value(JSON_ALIAS).toString().toStdString());
    }
    catch(DataAccessorException& e)
    {
      QString errMsg = QObject::tr("Invalid generated file: %1").arg(*boost::get_error_info<terrama2::ErrorDescription>(e));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
    }
  }
}


bool terrama2::core::DataAccessorCSV::checkOriginFields(std::shared_ptr<te::da::DataSetTypeConverter> converter,
                                                          const QJsonArray& fieldsArray) const
{
  for(const auto& item : fieldsArray)
  {
    auto field = item.toObject();

    if(field.value(JSON_TYPE).toString() == "GEOMETRY_POINT")
    {
      if(field.contains(JSON_LONGITUDE_PROPERTY_NAME)
          && field.contains(JSON_LATITUDE_PROPERTY_NAME))
      {
        if(!(checkProperty(converter->getConvertee(), field.value(JSON_LATITUDE_PROPERTY_NAME).toString().toStdString())
                && checkProperty(converter->getConvertee(), field.value(JSON_LONGITUDE_PROPERTY_NAME).toString().toStdString())))
          return false;
      }
    }
    else
    {
      if(field.contains(JSON_PROPERTY_NAME))
      {
        if(!checkProperty(converter->getConvertee(), field.value(JSON_PROPERTY_NAME).toString().toStdString()))
          return false;
      }
    }
  }

  return true;
}

bool terrama2::core::DataAccessorCSV::checkProperty(te::da::DataSetType* dataSetType,
                                                       std::string property) const
{
  size_t pos = std::numeric_limits<size_t>::max();

  pos = dataSetType->getPropertyPosition(property);

  if(pos == std::numeric_limits<size_t>::max())
    return false;
  else
    return true;
}
