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
  \file terrama2/unittest/core/TsDataAccessorDcpToa5.cpp
  \brief Tests for Class DataAccessorDcpToa5
  \author Evandro Delatin
*/

// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpToa5.hpp>
#include <terrama2/Config.hpp>
#include <terrama2/core/Exception.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include "TsDataAccessorDcpToa5.hpp"
#include "MockDataRetriever.hpp"

// QT
#include <QObject>
#include <QDebug>

// STL
#include <iostream>
#include <fstream>

// GMock
#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::_;


void TsDataAccessorDcpToa5::TestFailAddNullDataAccessorDcpToa5()
{
  try
  {
    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(nullptr, nullptr);

    QFAIL("Expected Exception!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }
  return;
}

void TsDataAccessorDcpToa5::TestFailDataProviderNull()
{
  try
  {
    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(nullptr, dataSeriesPtr);

    QFAIL("Expected Exception!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }
  return;
}

void TsDataAccessorDcpToa5::TestFailDataSeriesNull()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, nullptr);

    QFAIL("Expected Exception!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }
  return;
}

void TsDataAccessorDcpToa5::TestFailDataSeriesSemanticsInvalid()
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

    QFAIL("Expected Exception!");
  }
  catch(terrama2::core::SemanticsException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }
  return;
}

void TsDataAccessorDcpToa5::TestOKDataRetrieverValid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/pcd_toa5";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-toa5");

    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "GRM_slow_2014_01_02_1713.dat");
    dataSet->format.emplace("timezone", "+00");
    dataSet->format.emplace("folder", "GRM");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    std::string uri = "";
    std::string mask = dataSet->format.at("mask");

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<MockDataRetriever> mock_(new MockDataRetriever(dataProviderPtr));

    ON_CALL(*mock_, isRetrivable()).WillByDefault(Return(false));
    ON_CALL(*mock_, retrieveData(_,_)).WillByDefault(Return(uri));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_.get());
    terrama2::core::DataRetrieverFactory::getInstance().add("DCP-toa5", makeMock);

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);
    }
    catch(const terrama2::Exception&)
    {
      QFAIL("Unexpected Exception!");
    }

    terrama2::core::DataRetrieverFactory::getInstance().remove("DCP-toa5");

  }
  catch(terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }

  return;

}

void TsDataAccessorDcpToa5::TestFailDataRetrieverInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/pcd_toa5";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-toa5");

    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "GRM_slow_2014_01_02_1713.dat");
    dataSet->format.emplace("timezone", "+00");
    dataSet->format.emplace("folder", "GRM");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    std::string uri = "";
    std::string mask = dataSet->format.at("mask");

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<MockDataRetriever> mock_(new MockDataRetriever(dataProviderPtr));

    ON_CALL(*mock_, isRetrivable()).WillByDefault(Return(true));
    ON_CALL(*mock_, retrieveData(_,_)).WillByDefault(Return(uri));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_.get());
    terrama2::core::DataRetrieverFactory::getInstance().add("DCP-toa5", makeMock);

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);
    }
    catch(const terrama2::Exception&)
    {
      QFAIL("Expected Exception!");
    }

    terrama2::core::DataRetrieverFactory::getInstance().remove("DCP-toa5");

  }
  catch(terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }

  return;

}

void TsDataAccessorDcpToa5::TestOK()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/pcd_toa5";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-toa5");

    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "GRM_slow_2014_01_02_1713.dat");

    dataSet->format.emplace("timezone", "+00");
    dataSet->format.emplace("folder", "GRM");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

    assert(dcpSeries->dcpSeriesMap().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->dcpSeriesMap().begin()).second.syncDataSet->dataset();

    std::string uri = dataProvider->uri;
    std::string mask = dataSet->format.at("mask");
    std::string folder = dataSet->format.at("folder");

    QUrl url((uri+"/"+folder+"/"+mask).c_str());
    QFileInfo originalInfo(url.path());
    QFile file(originalInfo.absoluteFilePath());
    QString errMsg;
    QFileDevice::FileError err = QFileDevice::NoError;
    if (!file.open(QIODevice::ReadOnly))
    {
      QFAIL("Unexpected Exception!");
      errMsg = file.errorString();
      err = file.error();
    }

    int numberLinesOriginalFile = -4; // ignore header lines
    // Get Number Lines Original File.
    while (!file.atEnd())
    {
      file.readLine();
      ++numberLinesOriginalFile;
    }

    QStringList numberPropertiesOriginalFile;

    // Get number Properties Original File.
    if(file.seek(0))
    {
      //ignore first line
      file.readLine();
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
  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }

  return;

}
