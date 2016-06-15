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
  \file terrama2/unittest/core/TsDataAccessorDcpInpe.cpp
  \brief Tests for Class DataAccessorDcpInpe
  \author Evandro Delatin
*/

// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpInpe.hpp>
#include <terrama2/Config.hpp>
#include <terrama2/core/Exception.hpp>

#include "TsDataAccessorDcpInpe.hpp"

// QT
#include <QObject>
#include <QDebug>

// STL
#include <iostream>
#include <fstream>

// GMock
#include <gtest/gtest.h>


void TsDataAccessorDcpInpe::TestFailAddNullDataAccessorDcpInpe()
{  
  try
  {
    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(nullptr, nullptr);

    QFAIL("Exception expected!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }  
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
  return;
}

void TsDataAccessorDcpInpe::TestFailDataProviderNull()
{
  try
  {
    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(nullptr, dataSeriesPtr);

    QFAIL("Exception expected!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
  return;
}

void TsDataAccessorDcpInpe::TestFailDataSeriesNull()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, nullptr);

    QFAIL("Exception expected!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
  return;
}

void TsDataAccessorDcpInpe::TestFailDataSeriesSemanticsInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP");

    QFAIL("Exception expected!");
  }
  catch(terrama2::core::SemanticsException& e)
  {

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
  return;
}

void TsDataAccessorDcpInpe::TestOK()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri+=TERRAMA2_DATA_DIR;
    dataProvider->uri+="/PCD_serrmar_INPE";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-inpe");

    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "30885.txt");
    dataSet->format.emplace("timezone", "+00");
    //dataSet->format.emplace("folder", "");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

    assert(dcpSeries->getDcpSeries().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->getDcpSeries().begin()).second.syncDataSet->dataset();

    std::string uri = dataProvider->uri;
    std::string mask = dataSet->format.at("mask");

    QUrl url((uri+"/"+mask).c_str());
    QFile file(url.path());
    file.open(QIODevice::QIODevice::ReadOnly);

    int numberLinesOriginalFile = -1;
    // Get Number Lines Original File.
    while (!file.atEnd())
    {
      file.readLine();
      ++numberLinesOriginalFile;
    }

    QStringList numberPropertiesOriginalFile;

    if(file.seek(0))
    {
      // Get number Properties Original File.
      QTextStream in(&file);
      QString line = in.readLine();
      numberPropertiesOriginalFile = line.split(",");
    }

    file.close();

    // Get Number Properties New File.
    int numberPropertiesNewFile = teDataSet->getNumProperties();

    // Get Number Lines New File.
    int numberLinesNewFile = teDataSet->size();

    QCOMPARE(numberLinesOriginalFile,numberLinesNewFile);
    QCOMPARE(numberPropertiesOriginalFile.size(),numberPropertiesNewFile);

  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }

  return;

}
