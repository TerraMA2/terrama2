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
  \file terrama2/ws/integration/TsClient.cpp

  \brief Tests for the Web Server.

  \author Vinicius Campanha
*/

// STL
#include <memory>

// TerraMA2 Test
#include "TsClient.hpp"
#include "Utils.hpp"

// TerraMA2
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/ws/collector/client/Exception.hpp>

void TsClient::initTestCase()
{
  // VINICIUS: not working, depends on TerraLib5
//  CreateDatabase();
}


void TsClient::cleanupTestCase()
{
  // VINICIUS: not working, depends on TerraLib5
//  DropDatabase();
}


void TsClient::init()
{
  wsClient_ = new terrama2::ws::collector::Client("http://localhost:1989");

  clearDatabase();
}


void TsClient::cleanup()
{
  clearDatabase();

  delete wsClient_;
}


void TsClient::clearDatabase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "TRUNCATE TABLE terrama2.data_provider CASCADE";
  transactor->execute(query);

  transactor->commit();
}


terrama2::core::DataProvider TsClient::buildDataProvider()
{

  terrama2::core::DataProvider  dataProvider("Data Provider", terrama2::core::DataProvider::Kind::FILE_TYPE, 0);

  dataProvider.setUri("pathDataProvider");
  dataProvider.setDescription("Data Provider Description");
  dataProvider.setStatus(terrama2::core::DataProvider::Status::ACTIVE);

  return dataProvider;
}


terrama2::core::DataSet TsClient::buildDataSet()
{
  terrama2::core::DataProvider dataProvider = buildDataProvider();

  wsClient_->addDataProvider(dataProvider);

  terrama2::core::DataSet dataSet("Data Set Name", terrama2::core::DataSet::Kind::GRID_TYPE, 0, dataProvider.id());

  dataSet.setDescription("Data Set Description");
  dataSet.setStatus(terrama2::core::DataSet::Status::ACTIVE);

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


void TsClient::TestStatus()
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


void TsClient::TestWrongConection()
{
  std::string answer;
  try
  {
    terrama2::ws::collector::Client wsClient("http://wrongaddress:00");

    wsClient.ping(answer);

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


void TsClient::TestAddDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProvider = buildDataProvider();

    wsClient_->addDataProvider(dataProvider);

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


void TsClient::TestAddNullDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProvider;

    wsClient_->addDataProvider(dataProvider);

    QFAIL("Should not add a null Data Provider");
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

    wsClient_->addDataProvider(dataProvider);

    QVERIFY2(dataProvider.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    wsClient_->removeDataProvider(dataProvider.id());
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


void TsClient::testUpdateDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProvider = buildDataProvider();

    wsClient_->addDataProvider(dataProvider);

    QVERIFY2(dataProvider.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    dataProvider.setDescription("Description updated");
    dataProvider.setKind(terrama2::core::DataProvider::Kind::FTP_TYPE);
    dataProvider.setName("Name updated");
    dataProvider.setStatus(terrama2::core::DataProvider::Status::INACTIVE);
    dataProvider.setUri("URI_updated");

    terrama2::core::DataProvider dataProvider_updated(dataProvider);

    wsClient_->updateDataProvider(dataProvider_updated);

    QCOMPARE(dataProvider.id(), dataProvider_updated.id());
    QCOMPARE(dataProvider.description().c_str(), dataProvider_updated.description().c_str());
    QCOMPARE(dataProvider.kind(), dataProvider_updated.kind());
    QCOMPARE(dataProvider.name().c_str(), dataProvider_updated.name().c_str());
    QCOMPARE(dataProvider.status(), dataProvider_updated.status());
    QCOMPARE(dataProvider.uri().c_str(), dataProvider_updated.uri().c_str());

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


void TsClient::testUpdateDataProviderWrongID()
{
  try
  {
    terrama2::core::DataProvider dataProvider = buildDataProvider();

    dataProvider.setId(1);

    wsClient_->updateDataProvider(dataProvider);

    QFAIL("Should not update a invalid Data Set!");
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
}


void TsClient::testFindDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProvider = buildDataProvider();

    wsClient_->addDataProvider(dataProvider);

    QVERIFY2(dataProvider.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    terrama2::core::DataProvider dataProvider_found = wsClient_->findDataProvider(dataProvider.id());

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
  try
  {
    terrama2::core::DataProvider dataProvider_found(wsClient_->findDataProvider(1));

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

}


void TsClient::testListDataProvider()
{
  try
  {
    terrama2::core::DataProvider dataProvider = buildDataProvider();

    wsClient_->addDataProvider(dataProvider);

    dataProvider.setId(0);
    dataProvider.setName("Data Provider2");

    wsClient_->addDataProvider(dataProvider);

    std::vector< terrama2::core::DataProvider > list_dataProvider;

    wsClient_->listDataProvider(list_dataProvider);

    QCOMPARE(list_dataProvider.size(), 2uL);
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

void TsClient::TestAddDataSet()
{
  try
  {
    terrama2::core::DataSet dataSet = buildDataSet();

    wsClient_->addDataSet(dataSet);

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


void TsClient::TestAddNullDataSet()
{
  try
  {
    terrama2::core::DataSet dataSet;

    wsClient_->addDataSet(dataSet);

    QFAIL("Should not add a null Data Provider");
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
}


void TsClient::TestAddDataSetWithID()
{
  try
  {
    terrama2::core::DataSet dataSet = buildDataSet();

    dataSet.setId(1);

    wsClient_->addDataSet(dataSet);

    QFAIL("Should not add a Data Set with an ID!");
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
}


void TsClient::TestAddDataSetWithWrongDataProviderID()
{
  try
  {
    terrama2::core::DataSet dataSet = buildDataSet();

    dataSet.setProvider(1);

    wsClient_->addDataSet(dataSet);

    QFAIL("Should not add a Data Set with a wrong Data Provider ID!");
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
}


void TsClient::testRemoveDataSet()
{
  try
  {
    terrama2::core::DataSet dataSet = buildDataSet();

    wsClient_->addDataSet(dataSet);

    QVERIFY2(dataSet.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    wsClient_->removeDataSet(dataSet.id());
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
    wsClient_->removeDataSet(1);

    QFAIL("Should not remove a Data Set with a invalid ID!");
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
}


void TsClient::testUpdateDataSet()
{
  try
  {
    terrama2::core::DataSet dataSet = buildDataSet();

    wsClient_->addDataSet(dataSet);

    QVERIFY2(dataSet.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    dataSet.setName("Data Set Updated");
    dataSet.setKind(terrama2::core::DataSet::Kind::UNKNOWN_TYPE);
    dataSet.setDescription("Data Set Description Updated");
    dataSet.setStatus(terrama2::core::DataSet::Status::ACTIVE);

    boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string("00:09:00.00"));
    boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string("00:10:00.00"));
    boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string("00:11:00.00"));
    boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string("00:12:00.00"));

    dataSet.setDataFrequency(te::dt::TimeDuration(dataFrequency));
    dataSet.setSchedule(te::dt::TimeDuration(schedule));
    dataSet.setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
    dataSet.setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

    terrama2::core::DataSet dataSet_updated(dataSet);

    wsClient_->updateDataSet(dataSet_updated);

    QCOMPARE(dataSet.id(), dataSet_updated.id());
    QCOMPARE(dataSet.name().c_str(), dataSet_updated.name().c_str());
    QCOMPARE(dataSet.kind(), dataSet_updated.kind());
    QCOMPARE(dataSet.description().c_str(), dataSet_updated.description().c_str());
    QCOMPARE(dataSet.status(), dataSet_updated.status());
    QCOMPARE(dataSet.provider(), dataSet_updated.provider());
    QCOMPARE(dataSet.dataFrequency(), dataSet_updated.dataFrequency());
    QCOMPARE(dataSet.schedule(), dataSet_updated.schedule());
    QCOMPARE(dataSet.scheduleRetry(), dataSet_updated.scheduleRetry());
    QCOMPARE(dataSet.scheduleTimeout(), dataSet_updated.scheduleTimeout());
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
    terrama2::core::DataSet dataSet = buildDataSet();

    wsClient_->addDataSet(dataSet);

    QVERIFY2(dataSet.id() != 0 , "Can't create a Data Provider with a invalid ID!");

    terrama2::core::DataSet dataSet_found(wsClient_->findDataSet(dataSet.id()));

    QCOMPARE(dataSet.id(), dataSet_found.id());
    QCOMPARE(dataSet.name().c_str(), dataSet_found.name().c_str());
    QCOMPARE(dataSet.kind(), dataSet_found.kind());
    QCOMPARE(dataSet.description().c_str(), dataSet_found.description().c_str());
    QCOMPARE((int)dataSet.status(), (int)dataSet_found.status());
    QCOMPARE(dataSet.provider(), dataSet_found.provider());
    QCOMPARE(dataSet.dataFrequency(), dataSet_found.dataFrequency());
    QCOMPARE(dataSet.schedule(), dataSet_found.schedule());
    QCOMPARE(dataSet.scheduleRetry(), dataSet_found.scheduleRetry());
    QCOMPARE(dataSet.scheduleTimeout(), dataSet_found.scheduleTimeout());

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
    wsClient_->findDataSet(1);

    QFAIL("Should not find a Data Set with a invalid ID!");
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
}


void TsClient::testListDataSet()
{
  try
  {
    terrama2::core::DataSet dataSet = buildDataSet();

    wsClient_->addDataSet(dataSet);

    dataSet.setId(0);
    dataSet.setName("Data Set2");

    wsClient_->addDataSet(dataSet);

    std::vector< terrama2::core::DataSet > list_dataSet;

    wsClient_->listDataSet(list_dataSet);

    QCOMPARE(list_dataSet.size(), 2uL);
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


void TsClient::testShutdownWebService()
{
  try
  {
    wsClient_->shutdown();
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
