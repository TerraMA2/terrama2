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

//terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/ogr/Config.h>
#include <terralib/ogr/DataSource.h>

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
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include "TsDataAccessorDcpInpe.hpp"
#include "MockDataRetriever.hpp"
#include "MockDataSource.hpp"
#include "MockDataSourceTransactor.hpp"
#include "MockDataSet.hpp"
#include "Utils.hpp"

// QT
#include <QObject>
#include <QDebug>

// STL
#include <iostream>
#include <fstream>
#include <functional>

// GMock
#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::_;

//FIXME: using a real dir, can be improved using some mock access

void TsDataAccessorDcpInpe::TestFailAddNullDataAccessorDcpInpe()
{
  try
  {
    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(nullptr, nullptr);

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::DataAccessorException&)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorDcpInpe::TestFailDataProviderNull()
{
  try
  {
    terrama2::core::DataSeriesPtr dataSeriesPtr(new terrama2::core::DataSeries());

    try
    {
      terrama2::core::DataAccessorDcpInpe accessor(nullptr, dataSeriesPtr);
      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::DataAccessorException&)
    {

    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch (...)
  {
    QFAIL("Unexpected exception!");
  }

}

void TsDataAccessorDcpInpe::TestFailDataSeriesNull()
{
  try
  {
    terrama2::core::DataProviderPtr dataProviderPtr(new terrama2::core::DataProvider());

    try
    {
      terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, nullptr);
      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::DataAccessorException&)
    {

    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch (...)
  {
    QFAIL("Unexpected exception!");
  }

}

void TsDataAccessorDcpInpe::TestFailDataSeriesSemanticsInvalid()
{
  //FIXME: move this test to the semantics test
  try
  {
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    semanticsManager.getSemantics("DCP");

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::SemanticsException&)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorDcpInpe::TestFailDataRetrieverInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "mock://mock_provider";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-inpe");

    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "mockMask");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    terrama2::core::NotRetrivableException exceptionMock;

    QString errMsg = QObject::tr("Non retrievable DataRetriever.");
    exceptionMock << terrama2::ErrorDescription(errMsg);

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);

    auto mock = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock, isRetrivable()).WillOnce(Return(true));
    EXPECT_CALL(*mock, retrieveData(_,_,_,_,_,_)).WillOnce(testing::Throw(exceptionMock));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock);

    DataRetrieverFactoryRaii raiiDataRetriever("MOCK",makeMock);

    auto remover = std::make_shared<terrama2::core::FileRemover>();

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);
      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::NotRetrivableException&)
    {

    }
  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
}

void TsDataAccessorDcpInpe::TestFailDataSourceInvalid()
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

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<te::da::MockDataSource> mock_(new ::testing::NiceMock<te::da::MockDataSource>());

    EXPECT_CALL(*mock_, open()).WillOnce(Return());
    EXPECT_CALL(*mock_, isOpened()).WillOnce(Return(false));
    EXPECT_CALL(*mock_, close()).WillOnce(Return());

    auto makeMock = std::bind(te::da::MockDataSource::makeMockDataSource, mock_.release());
    DataSourceFactoryRaii raiiDataSource("OGR", std::move(makeMock));

    auto remover = std::make_shared<terrama2::core::FileRemover>();

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);
      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::NoDataException&)
    {

    }
  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
}

void TsDataAccessorDcpInpe::TestFailDataSetInvalid()
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

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<te::da::MockDataSource> mock_(new ::testing::NiceMock<te::da::MockDataSource>());
    std::unique_ptr<te::da::MockDataSourceTransactor> mockDataSourceTransactor(new ::testing::NiceMock<te::da::MockDataSourceTransactor>());

    std::unique_ptr<te::da::MockDataSet> mockDataSet(new ::testing::NiceMock<te::da::MockDataSet>());
    EXPECT_CALL(*mockDataSet, moveNext()).WillOnce(::testing::Return(false));

    auto create_MockDataSet = [](te::da::MockDataSet* mockDataSet)->te::da::MockDataSet* { return mockDataSet; };

    std::string name = "mock";
    std::vector<std::string> dataSetNames = {name};

    EXPECT_CALL(*mockDataSourceTransactor, getDataSetNames()).WillOnce(::testing::Return(dataSetNames));
    EXPECT_CALL(*mockDataSourceTransactor, DataSetTypePtrReturn()).WillOnce(::testing::Return(new te::da::DataSetType(name)));
    EXPECT_CALL(*mockDataSourceTransactor, DataSetPtrReturn()).WillOnce(::testing::Invoke(std::bind(create_MockDataSet, mockDataSet.release())));

    auto create_MockDataSourceTransactor = [](te::da::MockDataSourceTransactor* mockTransactor)->te::da::MockDataSourceTransactor* { return mockTransactor; };

    EXPECT_CALL(*mock_, open()).WillOnce(Return());
    EXPECT_CALL(*mock_, isOpened()).WillOnce(Return(true));
    EXPECT_CALL(*mock_, DataSourceTransactoPtrReturn()).WillOnce(::testing::Invoke(std::bind(create_MockDataSourceTransactor, mockDataSourceTransactor.release())));
    EXPECT_CALL(*mock_, close()).WillOnce(Return());

    auto makeMock = std::bind(te::da::MockDataSource::makeMockDataSource, mock_.release());
    DataSourceFactoryRaii raiiDataSource("OGR", std::move(makeMock));

    auto remover = std::make_shared<terrama2::core::FileRemover>();

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);
      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::NoDataException&)
    {

    }
  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}

void TsDataAccessorDcpInpe::TestOK()
{
  QFile file(QString::fromStdString(TERRAMA2_DATA_DIR+"/PCD_serrmar_INPE/30885.txt"));
  if(!file.exists())
  {
    QFAIL("Test data not found.");
  }

  try
  {
    // add OGR build to the factory if not present
    if(!te::da::DataSourceFactory::find(OGR_DRIVER_IDENTIFIER))
      te::da::DataSourceFactory::add(OGR_DRIVER_IDENTIFIER, te::ogr::Build);

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = TERRAMA2_DATA_DIR;

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-inpe");

    std::shared_ptr<terrama2::core::DataSetDcp> dataSet(new terrama2::core::DataSetDcp());
    dataSet->active = true;
    dataSet->format.emplace("mask", "/PCD_serrmar_INPE/30885.txt");
    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);

    assert(dcpSeries->dcpSeriesMap().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->dcpSeriesMap().begin()).second.syncDataSet->dataset();

    std::string uri = dataProvider->uri;
    std::string mask = terrama2::core::getFileMask(dataSet);

    QUrl url((uri+"/PCD_serrmar_INPE/"+mask).c_str());
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



    // compare number of lines of the file and of the dataset
    int numberLinesNewFile = teDataSet->size();
    QCOMPARE(numberLinesOriginalFile,numberLinesNewFile);

    // compare number of properties of the file and of the dataset
    int numberPropertiesNewFile = teDataSet->getNumProperties();
    QCOMPARE(numberPropertiesOriginalFile.size(),numberPropertiesNewFile);

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;
}
