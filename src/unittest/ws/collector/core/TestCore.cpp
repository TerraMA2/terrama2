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
  \file terrama2/unittest/ws/server/TestClient.cpp

  \brief Tests for the WebProxy class.

  \author Vinicius Campanha
*/

// STL
#include <memory>

// Boost
#include "boost/date_time/posix_time/posix_time.hpp"

// TerraMA2
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/ws/collector/core/Utils.hpp>
#include "soapWebService.h"

// TerraMA2 Test
#include "TestCore.hpp"


void TestCore::init()
{
  clearDatabase();
}

void TestCore::cleanup()
{
  clearDatabase();
}

void TestCore::clearDatabase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "TRUNCATE TABLE terrama2.data_provider CASCADE";
  transactor->execute(query);

  transactor->commit();
}

terrama2::core::DataProviderPtr TestCore::buildDataProviderPtr()
{

  terrama2::core::DataProviderPtr  dataProviderPtr(new terrama2::core::DataProvider("Data Provider", (terrama2::core::DataProvider::Kind)1));

  dataProviderPtr->setUri("C:/DataProvider/");
  dataProviderPtr->setDescription("Data Provider Description");
  dataProviderPtr->setStatus((terrama2::core::DataProvider::Status)1);

  return dataProviderPtr;
}

terrama2::core::DataSetPtr TestCore::buildDataSetPtr()
{
  terrama2::core::DataProviderPtr dataProviderPtr = buildDataProviderPtr();

  terrama2::core::DataManager::getInstance().add(dataProviderPtr);

  terrama2::core::DataSetPtr dataSetPtr(new terrama2::core::DataSet(dataProviderPtr, "DataSet name", (terrama2::core::DataSet::Kind)1, 9));

  dataSetPtr->setDescription("Data Set Description");
  dataSetPtr->setStatus((terrama2::core::DataSet::Status)1);

  boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string("00:05:00.00"));
  boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string("00:06:00.00"));
  boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string("00:07:00.00"));
  boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string("00:08:00.00"));

  dataSetPtr->setDataFrequency(te::dt::TimeDuration(dataFrequency));
  dataSetPtr->setSchedule(te::dt::TimeDuration(schedule));
  dataSetPtr->setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
  dataSetPtr->setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

  return dataSetPtr;
}

void TestCore::TestConvertDataProviderPtrToDataProviderStruct()
{
  terrama2::core::DataProviderPtr dataProviderPtr = buildDataProviderPtr();

  DataProvider struct_dataProvider = terrama2::ws::collector::core::DataProvider2Struct< DataProvider >(dataProviderPtr);

  QVERIFY2(struct_dataProvider.id == dataProviderPtr->id(), "Id changed after conversion!");
  QVERIFY2(struct_dataProvider.name == dataProviderPtr->name(), "Name changed after conversion!");
  QVERIFY2(struct_dataProvider.kind == dataProviderPtr->kind(), "Kind changed after conversion!");
  QVERIFY2(struct_dataProvider.description == dataProviderPtr->description(), "Description changed after conversion!");
  QVERIFY2(struct_dataProvider.status == dataProviderPtr->status(), "Status changed after conversion!");
  QVERIFY2(struct_dataProvider.uri == dataProviderPtr->uri(), "URI changed after conversion!");
}


void TestCore::TestWrongConvertDataProviderStructToDataProviderPtr()
{
  DataProvider struct_dataProvider;

  struct_dataProvider.id = 7;
  struct_dataProvider.name = "Data Provider";
  struct_dataProvider.kind = 1;
  struct_dataProvider.description = "Data Provider description";
  struct_dataProvider.status = 1;
  struct_dataProvider.uri = "C:/Dataprovider/path";

  terrama2::core::DataProviderPtr dataProviderPtr = terrama2::ws::collector::core::Struct2DataProvider< DataProvider >(struct_dataProvider);

  QVERIFY2(struct_dataProvider.id == dataProviderPtr->id(), "Id changed after conversion!");
  QVERIFY2(struct_dataProvider.name == dataProviderPtr->name(), "Name changed after conversion!");
  QVERIFY2(struct_dataProvider.kind == dataProviderPtr->kind(), "Kind changed after conversion!");
  QVERIFY2(struct_dataProvider.description == dataProviderPtr->description(), "Description changed after conversion!");
  QVERIFY2(struct_dataProvider.status == dataProviderPtr->status(), "Status changed after conversion!");
  QVERIFY2(struct_dataProvider.uri == dataProviderPtr->uri(), "URI changed after conversion!");

}


void TestCore::TestConvertDataSetPtrToDataSetStruct()
{
  terrama2::core::DataSetPtr dataSetPtr = buildDataSetPtr();

  DataSet struct_dataSet = terrama2::ws::collector::core::DataSet2Struct< DataSet >(dataSetPtr);

  QVERIFY2(dataSetPtr->id() == struct_dataSet.id, "ID changed after conversion!");
  QVERIFY2(dataSetPtr->dataProvider()->id() == struct_dataSet.data_provider_id, "Data Provider changed after conversion!");
  QVERIFY2(dataSetPtr->name() == struct_dataSet.name, "Name changed after conversion!");
  QVERIFY2(dataSetPtr->kind() == struct_dataSet.kind, "Kind changed after conversion!");
  QVERIFY2(dataSetPtr->status() == struct_dataSet.status, "Status changed after conversion!");
  QVERIFY2(dataSetPtr->description() == struct_dataSet.description, "Description changed after conversion!");
  QVERIFY2(dataSetPtr->dataFrequency().toString() == struct_dataSet.data_frequency, "Data Frequency changed after conversion!");
  QVERIFY2(dataSetPtr->schedule().toString() == struct_dataSet.schedule, "Schedule changed after conversion!");
  QVERIFY2(dataSetPtr->scheduleRetry().toString() == struct_dataSet.schedule_retry, "Schedule retry changed after conversion!");
  QVERIFY2(dataSetPtr->scheduleTimeout().toString() == struct_dataSet.schedule_timeout, "Schedule Timeout changed after conversion!");

}


void TestCore::TestConvertDataSetStructToDataSetPtr()
{
  terrama2::core::DataProviderPtr dataProviderPtr = buildDataProviderPtr();

  terrama2::core::DataManager::getInstance().add(dataProviderPtr);

  DataSet struct_dataSet;

  struct_dataSet.id = 0;
  struct_dataSet.name = "Data Set";
  struct_dataSet.kind = 1;
  struct_dataSet.status = 1;
  struct_dataSet.description = "Data Set description";
  struct_dataSet.data_frequency = "00:01:00";
  struct_dataSet.schedule = "00:02:00";
  struct_dataSet.schedule_retry = "00:03:00";
  struct_dataSet.schedule_timeout = "00:04:00";
  struct_dataSet.data_provider_id = dataProviderPtr->id();

  terrama2::core::DataSetPtr dataSetPtr = terrama2::ws::collector::core::Struct2DataSet< DataSet >(struct_dataSet);

  QVERIFY2(dataSetPtr->id() == struct_dataSet.id, "ID changed after conversion!");
  QVERIFY2(dataSetPtr->dataProvider()->id() == struct_dataSet.data_provider_id, "Data Provider changed after conversion!");
  QVERIFY2(dataSetPtr->name() == struct_dataSet.name, "Name changed after conversion!");
  QVERIFY2(dataSetPtr->kind() == struct_dataSet.kind, "Kind changed after conversion!");
  QVERIFY2(dataSetPtr->status() == struct_dataSet.status, "Status changed after conversion!");
  QVERIFY2(dataSetPtr->description() == struct_dataSet.description, "Description changed after conversion!");
  QVERIFY2(dataSetPtr->dataFrequency().toString() == struct_dataSet.data_frequency, "Data Frequency changed after conversion!");
  QVERIFY2(dataSetPtr->schedule().toString() == struct_dataSet.schedule, "Schedule changed after conversion!");
  QVERIFY2(dataSetPtr->scheduleRetry().toString() == struct_dataSet.schedule_retry, "Schedule retry changed after conversion!");
  QVERIFY2(dataSetPtr->scheduleTimeout().toString() == struct_dataSet.schedule_timeout, "Schedule Timeout changed after conversion!");
}
