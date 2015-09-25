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

// TerraMA2 Test
#include "TestWebService.hpp"

// TerraMA2
#include "soapWebService.h"


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
      QFAIL("Should not be here");
    }

    if(struct_dataprovider.id == 0)
    {
      QFAIL("Should not be here");
    }

    if(struct_dataprovider.name != struct_dataprovider_check.name) QFAIL("Should not be here");

    if(struct_dataprovider.kind != struct_dataprovider_check.kind) QFAIL("Should not be here");

    if(struct_dataprovider.description != struct_dataprovider_check.description) QFAIL("Should not be here");

    if(struct_dataprovider.status != struct_dataprovider_check.status) QFAIL("Should not be here");

    if(struct_dataprovider.uri != struct_dataprovider_check.uri) QFAIL("Should not be here");

  }
  catch(...)
  {
    QFAIL("Should not be here");
  }
}

void TestWebService::TestAddNullDataProvider()
{

}

void TestWebService::TestAddDataProviderID()
{

}

void TestWebService::TestAddDataSet()
{

}
