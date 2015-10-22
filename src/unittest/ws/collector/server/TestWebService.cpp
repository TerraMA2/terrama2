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
#include <terrama2/core/Exception.hpp>
#include "soapWebService.h"
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>

void TestWebService::init()
{
  clearDatabase();
  terrama2::core::DataManager::getInstance().load();
}

void TestWebService::cleanup()
{
  clearDatabase();
  terrama2::core::DataManager::getInstance().unload();
}


void TestWebService::clearDatabase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "TRUNCATE TABLE terrama2.data_provider CASCADE";
  transactor->execute(query);

  transactor->commit();

  query = "TRUNCATE TABLE terrama2.dataset CASCADE";
  transactor->execute(query);

  transactor->commit();
}


void TestWebService::TestStatus()
{
  std::string answer;

  try
  {

    WebService webService;

    webService.ping(answer);

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


void TestWebService::TestAddDataProvider()
{
  try
  {

    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    if(struct_dataProviderResult.id == 0)
    {
      QFAIL("After added, a Data Providetr MUST have a valid ID!");
    }

    QVERIFY2(struct_dataProvider.name == struct_dataProviderResult.name, "Name changed after add!");
    QVERIFY2(struct_dataProvider.kind == struct_dataProviderResult.kind, "Kind changed after add!");
    QVERIFY2(struct_dataProvider.description == struct_dataProviderResult.description, "Description changed after add!");
    QVERIFY2(struct_dataProvider.status == struct_dataProviderResult.status, "Status changed after add!");
    QVERIFY2(struct_dataProvider.uri == struct_dataProviderResult.uri, "URI changed after add!");

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

void TestWebService::TestAddNullDataProvider()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    WebService webService;

    struct_dataProvider.id = 0;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) == SOAP_OK)
    {
      QFAIL("Should not add a null Data Provider!");
    }
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

void TestWebService::TestAddDataProviderWithID()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 1;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) == SOAP_OK)
    {
      QFAIL("Should not add a Data Provider with ID!");
    }
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


void TestWebService::testRemoveDataProvider()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    if(struct_dataProviderResult.id == 0)
    {
      QFAIL("After added, a Data Providetr MUST have a valid ID!");
    }

    if (webService.removeDataProvider(struct_dataProviderResult.id) != SOAP_STOP)
    {
      QFAIL("Fail to remove a Data Provider!");
    }
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

void TestWebService::testRemoveDataProviderInvalidId()
{
  try
  {
    WebService webService;

    if(webService.removeDataProvider(1) == SOAP_STOP)
    {
      QFAIL("Should not remove a invalid Data Provider ID");
    }
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


void TestWebService::testUpdateDataProvider()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    if(struct_dataProviderResult.id == 0)
    {
      QFAIL("After added, a Data Providetr MUST have a valid ID!");
    }

    DataProvider struct_dataproviderOLD = struct_dataProviderResult;

    struct_dataProvider.id = struct_dataProviderResult.id;
    struct_dataProvider.name = "Data Provider Updated";
    struct_dataProvider.kind = 2;
    struct_dataProvider.description = "Data Provider description Updated";
    struct_dataProvider.status = 2;
    struct_dataProvider.uri = "C:/Dataprovider/path/updated";

    webService.updateDataProvider(struct_dataProvider, struct_dataProviderResult);

    QVERIFY2(struct_dataproviderOLD.id == struct_dataProviderResult.id, "Data Provider ID changed after update!" );
    QVERIFY2(struct_dataproviderOLD.name != struct_dataProviderResult.name, "Data Provider name changed after update!" );
    QVERIFY2(struct_dataproviderOLD.kind != struct_dataProviderResult.kind, "Data Provider kind changed after update!" );
    QVERIFY2(struct_dataproviderOLD.description != struct_dataProviderResult.description, "Data Provider description changed after update!" );
    QVERIFY2(struct_dataproviderOLD.status != struct_dataProviderResult.status, "Data Provider status changed after update!" );
    QVERIFY2(struct_dataproviderOLD.uri != struct_dataProviderResult.uri, "Data Provider URI changed after update!" );
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

void TestWebService::testUpdateDataProviderInvalidId()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 1;

    WebService webService;

    if(webService.updateDataProvider(struct_dataProvider, struct_dataProviderResult) == SOAP_OK)
    {
      QFAIL("Should not update a invalid Data Provider!");
    }
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


void TestWebService::testFindDataProvider()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProvider_found, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    if(struct_dataProviderResult.id == 0)
    {
      QFAIL("After added, a Data Providetr MUST have a valid ID!");
    }

    if(webService.findDataProvider(struct_dataProviderResult.id, struct_dataProvider_found) != SOAP_OK)
    {
      QFAIL("Can't find the Data Provider!");
    }

    QVERIFY2(struct_dataProviderResult.id == struct_dataProvider_found.id, "ID is not the same inserted!");
    QVERIFY2(struct_dataProviderResult.name == struct_dataProvider_found.name, "Name is not the same inserted!");
    QVERIFY2(struct_dataProviderResult.kind == struct_dataProvider_found.kind, "Kind is not the same inserted!");
    QVERIFY2(struct_dataProviderResult.description == struct_dataProvider_found.description, "Description is not the same inserted!");
    QVERIFY2(struct_dataProviderResult.status == struct_dataProvider_found.status, "Status is not the same inserted!");
    QVERIFY2(struct_dataProviderResult.uri == struct_dataProvider_found.uri, "URI is not the same inserted!");

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


void TestWebService::testFindDataProviderInvalidID()
{
  try
  {
    DataProvider struct_dataProvider;

    WebService webService;

    if(webService.findDataProvider(1, struct_dataProvider) == SOAP_OK)
    {
      QFAIL("Should not find a invalid Data Provider!");
    }

    // test OK
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


void TestWebService::testListDataProvider()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    if(struct_dataProviderResult.id == 0)
    {
      QFAIL("After added, a Data Providetr MUST have a valid ID!");
    }

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider2";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    if(struct_dataProviderResult.id == 0)
    {
      QFAIL("After added, a Data Providetr MUST have a valid ID!");
    }

    std::vector<DataProvider> vector_struct_dataProvider;

    if(webService.listDataProvider(vector_struct_dataProvider) != SOAP_OK)
    {
      QFAIL("Error to list Data Provider!");
    }

    QVERIFY2(vector_struct_dataProvider.size() == 2, "Error to list Data Providers, two expected!");
    // test OK
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


void TestWebService::TestAddDataSet()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet;

    struct_dataSet.id = 0;
    struct_dataSet.name = "Data Set";
    struct_dataSet.kind = 1;
    struct_dataSet.status = 1;
    struct_dataSet.description = "Data Set description";
    struct_dataSet.data_frequency = "00:05:00.00";
    struct_dataSet.schedule = "00:05:00.00";
    struct_dataSet.schedule_retry = "00:05:00.00";
    struct_dataSet.schedule_timeout = "00:05:00.00";
    struct_dataSet.data_provider_id = struct_dataProviderResult.id;

    DataSet struct_dataSetResult;
    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    QVERIFY2(struct_dataSetResult.id != 0, "Data Set should have a valid ID!");

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

void TestWebService::TestAddNullDataSet()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet;

    struct_dataSet.id = 0;

    DataSet struct_dataSetResult;
    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) == SOAP_OK)
    {
      QFAIL("Should not add a null Data Set!");
    }
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


void TestWebService::TestAddDataSetWithID()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet;

    struct_dataSet.id = 1;
    struct_dataSet.name = "Data Set";
    struct_dataSet.kind = 1;
    struct_dataSet.status = 1;
    struct_dataSet.description = "Data Set description";
    struct_dataSet.data_frequency = "00:05:00.00";
    struct_dataSet.schedule = "00:05:00.00";
    struct_dataSet.schedule_retry = "00:05:00.00";
    struct_dataSet.schedule_timeout = "00:05:00.00";
    struct_dataSet.data_provider_id = struct_dataProvider.id;

    DataSet struct_dataSetResult;
    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) == SOAP_OK)
    {
      QFAIL("Should not add a Data Set With an ID!");
    }
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

void TestWebService::TestAddDataSetWithWrongDataProviderID()
{
  try
  {
    WebService webService;

    DataSet struct_dataSet;

    struct_dataSet.id = 0;
    struct_dataSet.name = "Data Set";
    struct_dataSet.kind = 1;
    struct_dataSet.status = 1;
    struct_dataSet.description = "Data Set description";
    struct_dataSet.data_frequency = "00:05:00.00";
    struct_dataSet.schedule = "00:05:00.00";
    struct_dataSet.schedule_retry = "00:05:00.00";
    struct_dataSet.schedule_timeout = "00:05:00.00";
    struct_dataSet.data_provider_id = 1;

    DataSet struct_dataSetResult;
    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) == SOAP_OK)
    {
      QFAIL("Should not add a Data Set With a wrong Data Provider ID!");
    }
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


void TestWebService::testRemoveDataSet()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet;

    struct_dataSet.id = 0;
    struct_dataSet.name = "Data Set";
    struct_dataSet.kind = 1;
    struct_dataSet.status = 1;
    struct_dataSet.description = "Data Set description";
    struct_dataSet.data_frequency = "00:05:00.00";
    struct_dataSet.schedule = "00:05:00.00";
    struct_dataSet.schedule_retry = "00:05:00.00";
    struct_dataSet.schedule_timeout = "00:05:00.00";
    struct_dataSet.data_provider_id = struct_dataProviderResult.id;

    DataSet struct_dataSetResult;
    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    if(webService.removeDataSet(struct_dataSetResult.id) != SOAP_STOP)
    {
      QFAIL("Fail to remove a Data Set!");
    }
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


void TestWebService::testRemoveDataSetInvalidId()
{
  try
  {
    WebService webService;

    if(webService.removeDataSet(1) == SOAP_STOP)
    {
      QFAIL("Should not remove a invalid Data Set!");
    }
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


void TestWebService::testUpdateDataSet()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet, struct_dataSet_check;

    struct_dataSet.id = 0;
    struct_dataSet.name = "Data Set";
    struct_dataSet.kind = 1;
    struct_dataSet.status = 1;
    struct_dataSet.description = "Data Set description";
    struct_dataSet.data_frequency = "00:05:00.00";
    struct_dataSet.schedule = "00:05:00.00";
    struct_dataSet.schedule_retry = "00:05:00.00";
    struct_dataSet.schedule_timeout = "00:05:00.00";
    struct_dataSet.data_provider_id = struct_dataProviderResult.id;

    DataSet struct_dataSetResult;
    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    struct_dataSet_check = struct_dataSetResult;

    struct_dataSet.id = struct_dataSetResult.id;
    struct_dataSet.name = "Data Set Updated";
    struct_dataSet.kind = 2;
    struct_dataSet.status = 2;
    struct_dataSet.description = "Data Set description Updated";
    struct_dataSet.data_frequency = "00:06:00.00";
    struct_dataSet.schedule = "00:06:00.00";
    struct_dataSet.schedule_retry = "00:06:00.00";
    struct_dataSet.schedule_timeout = "00:06:00.00";

    if(webService.updateDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Fail to update Data Set");
    }

    QVERIFY2(struct_dataSetResult.id == struct_dataSet_check.id, "ID changed after update!");
    QVERIFY2(struct_dataSetResult.data_provider_id == struct_dataSet_check.data_provider_id, "Data Provider changed after update!");
    QVERIFY2(struct_dataSetResult.name != struct_dataSet_check.name, "Name didn't update!");
    QVERIFY2(struct_dataSetResult.kind != struct_dataSet_check.kind, "Kind didn't update!");
    QVERIFY2(struct_dataSetResult.status != struct_dataSet_check.status, "Status didn't update!");
    QVERIFY2(struct_dataSetResult.description != struct_dataSet_check.description, "Description didn't update!");
    QVERIFY2(struct_dataSetResult.data_frequency != struct_dataSet_check.data_frequency, "Data Frequency didn't update!");
    QVERIFY2(struct_dataSetResult.schedule != struct_dataSet_check.schedule, "Schedule didn't update!");
    QVERIFY2(struct_dataSetResult.schedule_retry != struct_dataSet_check.schedule_retry, "Schedule retry didn't update!");
    QVERIFY2(struct_dataSetResult.schedule_timeout != struct_dataSet_check.schedule_timeout, "Schedule Timeout didn't update!");

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


void TestWebService::testUpdateDataSetInvalidId()
{
  try
  {
    DataSet struct_dataSet;

    struct_dataSet.id = 1;
    struct_dataSet.data_provider_id = 1;
    struct_dataSet.name = "Data Set Updated";
    struct_dataSet.kind = 2;
    struct_dataSet.status = 2;
    struct_dataSet.description = "Data Set description Updated";
    struct_dataSet.data_frequency = "00:06:00.00";
    struct_dataSet.schedule = "00:06:00.00";
    struct_dataSet.schedule_retry = "00:06:00.00";
    struct_dataSet.schedule_timeout = "00:06:00.00";

    WebService webService;

    DataSet struct_dataSetResult;
    if(webService.updateDataSet(struct_dataSet, struct_dataSetResult) == SOAP_OK)
    {
      QFAIL("Should not update a invalid Data Set!");
    }
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


void TestWebService::testFindDataSet()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet, struct_dataSet_found;

    struct_dataSet.id = 0;
    struct_dataSet.name = "Data Set";
    struct_dataSet.kind = 1;
    struct_dataSet.status = 1;
    struct_dataSet.description = "Data Set description";
    struct_dataSet.data_frequency = "00:05:00.00";
    struct_dataSet.schedule = "00:05:00.00";
    struct_dataSet.schedule_retry = "00:05:00.00";
    struct_dataSet.schedule_timeout = "00:05:00.00";
    struct_dataSet.data_provider_id = struct_dataProviderResult.id;

    DataSet struct_dataSetResult;
    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    QVERIFY2(struct_dataSetResult.id != 0, "Data Set have a invalid ID!");

    if(webService.findDataSet(struct_dataSetResult.id, struct_dataSet_found) != SOAP_OK)
    {
      QFAIL("Failed to find the Data Set!");
    }

    QCOMPARE(struct_dataSetResult.id, struct_dataSet_found.id);
    QVERIFY2(struct_dataSetResult.data_provider_id == struct_dataSet_found.data_provider_id, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.name == struct_dataSet_found.name, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.kind == struct_dataSet_found.kind, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.status == struct_dataSet_found.status, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.description == struct_dataSet_found.description, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.data_frequency == struct_dataSet_found.data_frequency, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.schedule == struct_dataSet_found.schedule, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.schedule_retry == struct_dataSet_found.schedule_retry, "Error to find properly the Data Set!");
    QVERIFY2(struct_dataSetResult.schedule_timeout == struct_dataSet_found.schedule_timeout, "Error to find properly the Data Set!");

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


void TestWebService::testFindDataSetInvalidID()
{
  try
  {
    DataSet struct_dataSet;
    WebService webService;

    // VINICIUS: check findDataSet in terrma2/core method, it dont check if the data Set don't exist
    if(webService.findDataSet(1, struct_dataSet) == SOAP_OK)
    {
      QFAIL("Should not find an invalid Data Set!");
    }

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


void TestWebService::testListDataSet()
{
  try
  {
    DataProvider struct_dataProvider, struct_dataProviderResult;

    struct_dataProvider.id = 0;
    struct_dataProvider.name = "Data Provider";
    struct_dataProvider.kind = 1;
    struct_dataProvider.description = "Data Provider description";
    struct_dataProvider.status = 1;
    struct_dataProvider.uri = "pathDataProvider";

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet, struct_dataSetResult;

    struct_dataSet.id = 0;
    struct_dataSet.name = "Data Set";
    struct_dataSet.kind = 1;
    struct_dataSet.status = 1;
    struct_dataSet.description = "Data Set description";
    struct_dataSet.data_frequency = "00:05:00.00";
    struct_dataSet.schedule = "00:05:00.00";
    struct_dataSet.schedule_retry = "00:05:00.00";
    struct_dataSet.schedule_timeout = "00:05:00.00";
    struct_dataSet.data_provider_id = struct_dataProviderResult.id;

    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    QVERIFY2(struct_dataSetResult.id != 0, "Data Set should have a valid ID!");

    struct_dataSet.id = 0;
    struct_dataSet.name = "Data Set2";

    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    QVERIFY2(struct_dataSetResult.id != 0, "Data Set should have a valid ID!");


    std::vector<DataSet> vector_struct_dataSet;

    if(webService.listDataSet(vector_struct_dataSet) != SOAP_OK)
    {
      QFAIL("Error to list Data Provider!");
    }

    QVERIFY2(vector_struct_dataSet.size() == 2, "Error to list Data Providers, two expected!");
    // test OK
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
