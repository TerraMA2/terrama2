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
//#include "MockDataRetriever.hpp"

// QT
#include <QObject>
#include <QDebug>

// STL
#include <iostream>
#include <fstream>

// GMock
#include <gtest/gtest.h>

//using ::testing::Return;
//using ::testing::_;


void TsDataAccessorOccurrenceWfp::TestFailAddNullDataAccessorOccurrenceWfp()
{
  try
  {
    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(nullptr, nullptr);

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
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
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
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
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
  catch(terrama2::core::SemanticsException& e)
  {

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
  return;
}

/*
void TsDataAccessorOccurrenceWfp::TestOKDataRetrieverValid()
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
    std::string uri = "";
    std::string mask = dataSet->format.at("mask");

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);

    MockDataRetriever *mock_ = new MockDataRetriever(dataProviderPtr);

    MockDataRetriever::setMockDataRetriever(mock_);

    ON_CALL(*mock_, isRetrivable()).WillByDefault(Return(false));
    ON_CALL(*mock_, retrieveData(_,_)).WillByDefault(Return(uri));

    terrama2::core::DataRetrieverFactory::getInstance().add("OCCURRENCE-wfp", MockDataRetriever::makeMockDataRetriever);

    try
    {
      terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter);
    }
    catch(const terrama2::Exception&)
    {
      QFAIL("Exception expected!");
    }

    terrama2::core::DataRetrieverFactory::getInstance().remove("OCCURRENCE-wfp");
    delete mock_;

  }
  catch(terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Exception unexpected!");
  }

  return;

}
*/
/*
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
    std::string uri = "";
    std::string mask = dataSet->format.at("mask");

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);

    MockDataRetriever *mock_ = new MockDataRetriever(dataProviderPtr);

    MockDataRetriever::setMockDataRetriever(mock_);

    ON_CALL(*mock_, isRetrivable()).WillByDefault(Return(true));
    ON_CALL(*mock_, retrieveData(_,_)).WillByDefault(Return(uri));

    terrama2::core::DataRetrieverFactory::getInstance().add("OCCURRENCE-wfp", MockDataRetriever::makeMockDataRetriever);

    try
    {
      terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter);
    }
    catch(const terrama2::Exception&)
    {
      QFAIL("Exception expected!");
    }

    terrama2::core::DataRetrieverFactory::getInstance().remove("OCCURRENCE-wfp");
    delete mock_;

  }
  catch(terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Exception unexpected!");
  }

  return;

}
*/

void TsDataAccessorOccurrenceWfp::TestOK()
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
    terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter);

    assert(occurrenceSeries->getOccurrences().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*occurrenceSeries->getOccurrences().begin()).second.syncDataSet->dataset();

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

    numberPropertiesOriginalFile.removeFirst();

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
