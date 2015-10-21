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

terrama2::core::DataProvider TestCore::buildDataProvider()
{

  terrama2::core::DataProvider  dataProvider(0, (terrama2::core::DataProvider::Kind)1);

  dataProvider.setName("Data Provider");
  dataProvider.setUri("pathDataProvider");
  dataProvider.setDescription("Data Provider Description");
  dataProvider.setStatus((terrama2::core::DataProvider::Status)1);

  return dataProvider;
}

terrama2::core::DataSet TestCore::buildDataSet()
{
  terrama2::core::DataProvider dataProvider = buildDataProvider();

  terrama2::core::DataManager::getInstance().add(dataProvider);

  terrama2::core::DataSet dataSet("Data Set Name", (terrama2::core::DataSet::Kind)1, 9, dataProvider.id());

  dataSet.setDescription("Data Set Description");
  dataSet.setStatus((terrama2::core::DataSet::Status)1);

  boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string("00:05:00.00"));
  boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string("00:06:00.00"));
  boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string("00:07:00.00"));
  boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string("00:08:00.00"));

  dataSet.setDataFrequency(te::dt::TimeDuration(dataFrequency));
  dataSet.setSchedule(te::dt::TimeDuration(schedule));
  dataSet.setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
  dataSet.setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

  return dataSet;
}

void TestCore::TestConvertDataProviderToDataProviderStruct()
{
  terrama2::core::DataProvider dataProvider = buildDataProvider();

  DataProvider struct_dataProvider = terrama2::ws::collector::core::DataProvider2Struct< DataProvider >(dataProvider);

  QVERIFY2(struct_dataProvider.id == dataProvider.id(), "Id changed after conversion!");
  QVERIFY2(struct_dataProvider.name == dataProvider.name(), "Name changed after conversion!");
  QVERIFY2(struct_dataProvider.kind == dataProvider.kind(), "Kind changed after conversion!");
  QVERIFY2(struct_dataProvider.description == dataProvider.description(), "Description changed after conversion!");
  QVERIFY2(struct_dataProvider.status == dataProvider.status(), "Status changed after conversion!");
  QVERIFY2(struct_dataProvider.uri == dataProvider.uri(), "URI changed after conversion!");
}


void TestCore::TestWrongConvertDataProviderStructToDataProvider()
{
  DataProvider struct_dataProvider;

  struct_dataProvider.id = 7;
  struct_dataProvider.name = "Data Provider";
  struct_dataProvider.kind = 1;
  struct_dataProvider.description = "Data Provider description";
  struct_dataProvider.status = 1;
  struct_dataProvider.uri = "C:/Dataprovider/path";

  terrama2::core::DataProvider dataProvider = terrama2::ws::collector::core::Struct2DataProvider< DataProvider >(struct_dataProvider);

  QVERIFY2(struct_dataProvider.id == dataProvider.id(), "Id changed after conversion!");
  QVERIFY2(struct_dataProvider.name == dataProvider.name(), "Name changed after conversion!");
  QVERIFY2(struct_dataProvider.kind == dataProvider.kind(), "Kind changed after conversion!");
  QVERIFY2(struct_dataProvider.description == dataProvider.description(), "Description changed after conversion!");
  QVERIFY2(struct_dataProvider.status == dataProvider.status(), "Status changed after conversion!");
  QVERIFY2(struct_dataProvider.uri == dataProvider.uri(), "URI changed after conversion!");

}


void TestCore::TestConvertDataSetToDataSetStruct()
{
  terrama2::core::DataSet dataSet = buildDataSet();

  DataSet struct_dataSet = terrama2::ws::collector::core::DataSet2Struct< DataSet >(dataSet);

  QVERIFY2(dataSet.id() == struct_dataSet.id, "ID changed after conversion!");
  QVERIFY2(dataSet.provider() == struct_dataSet.data_provider_id, "Data Provider changed after conversion!");
  QVERIFY2(dataSet.name() == struct_dataSet.name, "Name changed after conversion!");
  QVERIFY2(dataSet.kind() == struct_dataSet.kind, "Kind changed after conversion!");
  QVERIFY2(dataSet.status() == struct_dataSet.status, "Status changed after conversion!");
  QVERIFY2(dataSet.description() == struct_dataSet.description, "Description changed after conversion!");
  QVERIFY2(dataSet.dataFrequency().toString() == struct_dataSet.data_frequency, "Data Frequency changed after conversion!");
  QVERIFY2(dataSet.schedule().toString() == struct_dataSet.schedule, "Schedule changed after conversion!");
  QVERIFY2(dataSet.scheduleRetry().toString() == struct_dataSet.schedule_retry, "Schedule retry changed after conversion!");
  QVERIFY2(dataSet.scheduleTimeout().toString() == struct_dataSet.schedule_timeout, "Schedule Timeout changed after conversion!");

}


void TestCore::TestConvertDataSetStructToDataSet()
{
  terrama2::core::DataProvider dataProvider = buildDataProvider();

  terrama2::core::DataManager::getInstance().add(dataProvider);

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
  struct_dataSet.data_provider_id = dataProvider.id();

  terrama2::core::DataSet dataSet = terrama2::ws::collector::core::Struct2DataSet< DataSet >(struct_dataSet);

  QVERIFY2(dataSet.id() == struct_dataSet.id, "ID changed after conversion!");
  QVERIFY2(dataSet.provider() == struct_dataSet.data_provider_id, "Data Provider changed after conversion!");
  QVERIFY2(dataSet.name() == struct_dataSet.name, "Name changed after conversion!");
  QVERIFY2(dataSet.kind() == struct_dataSet.kind, "Kind changed after conversion!");
  QVERIFY2(dataSet.status() == struct_dataSet.status, "Status changed after conversion!");
  QVERIFY2(dataSet.description() == struct_dataSet.description, "Description changed after conversion!");
  QVERIFY2(dataSet.dataFrequency().toString() == struct_dataSet.data_frequency, "Data Frequency changed after conversion!");
  QVERIFY2(dataSet.schedule().toString() == struct_dataSet.schedule, "Schedule changed after conversion!");
  QVERIFY2(dataSet.scheduleRetry().toString() == struct_dataSet.schedule_retry, "Schedule retry changed after conversion!");
  QVERIFY2(dataSet.scheduleTimeout().toString() == struct_dataSet.schedule_timeout, "Schedule Timeout changed after conversion!");
}
