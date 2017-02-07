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

// Boost
#include <boost/bind.hpp>

// STL
#include <fstream>

/*
std::shared_ptr<te::dt::TimeInstantTZ> terrama2::core::DataAccessorTxtFile::readFile(DataSetSeries& series, std::shared_ptr<te::mem::DataSet>& completeDataset, std::shared_ptr<te::da::DataSetTypeConverter>& converter, QFileInfo fileInfo, const std::string& mask, terrama2::core::DataSetPtr dataSet) const
{
  QTemporaryFile tempFile;

  if(!tempFile.open())
  {
    // TODO: throw
  }

  QFileInfo filteredFileInfo = filterTxt(fileInfo, tempFile, dataSet);

  return DataAccessorFile::readFile(series, completeDataset, converter, filteredFileInfo, mask, dataSet);
}

QFileInfo terrama2::core::DataAccessorTxtFile::filterTxt(QFileInfo& fileInfo, QTemporaryFile& tempFile, terrama2::core::DataSetPtr dataSet) const
{
  std::ifstream file(fileInfo.absoluteFilePath().toStdString());

  if(!file.is_open())
  {
    // TODO: throw
  }

  std::ofstream outputFile(tempFile.fileName().toStdString());

  if(!outputFile.is_open())
  {
    // TODO: throw
  }

  std::vector<int> linesSkip;

  std::stringstream ss(dataSet->format.at("lines_skip"));

  std::string skipLineNumber;

  while(std::getline(ss, skipLineNumber, ','))
  {
    linesSkip.push_back(std::stoi(skipLineNumber));
  }

  auto dataSetDCP = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataSet);

  std::vector<int> validColumns;

  for(auto& field : dataSetDCP->fields)
  {
    validColumns.push_back(field.number);
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

    std::string newLine = "";
    std::stringstream strm(line);

    std::string field = "";
    int columnNumber = 0;
    while (std::getline(strm, field, ','))
    {
      if(validColumns.end() == std::find(validColumns.begin(), validColumns.end(), columnNumber))
      {
        columnNumber++;
        continue;
      }

      if(lineNumber == 0)
      {
        // Header line
        for(auto& DCPfield : dataSetDCP->fields)
        {
          if(DCPfield.number == columnNumber)
          {
            // use alias as column name
            newLine += (newLine.empty() ? "" : "," ) + DCPfield.alias;
            break;
          }
        }
      }
      else
      {
        newLine += (newLine.empty() ? "" : ",") + field;
      }

      columnNumber++;
    }

    outputFile << newLine + "\n";
    outputFile.flush();

    if(outputFile.fail())
    {
      // TODO: throw
    }

    lineNumber++;
  }

  outputFile.close();

  return QFileInfo(tempFile.fileName());
}

*/
terrama2::core::DataAccessorPtr terrama2::core::DataAccessorTxtFile::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
{
  return std::make_shared<DataAccessorTxtFile>(dataProvider, dataSeries);
}

void terrama2::core::DataAccessorTxtFile::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  std::vector<std::tuple< std::vector<std::string>, std::string, int>> fields;

  size_t lonPos = std::numeric_limits<size_t>::max();
  size_t latPos = std::numeric_limits<size_t>::max();

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);

    for(auto& field : fields)
    {
      std::string& propertyName = std::get<0>(field).at(0);

      if(propertyName == property->getName())
      {
        std::string alias = std::get<1>(field);
        int type = std::get<2>(field);

        if(alias.empty())
          alias = terrama2::core::simplifyString(property->getName());

        te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(alias, type);

        switch (type)
        {
          case te::dt::GEOMETRY_TYPE:
            delete newProperty;
            newProperty = new te::dt::SimpleProperty(alias, te::dt::DOUBLE_TYPE);

            converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));

            if(property->getName() == getLatitudePropertyName(dataSet))
              latPos = i;
            else if(property->getName() == getLongitudePropertyName(dataSet))
              lonPos = i;

            break;
          case te::dt::DOUBLE_TYPE:
            converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
            break;
          case te::dt::UINT32_TYPE:
            converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToInt, this, _1, _2, _3));
            break;
          case te::dt::DATETIME_TYPE:
            converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessorTxtFile::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet)));
            break;
          default:
            converter->add(i,property->clone());
            break;
        }
      }
    }
  }

  if(latPos != std::numeric_limits<size_t>::max() &&
     lonPos != std::numeric_limits<size_t>::max())
  {
    std::vector<size_t> latLonAttributes;
    latLonAttributes.push_back(lonPos);
    latLonAttributes.push_back(latPos);

    Srid srid = getSrid(dataSet);

    te::gm::GeometryProperty* geomProperty = new te::gm::GeometryProperty("point", srid, te::gm::PointType);

    converter->add(latLonAttributes, geomProperty, boost::bind(&terrama2::core::DataAccessorTxtFile::stringToPoint, this, _1, _2, _3, srid));
  }
}


te::dt::AbstractData* terrama2::core::DataAccessorTxtFile::stringToTimestamp(te::da::DataSet* dataset,
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

std::string terrama2::core::DataAccessorTxtFile::getLatitudePropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "latitude_property");
}

std::string terrama2::core::DataAccessorTxtFile::getLongitudePropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "longitude_property");
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

std::vector<std::tuple< std::vector<std::string>, std::string, int>>
terrama2::core::DataAccessorTxtFile::getFields(DataSetPtr dataSet) const
{
  std::vector<std::tuple< std::vector<std::string>, std::string, int>> fields;

  return fields;
}
