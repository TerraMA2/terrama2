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
  \file terrama2/impl/DataAccessorTxtFile.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "DataAccessorTxtFile.hpp"
#include "../core/data-model/DataSetDcp.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/Logger.hpp"

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


std::shared_ptr<te::dt::TimeInstantTZ> terrama2::core::DataAccessorTxtFile::readFile(DataSetSeries& series, std::shared_ptr<te::mem::DataSet>& completeDataset, std::shared_ptr<te::da::DataSetTypeConverter>& converter, QFileInfo fileInfo, const std::string& mask, terrama2::core::DataSetPtr dataSet) const
{
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

QFileInfo terrama2::core::DataAccessorTxtFile::filterTxt(QFileInfo& fileInfo, QTemporaryFile& tempFile, terrama2::core::DataSetPtr dataSet) const
{
  if(dataSet->format.at("lines_skip").empty())
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

  std::vector<int> linesSkip;

  std::stringstream ss(dataSet->format.at("lines_skip"));

  std::string skipLineNumber;

  while(std::getline(ss, skipLineNumber, ','))
  {
    linesSkip.push_back(std::stoi(skipLineNumber));
  }

  std::string line = "";
  int lineNumber = 0;

  while(std::getline(file, line))
  {
    if(linesSkip.end() != std::find(linesSkip.begin(), linesSkip.end(), lineNumber))
    {
      lineNumber++;
      continue;
    }

    outputFile << line;
    outputFile.flush();

    if(outputFile.fail())
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


te::dt::AbstractData* terrama2::core::DataAccessorTxtFile::stringToTimestamp(te::da::DataSet* dataset,
                                                                             const std::vector<std::size_t>& indexes,
                                                                             int /*dstType*/,
                                                                             const std::string& timezone,
                                                                             std::string dateTimeFormat) const
{
  assert(indexes.size() == 1);

  try
  {
    std::string dateTime = dataset->getAsString(indexes[0]);
    boost::posix_time::ptime boostDate;

    //mask to convert DateTime string to Boost::ptime
    std::locale format(std::locale(), new boost::posix_time::time_input_facet(dateTimeFormat));

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

std::string terrama2::core::DataAccessorTxtFile::getLatitudePropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "latitude_property");
}


std::string terrama2::core::DataAccessorTxtFile::getLongitudePropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "longitude_property");
}


std::string terrama2::core::DataAccessorTxtFile::getTimestampPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "timestamp_format");
}


bool terrama2::core::DataAccessorTxtFile::getConvertAll(DataSetPtr dataSet) const
{
  return (getProperty(dataSet, dataSeries_, "convert_all") == "true" ? true : false);
}


te::dt::AbstractData* terrama2::core::DataAccessorTxtFile::stringToPoint(te::da::DataSet* dataset,
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

std::vector<std::tuple< std::string, std::string, int>>
terrama2::core::DataAccessorTxtFile::getFields(DataSetPtr dataSet) const
{
  std::vector<std::tuple< std::string, std::string, int>> fields;

  QJsonDocument doc = QJsonDocument::fromJson(getProperty(dataSet, dataSeries_, "fields").c_str());

  QJsonObject obj = doc.object();

  if(!obj.contains("fields"))
  {
    QString errMsg = QObject::tr("Invalid JSON document!");
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  QJsonArray array =obj.value("fields").toArray();

  for(const auto& item : array)
  {
    QJsonObject obj = item.toObject();

    std::string column = obj.value("column").toString().toStdString();
    std::string alias = obj.value("alias").toString().toStdString();
    std::string type = obj.value("type").toString().toStdString();

    fields.push_back(std::make_tuple(column, alias, dataTypes.at(type)));
  }

  return fields;
}


std::string terrama2::core::DataAccessorTxtFile::simplifyString(std::string text) const
{
  boost::trim(text);
  text.erase(std::remove_if(text.begin(), text.end(), [](char x){return !(std::isalnum(x) || x == ' ');}), text.end());
  std::replace(text.begin(), text.end(), ' ', '_');

  if(std::isdigit(text.at(0)))
    text ="_" + text;

  return text;
}
