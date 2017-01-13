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

// TerraLib
#include <terralib/datatype/DateTimeProperty.h>

//QT
#include <QUrl>
#include <QDir>
#include <QSet>
#include <QTemporaryFile>

// STL
#include <fstream>


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


terrama2::core::DataAccessorPtr terrama2::core::DataAccessorTxtFile::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
{
  return std::make_shared<DataAccessorTxtFile>(dataProvider, dataSeries);
}

void terrama2::core::DataAccessorTxtFile::adapt(DataSetPtr dataset, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);

    if(property->getName() == getTimestampPropertyName(dataset))
    {/*
      te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty("DateTime", te::dt::TIME_INSTANT_TZ);
      // datetime column found
      converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorDcpToa5::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataset)));
      continue;*/
    }
  }
}
