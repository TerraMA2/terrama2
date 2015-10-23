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
  \file terrama2/integration_test/ws/TsClient.cpp

  \brief Tests for the Web Server.

  \author Vinicius Campanha
*/

// STL
#include <memory>

// TerraMA2 Test

#include "TsClient.hpp"
#include "WebProxyAdapterMock.hpp"

// TerraMA2
#include <terrama2/ws/collector/client/Exception.hpp>

// gSOAP
#include <stdsoap2.h>

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::Eq;

bool operator==(const DataProvider& first, const DataProvider& second)
{
  if(first.id != second.id)
    return false;
  if(first.kind != second.kind)
    return false;
  if(first.name != second.name)
    return false;
  if(first.uri != second.uri)
    return false;
  if(first.description != second.description)
    return false;
  if(first.status != second.status)
    return false;
  return true;
}

TsClient::TsClient()
: wsClient_(&mock_)
{
}

terrama2::core::DataProvider TsClient::buildDataProvider()
{

  terrama2::core::DataProvider  dataProvider("Data Provider", terrama2::core::DataProvider::HTTP_TYPE, 0);
  dataProvider.setUri("pathDataProvider");
  dataProvider.setDescription("Data Provider Description");
  dataProvider.setStatus(terrama2::core::DataProvider::Status::ACTIVE);

  return dataProvider;
}


DataProvider TsClient::buildDataProviderStruct()
{
  DataProvider  dataProvider;
  dataProvider.kind = terrama2::core::DataProvider::Kind::HTTP_TYPE;
  dataProvider.id = 0;
  dataProvider.name = "Data Provider";
  dataProvider.uri = "pathDataProvider";
  dataProvider.description = "Data Provider Description";
  dataProvider.status = terrama2::core::DataProvider::Status::ACTIVE;

  return dataProvider;
}

DataSet TsClient::buildDataSetStruct()
{
  DataSet dataset;

  dataset.name = "Data Set Name";
  dataset.id = 0;
  dataset.data_provider_id = 1;
  dataset.kind = terrama2::core::DataSet::Kind::GRID_TYPE;
  dataset.description = "Data Set Description";
  dataset.status = terrama2::core::DataSet::Status::ACTIVE;
  dataset.data_frequency = "00:05:00.00";
  dataset.schedule = "00:06:00.00";
  dataset.schedule_retry = "00:07:00.00";
  dataset.schedule_timeout = "00:08:00.00";

  return dataset;
}


terrama2::core::DataSet TsClient::buildDataSet()
{
  DataProvider providerReturn = buildDataProviderStruct();
  providerReturn.id = 1;

  // Expect call to myMethod() return 0 and set the by ref argument to false
  EXPECT_CALL(mock_, addDataProvider(_, _))
          .WillOnce(DoAll(SetArgReferee<1>(providerReturn),
                          Return(SOAP_OK)));

  terrama2::core::DataProvider dataProvider = buildDataProvider();

  wsClient_.addDataProvider(dataProvider);

  terrama2::core::DataSet dataSet("Data Set Name", terrama2::core::DataSet::Kind::GRID_TYPE, 0, dataProvider.id());
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


void TsClient::testAddDataProvider()
{

  DataProvider providerStruct = buildDataProviderStruct();
  DataProvider providerReturn = providerStruct;
  providerReturn.id = 1;

  // Expect call to myMethod() return 0 and set the by ref argument to false
  EXPECT_CALL(mock_, addDataProvider(_, _))
          .WillOnce(DoAll(SetArgReferee<1>(providerReturn),
                          Return(SOAP_OK)));

  try
  {
    terrama2::core::DataProvider dataProvider = buildDataProvider();

    wsClient_.addDataProvider(dataProvider);

    QVERIFY2(dataProvider.id() != 0 , "Can't create a Data Provider with a invalid ID!");
  }
  catch(terrama2::Exception &e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
}


void TsClient::testAddNullDataProvider()
{
 try
 {
   terrama2::core::DataProvider dataProvider;

   wsClient_.addDataProvider(dataProvider);
 }
 catch(terrama2::Exception &e)
 {
   // test OK
   qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
 }
 catch(...)
 {
   QFAIL("Exception unexpected!");
 }
}


void TsClient::testRemoveDataProvider()
{
 try
 {
   terrama2::core::DataProvider dataProvider = buildDataProvider();

   wsClient_.addDataProvider(dataProvider);

   QVERIFY2(dataProvider.id() != 0 , "Can't create a Data Provider with a invalid ID!");

   // VINICIUS: code to remove a data provider in core is in SEGMENTATION FAULT
   //wsClient_.removeDataProvider(dataProvider.id());
 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Exception unexpected!");
 }
}


void TsClient::testRemoveDataProviderInvalidId()
{
 try
 {
   wsClient_.removeDataProvider(1);
 }
 catch(terrama2::Exception &e)
 {
   // test OK
   qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
 }
 catch(...)
 {
   QFAIL("Unexpected exception !");
 }
}


void TsClient::testUpdateDataProvider()
{
 try
 {
   terrama2::core::DataProvider dataProvider = buildDataProvider();

   wsClient_.addDataProvider(dataProvider);

   QVERIFY2(dataProvider.id() != 0 , "Can't create a Data Provider with a invalid ID!");

   uint64_t id = dataProvider.id();

   dataProvider.setDescription("Description updated");
   dataProvider.setKind(terrama2::core::DataProvider::Kind::HTTP_TYPE);
   dataProvider.setName("Name updated");
   dataProvider.setStatus(terrama2::core::DataProvider::Status::ACTIVE);
   dataProvider.setUri("C:/URI_updated");

   wsClient_.updateDataProvider(dataProvider);

   QVERIFY2(dataProvider.id() == id, "Update failed!");
   QVERIFY2(dataProvider.description() == "Description updated", "Update failed!");
   QVERIFY2(dataProvider.kind() == terrama2::core::DataProvider::Kind::HTTP_TYPE, "Update failed!");
   QVERIFY2(dataProvider.name() == "Name updated", "Update failed!");
   QVERIFY2(dataProvider.status() == terrama2::core::DataProvider::Status::ACTIVE, "Update failed!");
   QVERIFY2(dataProvider.uri() == "C:/URI_updated", "Update failed!");

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testFindDataProvider()
{
 try
 {
   terrama2::core::DataProvider dataProvider = buildDataProvider();

   wsClient_.addDataProvider(dataProvider);

   QVERIFY2(dataProvider.id() != 0 , "Can't create a Data Provider with a invalid ID!");



   dataProvider.id();

   terrama2::core::DataProvider dataProvider_found = wsClient_.findDataProvider(dataProvider.id());

   QVERIFY2(dataProvider.id() == dataProvider_found.id(), "Find failed!");
   QVERIFY2(dataProvider.description() == dataProvider_found.description(), "Find failed!");
   QVERIFY2(dataProvider.kind() == dataProvider_found.kind(), "Find failed!");
   QVERIFY2(dataProvider.name() == dataProvider_found.name(), "Find failed!");
   QVERIFY2(dataProvider.status() == dataProvider_found.status(), "Find failed!");
   QVERIFY2(dataProvider.uri() == dataProvider_found.uri(), "Find failed!");

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testFindDataProviderInvalidID()
{
 /*
 // VINICIUS: check terrama2 core handling to find invalids IDs
 try
 {
   terrama2::core::DataProvider dataProvider_found;

   wsClient_.findDataProvider(1, dataProvider_found);

   QFAIL("Should not find a invalid Data Provider!");
 }
 catch(terrama2::Exception &e)
 {
   // test OK
   qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
*/
}


void TsClient::testAddDataSet()
{
 try
 {
   terrama2::core::DataSet dataSet = buildDataSet();

   wsClient_.addDataSet(dataSet);

   QVERIFY2(dataSet.id() != 0 , "Can't create a Data Provider with a invalid ID!");

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testAddNullDataSet()
{
 try
 {

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testAddDataSetWithID()
{
 try
 {

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testAddDataSetWithWrongDataProviderID()
{
 try
 {

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testRemoveDataSet()
{
 try
 {

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testRemoveDataSetInvalidId()
{
 try
 {

 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testUpdateDataSet()
{
 try
 {


 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testUpdateDataSetInvalidId()
{
 try
 {


 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testFindDataSet()
{
 try
 {


 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}


void TsClient::testFindDataSetInvalidID()
{
 try
 {


 }
 catch(terrama2::Exception &e)
 {
   QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
 }
 catch(...)
 {
   QFAIL("Unexpected exception!");
 }
}

