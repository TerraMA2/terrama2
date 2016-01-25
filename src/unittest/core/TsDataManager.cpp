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
  \file unittest/core/DataProviderDAO.cpp

  \brief Test for DataProviderDAO functionalities

  \author Paulo R. M. Oliveira
*/

#include "TsDataManager.hpp"
#include "Utils.hpp"

// TerraMA2
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/Filter.hpp>
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/Exception.hpp>
#include <terrama2/core/Intersection.hpp>
#include <terrama2/Exception.hpp>

//QT
#include <QtTest>
#include <QSignalSpy>

// STL
#include <memory>
#include <vector>


using namespace terrama2::core;

void TsDataManager::init()
{
  DataManager::getInstance().unload();
  clearDatabase();
}

void TsDataManager::cleanup()
{
  DataManager::getInstance().unload();
  clearDatabase();
}


void TsDataManager::clearDatabase()
{
  std::shared_ptr<te::da::DataSource> dataSource = ApplicationController::getInstance().getDataSource();

  if(!dataSource.get())
  {
    QFAIL("Invalid database connection");
  }

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();

  if(!transactor.get())
  {
    QFAIL("Invalid database transactor");
  }

  transactor->begin();

  std::string query = "DELETE FROM terrama2.dataset";
  transactor->execute(query);

  query = "DELETE FROM terrama2.data_provider";
  transactor->execute(query);

  transactor->commit();
}

DataProvider TsDataManager::createDataProvider()
{
  auto dataProvider = DataProvider();
  dataProvider.setName("Server 1");
  dataProvider.setKind(DataProvider::FTP_TYPE);
  dataProvider.setOrigin(DataProvider::COLLECTOR);
  dataProvider.setStatus(DataProvider::ACTIVE);
  dataProvider.setDescription("This server...");
  dataProvider.setUri("localhost@...");

  return dataProvider;
}

DataSet TsDataManager::createDataSet()
{
  DataProvider dataProvider = createDataProvider();
  DataManager::getInstance().add(dataProvider);

  // create a new dataset and save it to the database
  DataSet dataSet("Queimadas", DataSet::GRID_TYPE, 0, dataProvider.id());
  dataSet.setStatus(DataSet::Status::ACTIVE);

  te::dt::TimeDuration dataFrequency(2,0,0);
  dataSet.setDataFrequency(dataFrequency);

  std::map<std::string, std::string> metadata;
  metadata["key"] = "value";
  metadata["key1"] = "value1";
  metadata["key2"] = "value2";

  dataSet.setMetadata(metadata);

  Intersection intersection;
  std::map<std::string, std::vector<std::string> > attributeMap;
  std::vector<std::string> attrVec;
  attrVec.push_back("uf");
  attrVec.push_back("municipio");
  attributeMap["public.municipio"]  = attrVec;
  intersection.setAttributeMap(attributeMap);

  std::map<uint64_t, std::string > bandMap;
  bandMap[20]  = "";
  intersection.setBandMap(bandMap);
  dataSet.setIntersection(intersection);


  DataSetItem dataSetItem(DataSetItem::PCD_INPE_TYPE, 0, dataSet.id());

  dataSetItem.setPath("path");

  Filter filter(dataSetItem.id());
  filter.setExpressionType(Filter::GREATER_THAN_TYPE);
  filter.setValue(std::move(std::unique_ptr<double>(new double(100.))));
  dataSetItem.setFilter(filter);
  dataSetItem.setSrid(4326);

  dataSet.add(dataSetItem);



  DataSetItem dataSetItem2(DataSetItem::FIRE_POINTS_TYPE, 0, dataSet.id());

  std::map<std::string, std::string> itemMetadata;
  itemMetadata["key"] = "value";
  itemMetadata["key1"] = "value1";
  itemMetadata["key2"] = "value2";

  dataSetItem2.setMetadata(itemMetadata);

  dataSet.add(dataSetItem2);

  return dataSet;
}

void TsDataManager::testLoad()
{
  try
  {
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataManagerLoaded()));

    DataManager::getInstance().load();

    DataSet dataSet = createDataSet();
    DataManager::getInstance().add(dataSet);

    DataManager::getInstance().unload();

    DataManager::getInstance().load();


    QCOMPARE(spy.count(), 2);


    QVERIFY2(DataManager::getInstance().providers().size() == 1, "List should have one provider!");
    QVERIFY2(DataManager::getInstance().dataSets().size() == 1, "List should have one dataset!");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }


}

void TsDataManager::testUnload()
{
  try
  {
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataManagerUnloaded()));

    DataSet dataSet = createDataSet();
    DataManager::getInstance().add(dataSet);

    DataManager::getInstance().unload();

    QVERIFY2(spy.count() == 1, "Expect an emitted signal");

    QVERIFY2(DataManager::getInstance().providers().size() == 0, "List of providers should be empty after unload!");
    QVERIFY2(DataManager::getInstance().dataSets().size() == 0, "List of datasets should be empty after unload!");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}



void TsDataManager::testAddDataProvider()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProvider)));

    DataProvider dataProvider = createDataProvider();

    DataManager::getInstance().add(dataProvider);

    QVERIFY2(spy.count() == 1, "Expect an emitted signal");

    QVERIFY2(dataProvider.id() != 0, "The id wasn't set in the provider after insert!");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testRemoveDataProvider()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderRemoved(DataProvider)));

    DataProvider dataProvider = createDataProvider();

    DataManager::getInstance().add(dataProvider);

    QVERIFY2(dataProvider.id() != 0, "The id wasn't set in the provider after insert!");

    // Removes the data provider
    DataManager::getInstance().removeDataProvider(dataProvider.id());

    QVERIFY2(spy.count() == 1, "Expect an emitted signal");

    // Lists all data providers
    auto vecDataProvider = DataManager::getInstance().providers();

    QVERIFY2(vecDataProvider.empty(), "List should be empty after remove!");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }


}

void TsDataManager::testFindDataProvider()
{
  try
  {
    DataProvider dataProvider = createDataProvider();

    DataManager::getInstance().add(dataProvider);

    // Find the same data provider by id
    DataProvider foundDataProvider = DataManager::getInstance().findDataProvider(dataProvider.id());

    QVERIFY2(foundDataProvider.id() == dataProvider.id(), "Could not recover the data provider by id!");

    QVERIFY2("This server..." == foundDataProvider.description(), "Wrong Description in recovered provider");
    QVERIFY2("Server 1" == foundDataProvider.name(), "Wrong name in recovered provider");
    QVERIFY2(DataProvider::FTP_TYPE == foundDataProvider.kind(), "Wrong type in recovered provider");
    QVERIFY2(DataProvider::ACTIVE == foundDataProvider.status(), "Wrong status in recovered provider");
    QVERIFY2("localhost@..." == foundDataProvider.uri(), "Wrong uri in recovered provider");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}


void TsDataManager::testFindDataProviderByName()
{
  try
  {
    DataProvider dataProvider = createDataProvider();

    DataManager::getInstance().add(dataProvider);

    // Find the same data provider by name
    DataProvider foundDataProvider = DataManager::getInstance().findDataProvider(dataProvider.name());

    QVERIFY2(foundDataProvider.id() == dataProvider.id(), "Could not recover the data provider by id!");

    QVERIFY2("This server..." == foundDataProvider.description(), "Wrong Description in recovered provider");
    QVERIFY2("Server 1" == foundDataProvider.name(), "Wrong name in recovered provider");
    QVERIFY2(DataProvider::FTP_TYPE == foundDataProvider.kind(), "Wrong type in recovered provider");
    QVERIFY2(DataProvider::ACTIVE == foundDataProvider.status(), "Wrong status in recovered provider");
    QVERIFY2("localhost@..." == foundDataProvider.uri(), "Wrong uri in recovered provider");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testUpdateDataProvider()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderUpdated(DataProvider)));

    DataProvider dataProvider = createDataProvider();

    DataManager::getInstance().add(dataProvider);

    dataProvider.setName("New server");
    dataProvider.setStatus(DataProvider::INACTIVE);
    dataProvider.setDescription("New server is ...");
    dataProvider.setUri("myserver@...");

    DataManager::getInstance().update(dataProvider);

    QVERIFY2(spy.count() == 1, "Expect an emitted signal");


    // Find the same data provider by id
    DataProvider foundDataProvider = DataManager::getInstance().findDataProvider(dataProvider.id());

    QVERIFY2(foundDataProvider.id() == dataProvider.id(), "Could not recover the data provider by id!");

    QVERIFY2(dataProvider.description() == foundDataProvider.description(), "Wrong Description after update");
    QVERIFY2(dataProvider.name() == foundDataProvider.name(), "Wrong name after update");
    QVERIFY2(dataProvider.kind() == foundDataProvider.kind(), "Wrong type after update");
    QVERIFY2(dataProvider.status() == foundDataProvider.status(), "Wrong status after update");
    QVERIFY2(dataProvider.uri() == foundDataProvider.uri(), "Wrong uri after update");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}


void TsDataManager::testUpdateDataProviderShallow()
{
  try
  {

    DataProvider dataProvider = createDataProvider();

    DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE, 0, dataProvider.id());
    dataSet.setStatus(DataSet::Status::ACTIVE);
    dataProvider.add(dataSet);

    DataSet dataSet2("Queimadas2", DataSet::OCCURENCE_TYPE, 0, dataProvider.id());
    dataSet.setStatus(DataSet::Status::ACTIVE);
    dataProvider.add(dataSet2);

    DataManager::getInstance().add(dataProvider);

    // Update the name of the second
    auto& ds2 = dataProvider.datasets()[1];
    ds2.setName("New Queimadas");


    // Remove the first dataset
    dataProvider.removeDataSet(dataProvider.datasets()[0].id());


    // Add a new dataset
    DataSet dataSet3("Queimadas3", DataSet::OCCURENCE_TYPE, 0, dataProvider.id());
    dataSet.setStatus(DataSet::Status::ACTIVE);
    dataProvider.add(dataSet3);

    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderUpdated(DataProvider)));

    qRegisterMetaType<DataSet>("DataSet");
    qRegisterMetaType<uint64_t>("uint64_t");
    QSignalSpy spyAdded(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));
    QSignalSpy spyRemoved(&DataManager::getInstance(), SIGNAL(dataSetRemoved(uint64_t)));
    QSignalSpy spyUpdated(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSet)));

    dataProvider.setName("New server");
    dataProvider.setStatus(DataProvider::INACTIVE);
    dataProvider.setDescription("New server is ...");
    dataProvider.setUri("myserver@...");

    DataManager::getInstance().update(dataProvider);

    QVERIFY2(spy.count() == 1, "Expect an emitted signal");
    QVERIFY2(spyAdded.count() == 1, "Expect an emitted signal for an added dataset");
    QVERIFY2(spyRemoved.count() == 1, "Expect an emitted signal for an removed dataset");
    QVERIFY2(spyUpdated.count() == 1, "Expect an emitted signal for an updated dataset");


    // Find the same data provider by id
    DataProvider foundDataProvider = DataManager::getInstance().findDataProvider(dataProvider.id());

    QVERIFY2(foundDataProvider.id() == dataProvider.id(), "Could not recover the data provider by id!");

    QVERIFY2(dataProvider.description() == foundDataProvider.description(), "Wrong Description after update");
    QVERIFY2(dataProvider.name() == foundDataProvider.name(), "Wrong name after update");
    QVERIFY2(dataProvider.kind() == foundDataProvider.kind(), "Wrong type after update");
    QVERIFY2(dataProvider.status() == foundDataProvider.status(), "Wrong status after update");
    QVERIFY2(dataProvider.uri() == foundDataProvider.uri(), "Wrong uri after update");

    QVERIFY2(dataProvider.datasets().size() == 2, "Wrong number of datasets after update");
    QCOMPARE("New Queimadas", dataProvider.datasets()[0].name().c_str());
    QCOMPARE("Queimadas3", dataProvider.datasets()[1].name().c_str());
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}



void TsDataManager::testUpdateDataProviderInvalidId()
{
  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    DataProvider dataProvider = createDataProvider();
    DataManager::getInstance().update(dataProvider);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}


void TsDataManager::testRemoveDataProviderInvalidId()
{
  try
  {
    DataManager::getInstance().removeDataProvider(0);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testAddDataSet()
{
  try
  {

    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderUpdated(DataProvider)));

    qRegisterMetaType<DataSet>("DataSet");
    QSignalSpy spy2(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));

    DataSet dataSet = createDataSet();

    DataManager::getInstance().add(dataSet);

    QVERIFY2(spy.count() == 1, "Provider signal emitted");
    QVERIFY2(spy2.count() == 1, "DataSet signal emitted!");

// assure we have a valid dataset identifier
    QVERIFY2(dataSet.id() > 0, "Id must be different than zero after save()!");

    // Test find dataset
    DataSet findDataSet = DataManager::getInstance().findDataSet(dataSet.id());

    QVERIFY2(findDataSet.dataSetItems()[0].path() == "path", "Wrong dataset Item Path");

    auto intersection = findDataSet.intersection();

    auto bandMap = intersection.bandMap();
    QVERIFY2(bandMap.size() == 1, "Wrong number of grid configuration in intersection");

    auto attrMap = intersection.attributeMap();
    QVERIFY2(attrMap.size() == 1, "Wrong number of attributes in intersection");
    auto attrVec = attrMap["public.municipio"];
    QVERIFY2(attrVec.size() == 2, "Wrong number of attributes in intersection");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }



}

void TsDataManager::testRemoveDataSet()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderUpdated(DataProvider)));

    DataSet dataSet = createDataSet();
    DataManager::getInstance().add(dataSet);

    qRegisterMetaType<uint64_t>("uint64_t");
    QSignalSpy spyDataSet(&DataManager::getInstance(), SIGNAL(dataSetRemoved(uint64_t)));

    DataManager::getInstance().removeDataSet(dataSet.id());

    QVERIFY2(spyDataSet.count() == 1, "Expect an emitted signal for a removed dataset");

    auto foundDataSet = DataManager::getInstance().findDataSet(dataSet.id());

    QVERIFY2(spy.count() == 1, "Provider signal emitted");

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testRemoveDataSetInvalidId()
{
  qRegisterMetaType<uint64_t>("uint64_t");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetRemoved(uint64_t)));

  try
  {
    DataManager::getInstance().removeDataSet(0);
    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");

    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testFindDataSet()
{
  try
  {
    DataSet dataSet = createDataSet();
    DataManager::getInstance().add(dataSet);

    auto foundDataSet = DataManager::getInstance().findDataSet(dataSet.id());

    QCOMPARE(foundDataSet.kind(), dataSet.kind());
    QCOMPARE(foundDataSet.name(), dataSet.name());
    QCOMPARE(foundDataSet.dataFrequency(), dataSet.dataFrequency());
    QCOMPARE(foundDataSet.status(), dataSet.status());

    QCOMPARE(foundDataSet.dataSetItems().size(), dataSet.dataSetItems().size());
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }



}


void TsDataManager::testFindDataSetByName()
{
  try
  {
    DataSet dataSet = createDataSet();
    DataManager::getInstance().add(dataSet);

    auto foundDataSet = DataManager::getInstance().findDataSet(dataSet.name());

    QCOMPARE(foundDataSet.kind(), dataSet.kind());
    QCOMPARE(foundDataSet.name(), dataSet.name());
    QCOMPARE(foundDataSet.dataFrequency(), dataSet.dataFrequency());

    QCOMPARE(foundDataSet.dataSetItems().size(), dataSet.dataSetItems().size());
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}


void TsDataManager::testUpdateDataSet()
{
  try
  {
    qRegisterMetaType<DataSet>("DataSet");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSet)));

    DataSet dataSet = createDataSet();
    DataManager::getInstance().add(dataSet);

    te::dt::TimeDuration schedule(12,0,0);
    dataSet.setSchedule(schedule);

    te::dt::TimeDuration scheduleTimeout(0,30,0);
    dataSet.setScheduleTimeout(scheduleTimeout);

    te::dt::TimeDuration scheduleRetry(0,5,0);
    dataSet.setScheduleRetry(scheduleRetry);

    dataSet.setStatus(DataSet::ACTIVE);

    dataSet.setDescription("Description...");
    dataSet.setName("New queimadas");

    // Remove the dataset item PCD_INPE

    auto& dataSetItems = dataSet.dataSetItems();

    // Updates the data from FIRE_POINTS_TYPE
    auto& dsItem = dataSetItems[1];
    dsItem.setMask("Queimadas_*");
    dsItem.setPath("other_path");

    // Add a new dataset item of type PCD_TOA5_TYPE
    DataSetItem dataSetItem(DataSetItem::PCD_TOA5_TYPE, 0, dataSet.id());
    dataSetItem.setSrid(0);
    dataSet.add(dataSetItem);

    DataManager::getInstance().update(dataSet);

    QVERIFY2(spy.count() == 1, "Expect an emitted signal");

    auto foundDataSet = DataManager::getInstance().findDataSet(dataSet.id());


    QVERIFY2(foundDataSet.id() == dataSet.id(), "Find should return a valid dataset");

    QVERIFY2(dataSet.name() == foundDataSet.name(), "Name must be the same!");
    QVERIFY2(dataSet.kind() == foundDataSet.kind(), "Kind must be the same!");
    QVERIFY2(dataSet.status() == foundDataSet.status(), "Status must be the same!");
    QVERIFY2(dataSet.scheduleTimeout() == foundDataSet.scheduleTimeout(), "Schedule timeout must be the same!");
    QVERIFY2(dataSet.schedule() == foundDataSet.schedule(), "Schedule must be the same!");
    QVERIFY2(dataSet.scheduleRetry() == foundDataSet.scheduleRetry(), "Schedule retry must be the same!");
    QVERIFY2(dataSet.dataFrequency() == foundDataSet.dataFrequency(), "Data frequency must be the same!");

    std::map<std::string, std::string> metadata = dataSet.metadata();
    std::map<std::string, std::string> metadataFound = foundDataSet.metadata();

    QVERIFY2(metadata["key"] == metadataFound["key"], "Metadata key/value must be the same!");
    QVERIFY2(metadata["key1"] == metadataFound["key1"], "Metadata key1/value1 must be the same!");
    QVERIFY2(metadata["key2"] == metadataFound["key2"], "Metadata key2/value2 must be the same!");

    // Expected result is to remove the data PCD_INPE, update the FIRE_POINTS  and insert PCD_TOA5.

    QVERIFY2(dataSet.dataSetItems().size() == 3, "dataSetItems must have 2 itens!");

    auto dsItem0 = dataSet.dataSetItems()[0];
    auto dsItem1 = dataSet.dataSetItems()[1];
    auto dsItem2 = dataSet.dataSetItems()[2];

    QVERIFY2(dsItem0.id() > 0, "dataSetItems[0] Id must be valid");
    QVERIFY2(dsItem1.id() > 0, "dataSetItems[1] Id must be valid");
    QVERIFY2(dsItem2.id() > 0, "dataSetItems[2] Id must be valid");

    std::map<std::string, std::string> itemMetadata =  dsItem1.metadata();
    QVERIFY2("value" == itemMetadata["key"], "Metadata key/value must be the same!");
    QVERIFY2("value1" == itemMetadata["key1"], "Metadata key1/value1 must be the same!");
    QVERIFY2("value2" == itemMetadata["key2"], "Metadata key2/value2 must be the same!");

    foundDataSet.removeDataSetItem(dsItem0.id());
    foundDataSet.removeDataSetItem(dsItem1.id());
    foundDataSet.removeDataSetItem(dsItem2.id());

    DataManager::getInstance().update(foundDataSet);

    foundDataSet = DataManager::getInstance().findDataSet(dataSet.id());

    QVERIFY2(foundDataSet.dataSetItems().size() == 0, "dataSetItems must be empty!");


  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testUpdateDataSetInvalidId()
{
  try
  {
    qRegisterMetaType<DataSet>("DataSet");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSet)));

    // Tries to update a dataset that doesn't have a valid ID
    try
    {
      DataSet dataSet = createDataSet();
      DataManager::getInstance().update(dataSet);

      // An exception should be thrown, if not the test fails.
      QFAIL("terrama2::InvalidArgumentException not thrown");
    }
    catch (terrama2::InvalidArgumentException /*ex*/)
    {
      QVERIFY2(spy.count() == 0, "Should not emit a signal");

      // test ok
    }
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}


void TsDataManager::testRemoveDataSetInUse()
{
  //TODO: Try to remove a dataset in use by an analysis, expected an exception: DataSetInUseException
}

void TsDataManager::testAddDataProviderWithId()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProvider)));

    // Tries to add a data provider with an Id different than 0
    try
    {
      auto dataProvider = DataProvider("Provider", DataProvider::FTP_TYPE, 1);
      DataManager::getInstance().add(dataProvider);

      // An exception should be thrown, if not the test fails.
      QFAIL("terrama2::InvalidArgumentException not thrown");
    }
    catch (terrama2::InvalidArgumentException /*ex*/)
    {
      // test ok
    }
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testAddDataSetWihId()
{
  try
  {
    qRegisterMetaType<DataSet>("DataSet");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));

    // Tries to update a dataset that doesn't have a valid ID
    try
    {
      DataProvider dataProvider = createDataProvider();
      DataManager::getInstance().add(dataProvider);

      // create a new dataset and save it to the database
      DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE, 1, dataProvider.id());

      DataManager::getInstance().add(dataSet);

      // An exception should be thrown, if not the test fails.
      QFAIL("terrama2::InvalidArgumentException not thrown");
    }
    catch (terrama2::InvalidArgumentException /*ex*/)
    {
      QVERIFY2(spy.count() == 0, "Should not emit a signal");

      // test ok
    }
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }


}

void TsDataManager::testAddDataProviderShallow()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProvider)));

    qRegisterMetaType<DataSet>("DataSet");
    QSignalSpy spy2(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));

    auto dataProvider = createDataProvider();

    DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE, 0, dataProvider.id());
    dataProvider.add(dataSet);

    DataSet dataSet2("Queimadas2", DataSet::OCCURENCE_TYPE, 0, dataProvider.id());
    dataProvider.add(dataSet2);

    DataManager::getInstance().add(dataProvider, false);

    QVERIFY2(DataManager::getInstance().dataSets().size() == 2,  "The dataset was not added to the data manager!");
    QVERIFY2(dataProvider.datasets().size() != 0, "The dataset was not persisted!");

    for(auto ds: dataProvider.datasets())
    {
      QVERIFY2(ds.id() != 0, "DataSet id wasn't set in the provider after insert!");
    }


    QVERIFY2(spy.count() == 1, "Expect an emitted signal for an added provider");

    QVERIFY2(spy2.count() == 2, "Expect two emitted signals for added datasets");

    QVERIFY2(dataProvider.id() != 0, "The id wasn't set in the provider after insert!");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }


}

void TsDataManager::testAddNullDataProvider()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProvider)));

    // Tries to update a data provider that doesn't have a valid ID
    try
    {
      DataProvider dataProvider;
      DataManager::getInstance().add(dataProvider);

      // An exception should be thrown, if not the test fails.
      QFAIL("terrama2::InvalidArgumentException not thrown");
    }
    catch (terrama2::InvalidArgumentException /*ex*/)
    {
      QVERIFY2(spy.count() == 0, "Should not emit a signal");
      // test ok
    }
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}


void TsDataManager::testAddNullDataSet()
{
  try
  {
    qRegisterMetaType<DataSet>("DataSet");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));

    // Tries to update a dataset that doesn't have a valid ID
    try
    {
      DataSet dataSet;
      DataManager::getInstance().add(dataSet);

      // An exception should be thrown, if not the test fails.
      QFAIL("terrama2::InvalidArgumentException not thrown");
    }
    catch (terrama2::InvalidArgumentException /*ex*/)
    {
      QVERIFY2(spy.count() == 0, "Should not emit a signal");
      // test ok
    }
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }



}


void TsDataManager::testUpdateNullDataProvider()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProvider)));

    // Tries to update a data provider that doesn't have a valid ID
    try
    {
      DataProvider dataProvider;
      DataManager::getInstance().update(dataProvider);

      // An exception should be thrown, if not the test fails.
      QFAIL("terrama2::InvalidArgumentException not thrown");
    }
    catch (terrama2::InvalidArgumentException /*ex*/)
    {
      QVERIFY2(spy.count() == 0, "Should not emit a signal");
      // test ok
    }
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }


}

void TsDataManager::testUpdateNullDataSet()
{
  try
  {
    qRegisterMetaType<DataSet>("DataSet");
    QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));

    // Tries to update a dataset that doesn't have a valid ID
    try
    {
      DataSet dataSet;
      DataManager::getInstance().update(dataSet);

      // An exception should be thrown, if not the test fails.
      QFAIL("terrama2::InvalidArgumentException not thrown");
    }
    catch (terrama2::InvalidArgumentException /*ex*/)
    {
      QVERIFY2(spy.count() == 0, "Should not emit a signal");
      // test ok
    }

  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testFindNonExistentDataSet()
{
  try
  {
    DataProvider foundDataProvider = DataManager::getInstance().findDataProvider(999);

    QVERIFY2(foundDataProvider.id()== 0, "Should return an invalid provider");
    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testUpdateNonexistentDataProvider()
{

  qRegisterMetaType<DataProvider>("DataProvider");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProvider)));

  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    DataProvider dataProvider("Server 1", DataProvider::FTP_TYPE, 10);

    DataManager::getInstance().update(dataProvider);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testFindNonExistentDataProvider()
{

  try
  {
    auto dataSet = DataManager::getInstance().findDataSet(999);

    QVERIFY2(dataSet.id() == 0, "Should return an invalid dataset");

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testRemoveNonExistentDataSet()
{
  qRegisterMetaType<uint64_t>("uint64_t");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetRemoved(uint64_t)));

  // Tries to remove an nonexistent dataset
  try
  {
    DataManager::getInstance().removeDataSet(1);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");

    // test ok
  }

  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testRemoveNonExistentDataProvider()
{
  qRegisterMetaType<DataProvider>("DataProvider");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProvider)));

  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    // Removes the data provider
    DataManager::getInstance().removeDataProvider(1);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok

  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testAddDataSetWithNullProvider()
{
  qRegisterMetaType<DataSet>("DataSet");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));

  // Tries to add an dataset with an invalid data provider
  try
  {
    DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE);

    DataManager::getInstance().add(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }


}

void TsDataManager::testAddDataSetWithNonexistentProvider()
{

  qRegisterMetaType<DataProvider>("DataProvider");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSet)));

  // Tries to add an dataset with an invalid data provider
  try
  {
    DataProvider nonExistentProvider("Server 1", DataProvider::FTP_TYPE, 1);
    DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE, 0, nonExistentProvider.id());

    DataManager::getInstance().add(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testRemoveDataProviderWithDataSet()
{
  try
  {
    qRegisterMetaType<DataProvider>("DataProvider");
    QSignalSpy spyDataProvider(&DataManager::getInstance(), SIGNAL(dataProviderRemoved(DataProvider)));


    auto dataProvider = createDataProvider();
    auto dataSets = dataProvider.datasets();
    DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE, 0, dataProvider.id());

    dataProvider.add(dataSet);

    DataManager::getInstance().add(dataProvider);


    // Removes the data provider
    DataManager::getInstance().removeDataProvider(dataProvider.id());

    QVERIFY2(spyDataProvider.count() == 1, "Expect an emitted signal for a removed data provider");

    // Lists all data providers
    auto vecDataProvider = DataManager::getInstance().providers();
    QVERIFY2(vecDataProvider.empty(), "List of providers should be empty after remove!");

    auto vecDataSets = DataManager::getInstance().dataSets();
    QVERIFY2(vecDataSets.empty(), "List of datasets should be empty after remove!");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

}

void TsDataManager::testUpdateDataSetWithNullProvider()
{
  qRegisterMetaType<DataSet>("DataSet");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSet)));

  // Tries to add an dataset with an invalid data provider
  try
  {

    DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE);

    DataManager::getInstance().update(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testUpdateDataSetWithNonexistentProvider()
{

  DataSet dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);

  qRegisterMetaType<DataSet>("DataSet");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSet)));

  // Tries to add an dataset with an invalid data provider
  try
  {
    // Nonexistent data provider
    DataProvider dataProvider("Server 1", DataProvider::FTP_TYPE, 10);

    DataSet dataSet("Queimadas", DataSet::OCCURENCE_TYPE, 1, dataProvider.id());

    DataManager::getInstance().update(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidArgumentException not thrown");
  }
  catch (terrama2::InvalidArgumentException /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testDataProviderValidName()
{
  try
  {
    auto dataProvider = createDataProvider();
    DataManager::getInstance().add(dataProvider);

    QVERIFY2(DataManager::getInstance().isDataProviderNameValid("Server 1") == false, "Should not be valid");
    QVERIFY2(DataManager::getInstance().isDataProviderNameValid("Server 2") == true, "Should be valid");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }


}

void TsDataManager::testDatasetValidName()
{
  try
  {
    auto dataset = createDataSet();
    DataManager::getInstance().add(dataset);

    QVERIFY2(DataManager::getInstance().isDatasetNameValid("Queimadas") == false, "Should not be valid");
    QVERIFY2(DataManager::getInstance().isDatasetNameValid("Queimadas 1") == true, "Should be valid");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testListDataSetWihtAdditionalMap()
{
  try
  {
    auto dataset = createDataSet();
    dataset.setKind(DataSet::STATIC_DATA);
    DataManager::getInstance().add(dataset);

    auto datasets = DataManager::getInstance().dataSets();

    QVERIFY2(datasets.size() > 0, "Shouldn't be empty.");
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsDataManager::testMemoryDataManager()
{
  DataManager::getInstance().unload();


  std::vector<DataProvider> vecProviders;
  auto provider = createDataProvider();
  provider.setId(1);

  vecProviders.push_back(provider);

  DataManager::getInstance().load(vecProviders);

  auto foundProvider = DataManager::getInstance().findDataProvider(1);

  QVERIFY2(provider == foundProvider, "Should be the same.");


  DataManager::getInstance().unload();
  DataManager::getInstance().load();


}
