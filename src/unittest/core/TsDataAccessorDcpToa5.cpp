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

class RaiiTsDataAccessorDcpToa5
{
  public:

    RaiiTsDataAccessorDcpToa5(const std::string& dataProviderType,
                              const terrama2::core::DataRetrieverFactory::FactoryFnctType& f)
      : dataProviderType_(dataProviderType), f_(f)
    {
      terrama2::core::DataRetrieverFactory::getInstance().add(dataProviderType_, f_);
    }

    ~RaiiTsDataAccessorDcpToa5()
    {
      terrama2::core::DataRetrieverFactory::getInstance().remove(dataProviderType_);
    }

  private:
    std::string dataProviderType_;
    terrama2::core::DataRetrieverFactory::FactoryFnctType f_;
};

class RaiiDataSourceTsDataAccessorDcpToa5
{
  public:
    RaiiDataSourceTsDataAccessorDcpToa5(const std::string& type,
                                        const te::da::DataSourceFactory::FactoryFnctType& ft ) : type_(type), ft_(ft)
    {
      if(te::da::DataSourceFactory::find(type_))
      {
        te::da::DataSourceFactory::remove(type_);
        te::da::DataSourceFactory::add(type_,ft_);
      }
      else te::da::DataSourceFactory::add(type_,ft_);
    }

    ~RaiiDataSourceTsDataAccessorDcpToa5()
    {
      te::da::DataSourceFactory::remove(type_);
    }

  private:
    std::string type_;
    te::da::DataSourceFactory::FactoryFnctType ft_;
};


te::da::MockDataSet* create_MockDataSetDcpToa5()
{
  te::da::MockDataSet* mockDataSet(new ::testing::NiceMock<te::da::MockDataSet>());

  ON_CALL(*mockDataSet, moveNext()).WillByDefault(::testing::Return(false));

  return mockDataSet;
}

te::da::MockDataSourceTransactor* create_MockDataSourceTransactorDcpToa5()
{
  te::da::MockDataSourceTransactor* mockDataSourceTransactor(new ::testing::NiceMock<te::da::MockDataSourceTransactor>());

  std::vector<std::string> dataSetNames;
  dataSetNames.push_back("GRM_slow_2014_01_02_1713");
  std::string name = "GRM_slow_2014_01_02_1713";

  EXPECT_CALL(*mockDataSourceTransactor, getDataSetNames()).WillOnce(::testing::Return(dataSetNames));
  EXPECT_CALL(*mockDataSourceTransactor, DataSetTypePtrReturn()).WillOnce(::testing::Return(new te::da::DataSetType(name)));
  ON_CALL(*mockDataSourceTransactor, DataSetPtrReturn()).WillByDefault(::testing::Invoke(&create_MockDataSetDcpToa5));

  return mockDataSourceTransactor;
}


void TsDataAccessorDcpToa5::TestFailAddNullDataAccessorDcpToa5()
{
  try
  {
    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(nullptr, nullptr);

    QFAIL("Exception expected!");
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

    QFAIL("Exception expected!");
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

    QFAIL("Exception expected!");
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

    QFAIL("Exception expected!");
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
    dataProvider->dataProviderType = "MOCK";
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

    auto mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock_, isRetrivable()).WillOnce(Return(false));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_);

    RaiiTsDataAccessorDcpToa5 raiiDataRetriever("MOCK",makeMock);

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);
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
    dataSet->format.emplace("mask", "GRM_slow_2014_01_02_1713.dat");
    dataSet->format.emplace("timezone", "+00");
    dataSet->format.emplace("folder", "GRM");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    QString errMsg = QObject::tr("Non retrievable DataRetriever.");
    terrama2::core::NotRetrivableException exceptionMock;
    exceptionMock << terrama2::ErrorDescription(errMsg);

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    auto mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock_, isRetrivable()).WillOnce(Return(true));
    EXPECT_CALL(*mock_, retrieveData(_,_)).WillOnce(testing::Throw(exceptionMock));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_);

    RaiiTsDataAccessorDcpToa5 raiiDataRetriever("MOCK",makeMock);

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);
      QFAIL("Exception expected!");
    }
    catch(terrama2::core::NotRetrivableException&)
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
    dataSet->format.emplace("mask", "GRM_slow_2014_01_02_1713.dat");

    dataSet->format.emplace("timezone", "+00");
    dataSet->format.emplace("folder", "GRM");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<te::da::MockDataSource> mock_(new ::testing::NiceMock<te::da::MockDataSource>());

    EXPECT_CALL(*mock_, setConnectionInfo(_)).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, open()).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, isOpened()).WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_, close()).WillRepeatedly(Return());

    auto makeMock = std::bind(te::da::MockDataSource::makeMockDataSource, mock_.release());

    RaiiDataSourceTsDataAccessorDcpToa5 raiiDataSource("OGR",makeMock);

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);
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

    std::unique_ptr<te::da::MockDataSource> mock_(new ::testing::NiceMock<te::da::MockDataSource>());

    EXPECT_CALL(*mock_, setConnectionInfo(_)).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, open()).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, isOpened()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_, DataSourceTransactoPtrReturn()).WillRepeatedly(::testing::Invoke(&create_MockDataSourceTransactorDcpToa5));
    EXPECT_CALL(*mock_, close()).WillRepeatedly(Return());

    auto makeMock = std::bind(te::da::MockDataSource::makeMockDataSource, mock_.release());

    RaiiDataSourceTsDataAccessorDcpToa5 raiiDataSource("OGR",makeMock);

    try
    {
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);
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
    te::da::DataSourceFactory::add(OGR_DRIVER_IDENTIFIER, te::ogr::Build);
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
