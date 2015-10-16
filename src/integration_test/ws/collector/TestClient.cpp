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
  \file terrama2/integration_test/ws/TestClient.cpp

  \brief Tests for the Web Server.

  \author Vinicius Campanha
*/

// STL
#include <memory>

// TerraMA2 Test
#include "TestClient.hpp"

// TerraMA2
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/ws/collector/client/Exception.hpp>

void TestClient::init()
{
  clearDatabase();

  wsClient_ = new terrama2::ws::collector::Client("http://localhost:1989");
}

void TestClient::cleanup()
{
  clearDatabase();

  delete wsClient_;
}


void TestClient::clearDatabase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "TRUNCATE TABLE terrama2.data_provider CASCADE";
  transactor->execute(query);

  transactor->commit();
}

terrama2::core::DataProvider TestClient::buildDataProvider()
{

  terrama2::core::DataProvider  dataProvider(0, (terrama2::core::DataProvider::Kind)1);

  dataProvider.setName("Data Provider");
  dataProvider.setUri("C:/DataProvider/");
  dataProvider.setDescription("Data Provider Description");
  dataProvider.setStatus((terrama2::core::DataProvider::Status)1);

  return dataProvider;
}

terrama2::core::DataSet TestClient::buildDataSet()
{
  terrama2::core::DataProvider dataProvider = buildDataProvider();

  wsClient_->addDataProvider(dataProvider);

  terrama2::core::DataSet dataSet((terrama2::core::DataSet::Kind)1, 0, dataProvider.id());

  dataSet.setName("Data Set Name");
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

void TestClient::TestStatus()
{
  std::string answer;

  try
  {
    wsClient_->ping(answer);

    std::cout << answer << std::endl;
  }
  catch(terrama2::Exception &e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }
}


void TestClient::TestWrongConection()
{
  std::string answer;
  try
  {
    delete wsClient_;
    wsClient_ = new terrama2::ws::collector::Client("http://wrongaddress:00");

    wsClient_->ping(answer);

    QFAIL("Can't connect to a wrong address!");
  }
  catch(terrama2::Exception &e)
  {
    // test ok
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
}


void TestClient::TestAddDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProviderPtr = buildDataProvider();

    wsClient_->addDataProvider(dataProviderPtr);

    QVERIFY2(dataProviderPtr.id() != 0 , "Can't create a Data Provider with a invalid ID!");

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


void TestClient::TestAddNullDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProviderPtr;

    wsClient_->addDataProvider(dataProviderPtr);
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


void TestClient::testRemoveDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProviderPtr = buildDataProvider();

    wsClient_->addDataProvider(dataProviderPtr);

    QVERIFY2(dataProviderPtr.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    // VINICIUS: code to remove a data provider in core is in SEGMENTATION FAULT
    //wsClient_->removeDataProvider(dataProviderPtr.id());
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


void TestClient::testRemoveDataProviderInvalidId()
{
  try
  {
    wsClient_->removeDataProvider(1);
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


void TestClient::testUpdateDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProviderPtr = buildDataProvider();

    wsClient_->addDataProvider(dataProviderPtr);

    QVERIFY2(dataProviderPtr.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    uint64_t id = dataProviderPtr.id();

    dataProviderPtr.setDescription("Description updated");
    dataProviderPtr.setKind((terrama2::core::DataProvider::Kind)2);
    dataProviderPtr.setName("Name updated");
    dataProviderPtr.setStatus((terrama2::core::DataProvider::Status)2);
    dataProviderPtr.setUri("C:/URI_updated");

    wsClient_->updateDataProvider(dataProviderPtr);

    QVERIFY2(dataProviderPtr.id() == id, "Update failed!");
    QVERIFY2(dataProviderPtr.description() == "Description updated", "Update failed!");
    QVERIFY2(dataProviderPtr.kind() == 2, "Update failed!");
    QVERIFY2(dataProviderPtr.name() == "Name updated", "Update failed!");
    QVERIFY2(dataProviderPtr.status() == 2, "Update failed!");
    QVERIFY2(dataProviderPtr.uri() == "C:/URI_updated", "Update failed!");

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


void TestClient::testFindDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProviderPtr = buildDataProvider();

    wsClient_->addDataProvider(dataProviderPtr);

    QVERIFY2(dataProviderPtr.id() != 0 , "Can't create a Data Provider with a invalid ID!");



    dataProviderPtr.id();

    terrama2::core::DataProvider dataProviderPtr_found = wsClient_->findDataProvider(dataProviderPtr.id());

    QVERIFY2(dataProviderPtr.id() == dataProviderPtr_found.id(), "Find failed!");
    QVERIFY2(dataProviderPtr.description() == dataProviderPtr_found.description(), "Find failed!");
    QVERIFY2(dataProviderPtr.kind() == dataProviderPtr_found.kind(), "Find failed!");
    QVERIFY2(dataProviderPtr.name() == dataProviderPtr_found.name(), "Find failed!");
    QVERIFY2(dataProviderPtr.status() == dataProviderPtr_found.status(), "Find failed!");
    QVERIFY2(dataProviderPtr.uri() == dataProviderPtr_found.uri(), "Find failed!");

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


void TestClient::testFindDataProviderInvalidID()
{
  /*
  // VINICIUS: check terrama2 core handling to find invalids IDs
  try
  {
    terrama2::core::DataProvider dataProviderPtr_found;

    wsClient_->findDataProvider(1, dataProviderPtr_found);

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


void TestClient::TestAddDataSet()
{
  try
  {
    terrama2::core::DataSet dataSetPtr = buildDataSet();

    wsClient_->addDataSet(dataSetPtr);

    QVERIFY2(dataSetPtr.id() != 0 , "Can't create a Data Provider with a invalid ID!");

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


void TestClient::TestAddNullDataSet()
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


void TestClient::TestAddDataSetWithID()
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


void TestClient::TestAddDataSetWithWrongDataProviderID()
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


void TestClient::testRemoveDataSet()
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


void TestClient::testRemoveDataSetInvalidId()
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


void TestClient::testUpdateDataSet()
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


void TestClient::testUpdateDataSetInvalidId()
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


void TestClient::testFindDataSet()
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


void TestClient::testFindDataSetInvalidID()
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
