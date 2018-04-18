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
  \file terrama2/unittest/core/TsDataAccessorOccurrenceWfp.cpp
  \brief Tests for Class DataAccessorOccurrenceWfp
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
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/impl/DataAccessorOccurrenceWfp.hpp>
#include <terrama2/core/data-access/OccurrenceSeries.hpp>
#include <terrama2/Config.hpp>
#include <terrama2/core/Exception.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include "TsDataAccessorOccurrenceWfp.hpp"
#include "MockDataRetriever.hpp"
#include "MockDataSource.hpp"
#include "MockDataSourceTransactor.hpp"
#include "MockDataSet.hpp"

// QT
#include <QObject>
#include <QDebug>
#include <QString>

// STL
#include <iostream>
#include <fstream>

// GMock
#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::_;

class RaiiTsDataAccessorOccurrenceWfp
{
  public:

    RaiiTsDataAccessorOccurrenceWfp(const std::string& dataProviderType,
                                    const terrama2::core::DataRetrieverFactory::FactoryFnctType& f)
      : dataProviderType_(dataProviderType), f_(f)
    {
      terrama2::core::DataRetrieverFactory::getInstance().add(dataProviderType_, f_);
    }

    ~RaiiTsDataAccessorOccurrenceWfp()
    {
      terrama2::core::DataRetrieverFactory::getInstance().remove(dataProviderType_);
    }

  private:
    std::string dataProviderType_;
    terrama2::core::DataRetrieverFactory::FactoryFnctType f_;
};

class RaiiDataSourceTsDataAccessorOccurrenceWfp
{
  public:
    RaiiDataSourceTsDataAccessorOccurrenceWfp(const std::string& type,
                                              const te::da::DataSourceFactory::FactoryFnctType& ft ) : type_(type), ft_(std::move(ft))
    {
      if(te::da::DataSourceFactory::find(type_))
      {
        te::da::DataSourceFactory::remove(type_);
        te::da::DataSourceFactory::add(type_,ft_);
      }
      else te::da::DataSourceFactory::add(type_,ft_);
    }

    ~RaiiDataSourceTsDataAccessorOccurrenceWfp()
    {
      te::da::DataSourceFactory::remove(type_);
    }

  private:
    std::string type_;
    te::da::DataSourceFactory::FactoryFnctType ft_;
};

te::da::MockDataSet* create_MockDataSetOccurrenceWfp()
{
  te::da::MockDataSet* mockDataSet(new ::testing::NiceMock<te::da::MockDataSet>());

  ON_CALL(*mockDataSet, moveNext()).WillByDefault(::testing::Return(false));

  return mockDataSet;
}

te::da::MockDataSourceTransactor* create_MockDataSourceTransactorOccurrenceWfp()
{
  te::da::MockDataSourceTransactor* mockDataSourceTransactor(new ::testing::NiceMock<te::da::MockDataSourceTransactor>());

  std::vector<std::string> dataSetNames;
  dataSetNames.push_back("exporta_20160501_0230");
  std::string name = "exporta_20160501_0230";

  EXPECT_CALL(*mockDataSourceTransactor, getDataSetNames()).WillOnce(::testing::Return(dataSetNames));
  EXPECT_CALL(*mockDataSourceTransactor, DataSetTypePtrReturn()).WillOnce(::testing::Return(new te::da::DataSetType(name)));
  ON_CALL(*mockDataSourceTransactor, DataSetPtrReturn()).WillByDefault(::testing::Invoke(&create_MockDataSetOccurrenceWfp));

  return mockDataSourceTransactor;
}


void TsDataAccessorOccurrenceWfp::TestFailAddNullDataAccessorOccurrenceWfp()
{
  try
  {
    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(nullptr, nullptr);

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorOccurrenceWfp::TestFailDataProviderNull()
{
  try
  {
    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(nullptr, dataSeriesPtr);

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorOccurrenceWfp::TestFailDataSeriesNull()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, nullptr);

    QFAIL("Exception expected!");
  }
  catch(const terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorOccurrenceWfp::TestFailDataSeriesSemanticsInvalid()
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
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorOccurrenceWfp::TestOKDataRetrieverValid()
{
  QFile file(QString::fromStdString(TERRAMA2_DATA_DIR+"/fire_system/exporta_20160501_0230.csv"));
  if(!file.exists())
  {
    QFAIL("Test data not found.");
  }

  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "exporta_20160501_0230.csv");
    dataSet->format.emplace("srid", "4326");
    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);

    auto mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock_, isRetrivable()).WillOnce(Return(false));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_);

    RaiiTsDataAccessorOccurrenceWfp raiiDataRetriever("MOCK",makeMock);

    try
    {
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter, remover);
    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch(...)
  {
    QFAIL("Unexpected exception not related to the method getOccurrenceSeries!");
  }

  return;

}

void TsDataAccessorOccurrenceWfp::TestFailDataRetrieverInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "exporta_20160501_0230.csv");
    dataSet->format.emplace("srid", "4326");
    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    QString errMsg = QObject::tr("Non retrievable DataRetriever.");
    terrama2::core::NotRetrivableException exceptionMock;
    exceptionMock << terrama2::ErrorDescription(errMsg);

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);

    auto mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock_, isRetrivable()).WillOnce(Return(true));
    EXPECT_CALL(*mock_, retrieveData(_,_,_,_,_,_)).WillOnce(testing::Throw(exceptionMock));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_);

    RaiiTsDataAccessorOccurrenceWfp raiiDataRetriever("MOCK",makeMock);

    try
    {
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter, remover);
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

  return;

}

void TsDataAccessorOccurrenceWfp::TestFailDataSourceInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "exporta_20160501_0230.csv");
    dataSet->format.emplace("srid", "4326");
    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<te::da::MockDataSource> mock_(new ::testing::NiceMock<te::da::MockDataSource>());

    EXPECT_CALL(*mock_, setConnectionInfo(_)).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, open()).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, isOpened()).WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_, close()).WillRepeatedly(Return());

    auto makeMock = std::bind(te::da::MockDataSource::makeMockDataSource, mock_.release());

    RaiiDataSourceTsDataAccessorOccurrenceWfp raiiDataSource("OGR", std::move(makeMock));

    try
    {
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter, remover);
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

void TsDataAccessorOccurrenceWfp::TestFailDataSetInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "exporta_20160501_0230.csv");
    dataSet->format.emplace("srid", "4326");
    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);

    std::unique_ptr<te::da::MockDataSource> mock_(new ::testing::NiceMock<te::da::MockDataSource>());

    EXPECT_CALL(*mock_, setConnectionInfo(_)).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, open()).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, isOpened()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_, DataSourceTransactoPtrReturn()).WillRepeatedly(::testing::Invoke(&create_MockDataSourceTransactorOccurrenceWfp));
    EXPECT_CALL(*mock_, close()).WillRepeatedly(Return());

    auto makeMock = std::bind(te::da::MockDataSource::makeMockDataSource, mock_.release());

    RaiiDataSourceTsDataAccessorOccurrenceWfp raiiDataSource("OGR", std::move(makeMock));

    try
    {
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter, remover);
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

void TsDataAccessorOccurrenceWfp::TestOK()
{
  QFile file(QString::fromStdString(TERRAMA2_DATA_DIR+"/fire_system/exporta_20160501_0230.csv"));
  if(!file.exists())
  {
    QFAIL("Test data not found.");
  }


  try
  {
    te::da::DataSourceFactory::add(OGR_DRIVER_IDENTIFIER, te::ogr::Build);
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "exporta_20160501_0230.csv");
    dataSet->format.emplace("srid", "4326");
    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter, remover);

    assert(occurrenceSeries->occurrencesMap().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*occurrenceSeries->occurrencesMap().begin()).second.syncDataSet->dataset();

    std::string uri = dataProvider->uri;
    std::string mask = dataSet->format.at("mask");

    QUrl url((uri+"/"+mask).c_str());
    QFile file(url.path());
    file.open(QIODevice::QIODevice::ReadOnly);

    std::size_t numberLinesOriginalFile = 0;
    // Get Number Lines Original File.
    while (!file.atEnd())
    {
      file.readLine();
      ++numberLinesOriginalFile;
    }
    // first line is header
    --numberLinesOriginalFile;

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
    auto numberPropertiesNewFile = teDataSet->getNumProperties();

    // Get Number Lines New File.
    auto numberLinesNewFile = teDataSet->size();

    QCOMPARE(numberLinesOriginalFile,numberLinesNewFile);
    QCOMPARE(numberPropertiesOriginalFile.size(), 4);
    QCOMPARE(numberPropertiesNewFile, 3ul);
  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}
