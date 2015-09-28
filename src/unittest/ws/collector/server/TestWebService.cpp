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
  \file terrama2/unittest/ws/server/TestWebService.cpp

  \brief Tests for the WebService class.

  \author Vinicius Campanha
*/

// STL
#include <memory>

// TerraMA2 Test
#include "TestWebService.hpp"

// TerraMA2
#include "soapWebService.h"
#include <terrama2/core/ApplicationController.hpp>

void TestWebService::init()
{
  clearDatabase();
}

void TestWebService::cleanup()
{
  clearDatabase();
}


void TestWebService::clearDatabase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "TRUNCATE TABLE terrama2.data_provider CASCADE";
  transactor->execute(query);

  transactor->commit();
}


void TestWebService::TestStatus()
{
  std::string answer;

  try
  {

    WebService test;

    test.ping(answer);

    std::cout << answer << std::endl;
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }
}


void TestWebService::TestAddDataProvider()
{
  try
  {

    DataProvider struct_dataprovider, struct_dataprovider_check;

    struct_dataprovider.id = 0;
    struct_dataprovider.name = "Data Provider";
    struct_dataprovider.kind = 1;
    struct_dataprovider.description = "Data Provider description";
    struct_dataprovider.status = 1;
    struct_dataprovider.uri = "C:/Dataprovider/path";

    struct_dataprovider_check = struct_dataprovider;

    WebService test;

    if (test.addDataProvider(struct_dataprovider) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    if(struct_dataprovider.id == 0)
    {
      QFAIL("After added, a Data Providetr MUST have a valid ID!");
    }

    QVERIFY2(struct_dataprovider.name == struct_dataprovider_check.name, "Name changed after add!");

    QVERIFY2(struct_dataprovider.kind == struct_dataprovider_check.kind, "Kind changed after add!");

    QVERIFY2(struct_dataprovider.description == struct_dataprovider_check.description, "Description changed after add!");

    QVERIFY2(struct_dataprovider.status == struct_dataprovider_check.status, "Status changed after add!");

    QVERIFY2(struct_dataprovider.uri == struct_dataprovider_check.uri, "URI changed after add!");

  }
  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
}

void TestWebService::TestAddNullDataProvider()
{
    try
    {
        DataProvider struct_dataprovider;

        WebService test;

        if (test.addDataProvider(struct_dataprovider) == SOAP_OK)
        {
          QFAIL("Should not add a null Data Provider!");
        }
    }
    catch(...)
    {
        QFAIL("Exception unexpected!");
    }

}

void TestWebService::TestAddDataProviderWithID()
{
    try
    {
        DataProvider struct_dataprovider, struct_dataprovider_check;

        struct_dataprovider.id = 1;
        struct_dataprovider.name = "Data Provider";
        struct_dataprovider.kind = 1;
        struct_dataprovider.description = "Data Provider description";
        struct_dataprovider.status = 1;
        struct_dataprovider.uri = "C:/Dataprovider/path";

        struct_dataprovider_check = struct_dataprovider;

        WebService test;

        if (test.addDataProvider(struct_dataprovider) == SOAP_OK)
        {
          QFAIL("Should not add a Data Provider with ID!");
        }
    }
    catch(...)
    {
        QFAIL("Exception unexpected!");
    }

}


void TestWebService::testRemoveDataProvider()
{
    try
    {
        DataProvider struct_dataprovider;

        struct_dataprovider.id = 0;
        struct_dataprovider.name = "Data Provider";
        struct_dataprovider.kind = 1;
        struct_dataprovider.description = "Data Provider description";
        struct_dataprovider.status = 1;
        struct_dataprovider.uri = "C:/Dataprovider/path";

        WebService test;

        if (test.addDataProvider(struct_dataprovider) != SOAP_OK)
        {
          QFAIL("Add a Data Provider failed!");
        }

        if(struct_dataprovider.id == 0)
        {
          QFAIL("After added, a Data Providetr MUST have a valid ID!");
        }

        if (test.removeDataProvider(struct_dataprovider.id, nullptr) != SOAP_OK)
        {
          QFAIL("Fail to remove a Data Provider!");
        }
    }
    catch(...)
    {
        QFAIL("Exception unexpected!");
    }
}

void TestWebService::testRemoveDataProviderInvalidId()
{

}


void TestWebService::testUpdateDataProvider()
{

}

void TestWebService::testUpdateDataProviderInvalidId()
{

}


void TestWebService::testFindDataProvider()
{

}


void TestWebService::TestAddDataSet()
{

}
