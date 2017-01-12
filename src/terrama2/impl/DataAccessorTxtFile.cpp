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
    // throw
  }

  QFileInfo filteredFileInfo = filterTxt(fileInfo, tempFile);

  return DataAccessorFile::readFile(series, completeDataset, converter, filteredFileInfo, mask, dataSet);
}

QFileInfo terrama2::core::DataAccessorTxtFile::filterTxt(QFileInfo& fileInfo, QTemporaryFile& tempFile) const
{
  std::ifstream file(fileInfo.absoluteFilePath().toStdString());

  if(!file.is_open())
  {
    // throw
  }

  std::ofstream outputFile(tempFile.fileName().toStdString());

  if(!outputFile.is_open())
  {
    // throw
  }

  std::string line = "";
  int lineNumber = 0;

  while(std::getline(file, line))
  {
    std::string newLine = "";
    std::stringstream strm(line);

    std::string field = "";
    int columnNumber = 0;
    while (std::getline(strm, field, ','))
    {
      newLine += (columnNumber == 0 ? field : "," + field);
      columnNumber++;
    }

    outputFile << newLine + "\n";
    outputFile.flush();

    if(outputFile.fail())
    {
      // throw
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
