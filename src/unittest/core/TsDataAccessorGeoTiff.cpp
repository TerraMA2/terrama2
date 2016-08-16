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
  \file terrama2/unittest/core/TsDataAccessorGeoTiff.cpp
  \brief Tests for Class DataAccessorGeoTiff
  \author Evandro Delatin
*/

// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/impl/DataAccessorGeoTiff.hpp>
#include <terrama2/core/data-access/GridSeries.hpp>
#include <terrama2/impl/DataAccessorGeoTiff.hpp>
#include <terrama2/Config.hpp>
#include <terrama2/core/Exception.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include "TsDataAccessorGeoTiff.hpp"
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

class RaiiTsDataAccessorGeoTiff
{
  public:

    RaiiTsDataAccessorGeoTiff(const std::string& dataProviderType,
                              const terrama2::core::DataRetrieverFactory::FactoryFnctType& f)
      : dataProviderType_(dataProviderType), f_(f)
    {
      terrama2::core::DataRetrieverFactory::getInstance().add(dataProviderType_, f_);
    }

    ~RaiiTsDataAccessorGeoTiff()
    {
      terrama2::core::DataRetrieverFactory::getInstance().remove(dataProviderType_);
    }

  private:
    std::string dataProviderType_;
    terrama2::core::DataRetrieverFactory::FactoryFnctType f_;
};

void TsDataAccessorGeoTiff::TestFailAddNullDataAccessorGeoTiff()
{
  try
  {
    //accessing data
    terrama2::core::DataAccessorGeoTiff accessor(nullptr, nullptr);

    QFAIL("Exception expected!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorGeoTiff::TestFailDataProviderNull()
{
  try
  {
    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

    //accessing data
    terrama2::core::DataAccessorGeoTiff accessor(nullptr, dataSeriesPtr);

    QFAIL("Exception expected!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorGeoTiff::TestFailDataSeriesNull()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

    //accessing data
    terrama2::core::DataAccessorGeoTiff accessor(dataProviderPtr, nullptr);

    QFAIL("Exception expected!");
  }
  catch(terrama2::core::DataAccessorException& e)
  {

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorGeoTiff::TestFailDataSeriesSemanticsInvalid()
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
    QFAIL("Unexpected exception!");
  }
  return;
}

void TsDataAccessorGeoTiff::TestOKDataRetrieverValid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/geotiff";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.code = "GRID-geotiff";

    terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
    dataSet->active = true;
    dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorGeoTiff accessor(dataProviderPtr, dataSeriesPtr);

    auto mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock_, isRetrivable()).WillOnce(Return(false));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_);

    RaiiTsDataAccessorGeoTiff raiiDataRetriever("MOCK",makeMock);

    try
    {
      terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter);
    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch(...)
  {
    QFAIL("Unexpected exception not related to the method getGridSeries!");
  }

  return;

}

void TsDataAccessorGeoTiff::TestFailDataRetrieverInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/geotiff";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.code = "GRID-geotiff";

    terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
    dataSet->active = true;
    dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    QString errMsg = QObject::tr("Non retrievable DataRetriever.");
    terrama2::core::NotRetrivableException exceptionMock;
    exceptionMock << terrama2::ErrorDescription(errMsg);

    //accessing data
    terrama2::core::DataAccessorGeoTiff accessor(dataProviderPtr, dataSeriesPtr);

    auto mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock_, isRetrivable()).WillOnce(Return(true));
    EXPECT_CALL(*mock_, retrieveData(_,_)).WillOnce(testing::Throw(exceptionMock));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_);

    RaiiTsDataAccessorGeoTiff raiiDataRetriever("MOCK",makeMock);

    try
    {
      terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter);
      QFAIL("Exception expected!");
    }
    catch(terrama2::core::NotRetrivableException&)
    {

    }
  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}

void TsDataAccessorGeoTiff::TestOK()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/geotiff";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.code = "GRID-geotiff";

    terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
    dataSet->active = true;
    dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataAccessorGeoTiff accessor(dataProviderPtr, dataSeriesPtr);
    terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter);

    assert(gridSeries->gridMap().size() == 1);

  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}
