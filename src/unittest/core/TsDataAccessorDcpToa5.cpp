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
#include <terrama2/impl/DataAccessorDcpToa5.hpp>
#include <terrama2/Config.hpp>
#include <terrama2/core/Exception.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include "TsDataAccessorDcpToa5.hpp"
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

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::DataAccessorException& e)
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

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::DataAccessorException& e)
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

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::DataAccessorException& e)
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

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::SemanticsException& e)
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
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-toa5");

    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "/GRM/GRM_slow_2014_01_02_1713.dat");
    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    try
    {
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);
    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch(...)
  {
    QFAIL("Unexpected exception not related to the method getDcpSeries!");
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
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-toa5");

    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "/GRM/GRM_slow_2014_01_02_1713.dat");
    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    try
    {
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);
      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::NotRetrivableException&)
    {

    }
  }
  catch(...)
  {
    QFAIL("Unexpected Exception!");
  }

  return;

}

void TsDataAccessorDcpToa5::TestFailDataSourceInvalid()
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
    dataSet->format.emplace("mask", "/GRM/GRM_slow_2014_01_02_1713.dat");

    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    try
    {
      auto remover = std::make_shared<terrama2::core::FileRemover>();
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

void TsDataAccessorDcpToa5::TestFailDataSetInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    //TODO: using a real dir, can be improved using some mock access
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
    dataSet->format.emplace("mask", "/GRM/GRM_slow_2014_01_02_1713.dat");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<te::da::MockDataSource> mock_(new ::testing::NiceMock<te::da::MockDataSource>());

    std::unique_ptr<te::da::MockDataSourceTransactor> mockDataSourceTransactor(new ::testing::NiceMock<te::da::MockDataSourceTransactor>());
    std::string name = "GRM_slow_2014_01_02_1713";
    std::vector<std::string> dataSetNames= {name};

    std::unique_ptr<te::da::MockDataSet> mockDataSet(new ::testing::NiceMock<te::da::MockDataSet>());
    EXPECT_CALL(*mockDataSet, moveNext()).WillOnce(::testing::Return(false));

    auto create_MockDataSet = [](te::da::MockDataSet* mockDataSet)->te::da::MockDataSet* { return mockDataSet; };

    EXPECT_CALL(*mockDataSourceTransactor, getDataSetNames()).WillOnce(::testing::Return(dataSetNames));
    EXPECT_CALL(*mockDataSourceTransactor, DataSetTypePtrReturn()).WillOnce(::testing::Return(new te::da::DataSetType(name)));
    EXPECT_CALL(*mockDataSourceTransactor, DataSetPtrReturn()).WillOnce(::testing::Invoke(std::bind(create_MockDataSet, mockDataSet.release())));

    auto create_MockDataSourceTransactor = [](te::da::MockDataSourceTransactor* mockDataSourceTransactor)-> te::da::MockDataSourceTransactor* {return mockDataSourceTransactor;};

    EXPECT_CALL(*mock_, open()).WillOnce(Return());
    EXPECT_CALL(*mock_, isOpened()).WillOnce(Return(true));
    EXPECT_CALL(*mock_, DataSourceTransactoPtrReturn()).WillOnce(::testing::Invoke(std::bind(create_MockDataSourceTransactor, mockDataSourceTransactor.release())));
    EXPECT_CALL(*mock_, close()).WillOnce(Return());

    auto makeMock = std::bind(te::da::MockDataSource::makeMockDataSource, mock_.release());

    DataSourceFactoryRaii raiiDataSource("OGR",makeMock);
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

void TsDataAccessorDcpToa5::TestOK()
{
  try
  {
    // add OGR build to the factory if not present
    if(!te::da::DataSourceFactory::find(OGR_DRIVER_IDENTIFIER))
      te::da::DataSourceFactory::add(OGR_DRIVER_IDENTIFIER, te::ogr::Build);

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    //TODO: using a real dir, can be improved using some mock access
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
    dataSet->format.emplace("mask", "/GRM/GRM_slow_2014_01_02_1713.dat");

    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);

    assert(dcpSeries->dcpSeriesMap().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->dcpSeriesMap().begin()).second.syncDataSet->dataset();

    std::string uri = dataProvider->uri;

    terrama2::core::DataSetPtr dataSetPtr(new terrama2::core::DataSetDcp(*dataSet));
    std::string mask = terrama2::core::getFileMask(dataSetPtr);
    std::string folder = terrama2::core::getFolderMask(dataSetPtr);

    QUrl url(QString::fromStdString(uri+"/"+folder+"/"+mask));
    QFileInfo originalInfo(url.path());
    QFile file(originalInfo.absoluteFilePath());

    if(!file.open(QIODevice::ReadOnly))
      QFAIL("Unexpected Exception!");

    int numberLinesOriginalFile = -4; // ignore header lines
    // Get Number Lines Original File.
    while(!file.atEnd())
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
