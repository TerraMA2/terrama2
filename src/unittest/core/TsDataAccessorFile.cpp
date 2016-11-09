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
  \file terrama2/codebase/src/unittest/core/TsDataAccessorFile.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "TsDataAccessorFile.hpp"
#include "TestDataAccessorFile.hpp"

// Qt
#include <QTemporaryDir>

// GMock
#include <gtest/gtest.h>

void TsDataAccessorFile::testGetFoldersList()
{
  QTemporaryDir dir(QDir::tempPath() + QString::fromStdString("/2016"));
  QTemporaryDir dir2(QDir::tempPath() + QString::fromStdString("/2020"));
  QTemporaryDir dir3(QDir::tempPath() + QString::fromStdString("/folder"));

  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

  TestDataAccessorFile da(dataProviderPtr, dataSeriesPtr);

  QFileInfoList foldersList = da.getFoldersList(QDir::tempPath().toStdString(), "2016*");

  if(foldersList.size() != 1)
    QFAIL("Wrong number of folders matched!");
}
