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
  \file terrama2/unittest/core/TsDataAccessorGDAL.cpp
  \brief Tests for Class DataAccessorGDAL
  \author Evandro Delatin
*/

// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/impl/DataAccessorGDAL.hpp>
#include <terrama2/core/data-access/GridSeries.hpp>
#include <terrama2/impl/DataAccessorGDAL.hpp>
#include <terrama2/Config.hpp>
#include <terrama2/core/Exception.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include "TsDataAccessorGDAL.hpp"
#include "MockDataRetriever.hpp"
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

//FIXME: using a real dir, can be improved using some mock access

void TsDataAccessorGDAL::TestFailAddNullDataAccessorGDAL()
{
  try
  {
    //accessing data
    terrama2::core::DataAccessorGDAL accessor(nullptr, nullptr);

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

void TsDataAccessorGDAL::TestFailDataProviderNull()
{
  try
  {
    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

    //accessing data
    terrama2::core::DataAccessorGDAL accessor(nullptr, dataSeriesPtr);

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

void TsDataAccessorGDAL::TestFailDataSeriesNull()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

    //accessing data
    terrama2::core::DataAccessorGDAL accessor(dataProviderPtr, nullptr);

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

void TsDataAccessorGDAL::TestFailDataSeriesSemanticsInvalid()
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
}

void TsDataAccessorGDAL::TestFailDataRetrieverInvalid()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/geotiff";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "MOCK";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.driver = "GRID-gdal";

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
    terrama2::core::DataAccessorGDAL accessor(dataProviderPtr, dataSeriesPtr);

    auto mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);

    EXPECT_CALL(*mock_, isRetrivable()).WillOnce(Return(true));
    EXPECT_CALL(*mock_, retrieveData(_,_,_,_,_,_)).WillOnce(testing::Throw(exceptionMock));

    auto makeMock = std::bind(MockDataRetriever::makeMockDataRetriever, std::placeholders::_1, mock_);

    DataRetrieverFactoryRaii raiiDataRetriever("MOCK",makeMock);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    try
    {
      terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter, remover);
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

void TsDataAccessorGDAL::TestOK()
{
  try
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/geotiff";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.driver = "GRID-gdal";

    terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
    dataSet->active = true;
    dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataAccessorGDAL accessor(dataProviderPtr, dataSeriesPtr);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter, remover);

    QVERIFY2(gridSeries->gridMap().size() == 1, "Wrong number of DataSet");
  }
  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}
