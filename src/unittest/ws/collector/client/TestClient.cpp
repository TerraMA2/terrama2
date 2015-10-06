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

terrama2::core::DataProviderPtr TestClient::buildDataProviderPtr()
{

  terrama2::core::DataProviderPtr  dataProviderPtr(new terrama2::core::DataProvider("Data Provider", (terrama2::core::DataProvider::Kind)1));

  dataProviderPtr->setUri("C:/DataProvider/");
  dataProviderPtr->setDescription("Data Provider Description");
  dataProviderPtr->setStatus((terrama2::core::DataProvider::Status)1);

  return dataProviderPtr;
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
    auto dataProviderPtr = buildDataProviderPtr();

    wsClient_->addDataProvider(dataProviderPtr);

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
    auto dataProviderPtr = buildDataProviderPtr();
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


void TestClient::TestAddDataProviderWithID()
{
  try
  {
    auto dataProviderPtr = buildDataProviderPtr();
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


void TestClient::testRemoveDataProvider()
{
  try
  {
    auto dataProviderPtr = buildDataProviderPtr();
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
    auto dataProviderPtr = buildDataProviderPtr();
  }
  catch(terrama2::Exception &e)
  {
QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
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
    auto dataProviderPtr = buildDataProviderPtr();
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


void TestClient::testUpdateDataProviderInvalidId()
{
  try
  {
    auto dataProviderPtr = buildDataProviderPtr();
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
    auto dataProviderPtr = buildDataProviderPtr();
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


void TestClient::TestAddDataSet()
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
