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
  \file terrama2/unittest/ws/server/TsWebService.cpp

  \brief Tests for the WebService class.

  \author Vinicius Campanha
*/

// STL
#include <memory>

// TerraMA2 Test
#include "TsWebService.hpp"

// TerraMA2
#include <terrama2/core/Exception.hpp>
#include "soapWebService.h"
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>

void TsWebService::init()
{
  clearDatabase();
  terrama2::core::DataManager::getInstance().load();
}

void TsWebService::cleanup()
{
  clearDatabase();
  terrama2::core::DataManager::getInstance().unload();
}


void TsWebService::clearDatabase()
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


DataProvider BuildStructDataProvider()
{
  DataProvider struct_dataProvider;

  struct_dataProvider.id = 0;
  struct_dataProvider.name = "Data Provider";
  struct_dataProvider.kind = 1;
  struct_dataProvider.description = "Data Provider description";
  struct_dataProvider.status = 1;
  struct_dataProvider.uri = "pathDataProvider";

  return struct_dataProvider;
}

DataSet BuildStructDataSet(int providerId)
{
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
  struct_dataSet.data_provider_id = providerId;

  for(int i = 0; i < 5; i++)
  {
    DataSetItem dataset_item{0,0,0,0,"","","",0,0,"","","","","","","",0,std::nan(""),0,""};

    dataset_item.kind = i + 1;
    dataset_item.id = 0;
    dataset_item.dataset = struct_dataSet.id;
    dataset_item.status = 1;
    dataset_item.mask = "mask" + i;
    dataset_item.timezone = "-3";

    if(i == 1)
    {
      dataset_item.filter_datasetItem = dataset_item.id;

      // VINICIUS: update tests
//      dataset_item.filter_discardBefore = "2002-Jan-20 23:59:59";
//      dataset_item.filter_discardAfter = "2002-Jan-21 23:59:59";

      te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType);

      const double &xc(5), &yc(5), &halfSize(5);
      s->setPoint(0, xc - halfSize, yc - halfSize); // lower left
      s->setPoint(1, xc - halfSize, yc + halfSize); // upper left
      s->setPoint(2, xc + halfSize, yc + halfSize); // upper rigth
      s->setPoint(3, xc + halfSize, yc - halfSize); // lower rigth
      s->setPoint(4, xc - halfSize, yc - halfSize); // closing

      te::gm::Polygon* p = new te::gm::Polygon(0, te::gm::PolygonType);
      p->push_back(s);

      // VINICIUS: toString() is generating a wrong WKT, need to replace '\n' for ','
      std::string geom = p->toString();
      std::replace( geom.begin(), geom.end(), '\n', ',');
      dataset_item.filter_geometry = geom;
      dataset_item.filter_value = 10;
      dataset_item.filter_expressionType = 2;
      dataset_item.filter_bandFilter = "filter_bandFilter";
    }

    if(i == 2)
      dataset_item.filter_value = std::nan("");

    dataset_item.metadata_keys.push_back("one");
    dataset_item.metadata_values.push_back("two");
    dataset_item.metadata_keys.push_back("two");
    dataset_item.metadata_values.push_back("one");

    struct_dataSet.dataset_items.push_back(dataset_item);
  }

  return struct_dataSet;
}

void TsWebService::TestStatus()
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


void TsWebService::TestAddDataProvider()
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

void TsWebService::TestAddNullDataProvider()
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

void TsWebService::TestAddDataProviderWithID()
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


void TsWebService::testRemoveDataProvider()
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

void TsWebService::testRemoveDataProviderInvalidId()
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


void TsWebService::testUpdateDataProvider()
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
    struct_dataProvider.status = 0;
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

void TsWebService::testUpdateDataProviderInvalidId()
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


void TsWebService::testFindDataProvider()
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


void TsWebService::testFindDataProviderInvalidID()
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


void TsWebService::testListDataProvider()
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


void TsWebService::TestAddDataSet()
{
  try
  {
    DataProvider struct_dataProvider = BuildStructDataProvider();
    DataProvider struct_dataProviderResult;

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet = BuildStructDataSet(struct_dataProviderResult.id);
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

void TsWebService::TestAddNullDataSet()
{
  try
  {
    DataProvider struct_dataProvider = BuildStructDataProvider();
    DataProvider struct_dataProviderResult;

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


void TsWebService::TestAddDataSetWithID()
{
  try
  {
    DataProvider struct_dataProvider = BuildStructDataProvider();
    DataProvider struct_dataProviderResult;

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet = BuildStructDataSet(struct_dataProviderResult.id);
    DataSet struct_dataSetResult;

    struct_dataSet.id = 1;

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

void TsWebService::TestAddDataSetWithWrongDataProviderID()
{
  try
  {
    DataSet struct_dataSet = BuildStructDataSet(1);
    DataSet struct_dataSetResult;

    WebService webService;

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


void TsWebService::testRemoveDataSet()
{
  try
  {
    DataProvider struct_dataProvider = BuildStructDataProvider();
    DataProvider struct_dataProviderResult;

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet = BuildStructDataSet(struct_dataProviderResult.id);
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


void TsWebService::testRemoveDataSetInvalidId()
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


void TsWebService::testUpdateDataSet()
{
  try
  {
    DataProvider struct_dataProvider = BuildStructDataProvider();
    DataProvider struct_dataProviderResult;

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet = BuildStructDataSet(struct_dataProviderResult.id);
    DataSet struct_dataSetResult;

    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    DataSet struct_dataSet_check = struct_dataSetResult;

    struct_dataSetResult.name = "Data Set Updated";
    struct_dataSetResult.kind = 2;
    struct_dataSetResult.status = 2;
    struct_dataSetResult.description = "Data Set description Updated";
    struct_dataSetResult.data_frequency = "00:06:00.00";
    struct_dataSetResult.schedule = "00:06:00.00";
    struct_dataSetResult.schedule_retry = "00:06:00.00";
    struct_dataSetResult.schedule_timeout = "00:06:00.00";


    for(unsigned int i = 0; i < struct_dataSetResult.dataset_items.size(); i++)
    {
      struct_dataSetResult.dataset_items.at(i).kind = (i != 5 ? struct_dataSetResult.dataset_items.at(i).kind +1 : 1);
      struct_dataSetResult.dataset_items.at(i).status = 0;
      struct_dataSetResult.dataset_items.at(i).mask = "mask_update";
      struct_dataSetResult.dataset_items.at(i).timezone = "-4";

      if(i == 1)
      {
        // VINICIUS: update tests
//        struct_dataSetResult.dataset_items.at(i).filter_discardBefore = "2015-Jan-20 23:59:59";
//        struct_dataSetResult.dataset_items.at(i).filter_discardAfter = "2015-Jan-21 23:59:59";

        te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType);

        const double &xc(7), &yc(7), &halfSize(7);
        s->setPoint(0, xc - halfSize, yc - halfSize); // lower left
        s->setPoint(1, xc - halfSize, yc + halfSize); // upper left
        s->setPoint(2, xc + halfSize, yc + halfSize); // upper rigth
        s->setPoint(3, xc + halfSize, yc - halfSize); // lower rigth
        s->setPoint(4, xc - halfSize, yc - halfSize); // closing

        te::gm::Polygon* p = new te::gm::Polygon(0, te::gm::PolygonType);
        p->push_back(s);

        // VINICIUS: toString() is generating a wrong WKT, need to replace '\n' for ','
        std::string geom = p->toString();
        std::replace( geom.begin(), geom.end(), '\n', ',');
        struct_dataSetResult.dataset_items.at(i).filter_geometry = geom;
        struct_dataSetResult.dataset_items.at(i).filter_value = std::nan("");
        struct_dataSetResult.dataset_items.at(i).filter_expressionType = 3;
        struct_dataSetResult.dataset_items.at(i).filter_bandFilter = "filter_bandFilter_update";
      }

      if(i == 2)
        struct_dataSetResult.dataset_items.at(i).filter_value = 15;

      struct_dataSetResult.dataset_items.at(i).metadata_keys.at(0) = "three";
      struct_dataSetResult.dataset_items.at(i).metadata_values.at(0) = "four";
      struct_dataSetResult.dataset_items.at(i).metadata_keys.at(1) = "four";
      struct_dataSetResult.dataset_items.at(i).metadata_values.at(1) = "three";
    }

    if(webService.updateDataSet(struct_dataSetResult, struct_dataSetResult) != SOAP_OK)
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

    QCOMPARE(struct_dataSet_check.dataset_items.size(), struct_dataSetResult.dataset_items.size());

    for(unsigned int i = 0; i < struct_dataSetResult.dataset_items.size(); i++)
    {
      QVERIFY(struct_dataSet_check.dataset_items.at(i).dataset == struct_dataSetResult.dataset_items.at(i).dataset);
      QVERIFY(struct_dataSet_check.dataset_items.at(i).id == struct_dataSetResult.dataset_items.at(i).id);
      QVERIFY(struct_dataSet_check.dataset_items.at(i).kind != struct_dataSetResult.dataset_items.at(i).kind);
      QVERIFY(struct_dataSet_check.dataset_items.at(i).mask != struct_dataSetResult.dataset_items.at(i).mask);
      QVERIFY(struct_dataSet_check.dataset_items.at(i).status != struct_dataSetResult.dataset_items.at(i).status);
      QVERIFY(struct_dataSet_check.dataset_items.at(i).timezone != struct_dataSetResult.dataset_items.at(i).timezone);

      QVERIFY(struct_dataSet_check.dataset_items.at(i).filter_datasetItem == struct_dataSetResult.dataset_items.at(i).filter_datasetItem);

      if(struct_dataSet_check.dataset_items.at(i).filter_expressionType != terrama2::core::Filter::NONE_TYPE)
        QVERIFY(struct_dataSet_check.dataset_items.at(i).filter_expressionType != struct_dataSetResult.dataset_items.at(i).filter_expressionType);
      else
        QCOMPARE(struct_dataSet_check.dataset_items.at(i).filter_expressionType, struct_dataSetResult.dataset_items.at(i).filter_expressionType);

      if(struct_dataSet_check.dataset_items.at(i).filter_bandFilter != "")
        QVERIFY(struct_dataSet_check.dataset_items.at(i).filter_bandFilter != struct_dataSetResult.dataset_items.at(i).filter_bandFilter);
      else
        QCOMPARE(struct_dataSet_check.dataset_items.at(i).filter_bandFilter, struct_dataSetResult.dataset_items.at(i).filter_bandFilter);

      // VINICIUS: update tests
//      if(struct_dataSet_check.dataset_items.at(i).filter_discardBefore != "")
//        QVERIFY(struct_dataSet_check.dataset_items.at(i).filter_discardBefore != struct_dataSetResult.dataset_items.at(i).filter_discardBefore);
//      else
//        QCOMPARE(struct_dataSet_check.dataset_items.at(i).filter_discardBefore, struct_dataSetResult.dataset_items.at(i).filter_discardBefore);

//      if(struct_dataSet_check.dataset_items.at(i).filter_discardAfter != "")
//        QVERIFY(struct_dataSet_check.dataset_items.at(i).filter_discardAfter != struct_dataSetResult.dataset_items.at(i).filter_discardAfter);
//      else
//        QCOMPARE(struct_dataSet_check.dataset_items.at(i).filter_discardAfter, struct_dataSetResult.dataset_items.at(i).filter_discardAfter);

      if(struct_dataSet_check.dataset_items.at(i).filter_geometry != "")
        QVERIFY(struct_dataSet_check.dataset_items.at(i).filter_geometry != struct_dataSetResult.dataset_items.at(i).filter_geometry);
      else
        QCOMPARE(struct_dataSet_check.dataset_items.at(i).filter_geometry, struct_dataSetResult.dataset_items.at(i).filter_geometry);

      if(!(std::isnan(struct_dataSet_check.dataset_items.at(i).filter_value) && std::isnan(struct_dataSet_check.dataset_items.at(i).filter_value)))
        QVERIFY(struct_dataSet_check.dataset_items.at(i).filter_value != struct_dataSetResult.dataset_items.at(i).filter_value);

      QCOMPARE(struct_dataSet_check.dataset_items.at(i).metadata_keys.size(), struct_dataSetResult.dataset_items.at(i).metadata_keys.size());
      QCOMPARE(struct_dataSet_check.dataset_items.at(i).metadata_values.size(), struct_dataSetResult.dataset_items.at(i).metadata_values.size());

      QCOMPARE(struct_dataSetResult.dataset_items.at(i).metadata_keys.size(), struct_dataSetResult.dataset_items.at(i).metadata_values.size());

      for(unsigned int j = 0; j < struct_dataSetResult.dataset_items.at(i).metadata_keys.size(); j++)
      {
        QVERIFY(struct_dataSetResult.dataset_items.at(i).metadata_keys.at(j) != struct_dataSet_check.dataset_items.at(i).metadata_keys.at(j));
        QVERIFY(struct_dataSetResult.dataset_items.at(i).metadata_values.at(j) != struct_dataSet_check.dataset_items.at(i).metadata_values.at(j));

        j++;
      }
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


void TsWebService::testUpdateDataSetInvalidId()
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


void TsWebService::testFindDataSet()
{
  try
  {
    DataProvider struct_dataProvider = BuildStructDataProvider();
    DataProvider struct_dataProviderResult;

    WebService webService;

    if (webService.addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
    {
      QFAIL("Add a Data Provider failed!");
    }

    DataSet struct_dataSet = BuildStructDataSet(struct_dataProviderResult.id);
    DataSet struct_dataSetResult;

    if(webService.addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
    {
      QFAIL("Add a Data Set failed!");
    }

    QVERIFY2(struct_dataSetResult.id != 0, "Data Set have a invalid ID!");

    DataSet struct_dataSet_found;

    if(webService.findDataSet(struct_dataSetResult.id, struct_dataSet_found) != SOAP_OK)
    {
      QFAIL("Failed to find the Data Set!");
    }

    QCOMPARE(struct_dataSetResult.id, struct_dataSet_found.id);
    QCOMPARE(struct_dataSetResult.data_provider_id, struct_dataSet_found.data_provider_id);
    QCOMPARE(struct_dataSetResult.name, struct_dataSet_found.name);
    QCOMPARE(struct_dataSetResult.kind, struct_dataSet_found.kind);
    QCOMPARE(struct_dataSetResult.status, struct_dataSet_found.status);
    QCOMPARE(struct_dataSetResult.description, struct_dataSet_found.description);
    QCOMPARE(struct_dataSetResult.data_frequency, struct_dataSet_found.data_frequency);
    QCOMPARE(struct_dataSetResult.schedule, struct_dataSet_found.schedule);
    QCOMPARE(struct_dataSetResult.schedule_retry, struct_dataSet_found.schedule_retry);
    QCOMPARE(struct_dataSetResult.schedule_timeout, struct_dataSet_found.schedule_timeout);

    QCOMPARE(struct_dataSet_found.dataset_items.size(), struct_dataSetResult.dataset_items.size());

    for(unsigned int i = 0; i < struct_dataSetResult.dataset_items.size(); i++)
    {
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).dataset, struct_dataSetResult.dataset_items.at(i).dataset);
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).id, struct_dataSetResult.dataset_items.at(i).id);
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).kind, struct_dataSetResult.dataset_items.at(i).kind);
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).mask, struct_dataSetResult.dataset_items.at(i).mask);
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).status, struct_dataSetResult.dataset_items.at(i).status);
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).timezone, struct_dataSetResult.dataset_items.at(i).timezone);

      QCOMPARE(struct_dataSet_found.dataset_items.at(i).filter_datasetItem, struct_dataSetResult.dataset_items.at(i).filter_datasetItem);
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).filter_expressionType, struct_dataSetResult.dataset_items.at(i).filter_expressionType);
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).filter_bandFilter, struct_dataSetResult.dataset_items.at(i).filter_bandFilter);

      // VINICIUS: update tests
//      QCOMPARE(struct_dataSet_found.dataset_items.at(i).filter_discardBefore, struct_dataSetResult.dataset_items.at(i).filter_discardBefore);
//      QCOMPARE(struct_dataSet_found.dataset_items.at(i).filter_discardAfter, struct_dataSetResult.dataset_items.at(i).filter_discardAfter);

      // VINICIUS: terrama2::core don't save geometry yet
      //QCOMPARE(struct_dataSet_found.dataset_items.at(i).filter_geometry, struct_dataSetResult.dataset_items.at(i).filter_geometry);

      if(!std::isnan(struct_dataSetResult.dataset_items.at(i).filter_value))
        QCOMPARE(struct_dataSet_found.dataset_items.at(i).filter_value, struct_dataSetResult.dataset_items.at(i).filter_value);
      else
        if(!std::isnan(struct_dataSet_found.dataset_items.at(i).filter_value))
          QFAIL("Compared filter values are not the same");

      QCOMPARE(struct_dataSet_found.dataset_items.at(i).metadata_keys.size(), struct_dataSetResult.dataset_items.at(i).metadata_keys.size());
      QCOMPARE(struct_dataSet_found.dataset_items.at(i).metadata_values.size(), struct_dataSetResult.dataset_items.at(i).metadata_values.size());

      QCOMPARE(struct_dataSetResult.dataset_items.at(i).metadata_keys.size(), struct_dataSetResult.dataset_items.at(i).metadata_values.size());

      for(unsigned int j = 0; j < struct_dataSetResult.dataset_items.at(i).metadata_keys.size(); j++)
      {
        QCOMPARE(struct_dataSetResult.dataset_items.at(i).metadata_keys.at(j), struct_dataSet_found.dataset_items.at(i).metadata_keys.at(j));
        QCOMPARE(struct_dataSetResult.dataset_items.at(i).metadata_values.at(j), struct_dataSet_found.dataset_items.at(i).metadata_values.at(j));

        j++;
      }
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


void TsWebService::testFindDataSetInvalidID()
{
  try
  {
    DataSet struct_dataSet;
    WebService webService;

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


void TsWebService::testListDataSet()
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
