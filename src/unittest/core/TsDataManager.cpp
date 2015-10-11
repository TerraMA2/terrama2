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

#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/Filter.hpp>
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/Exception.hpp>
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

DataProviderPtr TsDataManager::createDataProvider()
{
  auto dataProvider = DataProviderPtr(new DataProvider("Server 1", DataProvider::FTP_TYPE));
  dataProvider->setStatus(DataProvider::ACTIVE);
  dataProvider->setDescription("This server...");
  dataProvider->setUri("localhost@...");

  return dataProvider;
}

DataSetPtr TsDataManager::createDataSet()
{
  DataProviderPtr dataProvider = createDataProvider();
  DataManager::getInstance().add(dataProvider);

  // create a new dataset and save it to the database
  DataSetPtr dataSet(new DataSet(dataProvider, "Queimadas", DataSet::OCCURENCE_TYPE));
  te::dt::TimeDuration dataFrequency(2,0,0);
  dataSet->setDataFrequency(dataFrequency);

  std::vector<DataSet::CollectRule> collectRules;
  {
    DataSet::CollectRule collectRule;
    collectRule.script = "... LUA SCRIPT 1...";
    collectRules.push_back(collectRule);
  }
  {
    DataSet::CollectRule collectRule;
    collectRule.script = "... LUA SCRIPT 2...";
    collectRules.push_back(collectRule);
  }
  dataSet->setCollectRules(collectRules);

  std::map<std::string, std::string> metadata;
  metadata["key"] = "value";
  metadata["key1"] = "value1";
  metadata["key2"] = "value2";

  dataSet->setMetadata(metadata);


  // Creates a data list with two DataSetItem
  std::vector<DataSetItemPtr> dataSetItemList;

  DataSetItemPtr dataSetItem(new DataSetItem(dataSet, DataSetItem::PCD_INPE_TYPE));

  FilterPtr filter(new Filter(dataSetItem));
  filter->setExpressionType(Filter::GREATER_THAN_TYPE);
  filter->setValue(100.);
  dataSetItem->setFilter(filter);

  dataSetItemList.push_back(dataSetItem);


  DataSetItemPtr dataSetItem2(new DataSetItem(dataSet, DataSetItem::FIRE_POINTS_TYPE));

  std::map<std::string, std::string> storageMetadata;
  storageMetadata["key"] = "value";
  storageMetadata["key1"] = "value1";
  storageMetadata["key2"] = "value2";

  dataSetItem2->setStorageMetadata(storageMetadata);

  dataSetItemList.push_back(dataSetItem2);
  dataSet->setDataSetItemList(dataSetItemList);

  return dataSet;
}

void TsDataManager::testLoad()
{
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataManagerLoaded()));

  DataSetPtr dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);

  DataManager::getInstance().unload();

  DataManager::getInstance().load();

  QCOMPARE(spy.count(), 1);

  // Calling load again should have no effect
  DataManager::getInstance().load();

  QCOMPARE(spy.count(), 1);

  QVERIFY2(DataManager::getInstance().providers().size() == 1, "List should have one provider!");
  QVERIFY2(DataManager::getInstance().dataSets().size() == 1, "List should have one dataset!");
}

void TsDataManager::testUnload()
{
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataManagerUnloaded()));

  DataSetPtr dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);

  DataManager::getInstance().unload();

  QVERIFY2(spy.count() == 1, "Expect an emitted signal");

  QVERIFY2(DataManager::getInstance().providers().size() == 0, "List of providers should be empty after unload!");
  QVERIFY2(DataManager::getInstance().dataSets().size() == 0, "List of datasets should be empty after unload!");
}



void TsDataManager::testAddDataProvider()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProviderPtr)));

  DataProviderPtr dataProvider = createDataProvider();

  DataManager::getInstance().add(dataProvider);

  QVERIFY2(spy.count() == 1, "Expect an emitted signal");

  QVERIFY2(dataProvider->id() != 0, "The id wasn't set in the provider after insert!");
}

void TsDataManager::testRemoveDataProvider()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderRemoved(DataProviderPtr)));

  DataProviderPtr dataProvider = createDataProvider();

  DataManager::getInstance().add(dataProvider);

  QVERIFY2(dataProvider->id() != 0, "The id wasn't set in the provider after insert!");

  // Removes the data provider
  DataManager::getInstance().removeDataProvider(dataProvider->id());

  QVERIFY2(spy.count() == 1, "Expect an emitted signal");

  // Lists all data providers
  auto vecDataProvider = DataManager::getInstance().providers();

  QVERIFY2(vecDataProvider.empty(), "List should be empty after remove!");
}

void TsDataManager::testFindDataProvider()
{
  DataProviderPtr dataProvider = createDataProvider();

  DataManager::getInstance().add(dataProvider);

  // Find the same data provider by id
  DataProviderPtr foundDataProvider = DataManager::getInstance().findDataProvider(dataProvider->id());

  QVERIFY2(foundDataProvider.get(), "Could not recover the data provider by id!");

  QVERIFY2("This server..." == foundDataProvider->description(), "Wrong Description in recovered provider");
  QVERIFY2("Server 1" == foundDataProvider->name(), "Wrong name in recovered provider");
  QVERIFY2(DataProvider::FTP_TYPE == foundDataProvider->kind(), "Wrong type in recovered provider");
  QVERIFY2(DataProvider::ACTIVE == foundDataProvider->status(), "Wrong status in recovered provider");
  QVERIFY2("localhost@..." == foundDataProvider->uri(), "Wrong uri in recovered provider");

}


void TsDataManager::testFindDataProviderByName()
{
  DataProviderPtr dataProvider = createDataProvider();

  DataManager::getInstance().add(dataProvider);

  // Find the same data provider by name
  DataProviderPtr foundDataProvider = DataManager::getInstance().findDataProvider(dataProvider->name());

  QVERIFY2(foundDataProvider.get(), "Could not recover the data provider by id!");

  QVERIFY2("This server..." == foundDataProvider->description(), "Wrong Description in recovered provider");
  QVERIFY2("Server 1" == foundDataProvider->name(), "Wrong name in recovered provider");
  QVERIFY2(DataProvider::FTP_TYPE == foundDataProvider->kind(), "Wrong type in recovered provider");
  QVERIFY2(DataProvider::ACTIVE == foundDataProvider->status(), "Wrong status in recovered provider");
  QVERIFY2("localhost@..." == foundDataProvider->uri(), "Wrong uri in recovered provider");

}

void TsDataManager::testUpdateDataProvider()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderUpdated(DataProviderPtr)));

  DataProviderPtr dataProvider = createDataProvider();

  DataManager::getInstance().add(dataProvider);

  dataProvider->setName("New server");
  dataProvider->setStatus(DataProvider::INACTIVE);
  dataProvider->setDescription("New server is ...");
  dataProvider->setUri("myserver@...");

  DataManager::getInstance().update(dataProvider);

  QVERIFY2(spy.count() == 1, "Expect an emitted signal");


  // Find the same data provider by id
  DataProviderPtr foundDataProvider = DataManager::getInstance().findDataProvider(dataProvider->id());

  QVERIFY2(foundDataProvider.get(), "Could not recover the data provider by id!");

  QVERIFY2(dataProvider->description() == foundDataProvider->description(), "Wrong Description after update");
  QVERIFY2(dataProvider->name() == foundDataProvider->name(), "Wrong name after update");
  QVERIFY2(dataProvider->kind() == foundDataProvider->kind(), "Wrong type after update");
  QVERIFY2(dataProvider->status() == foundDataProvider->status(), "Wrong status after update");
  QVERIFY2(dataProvider->uri() == foundDataProvider->uri(), "Wrong uri after update");

}


void TsDataManager::testUpdateDataProviderInvalidId()
{
  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    DataProviderPtr dataProvider = createDataProvider();
    DataManager::getInstance().update(dataProvider);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    // test ok
  }
}


void TsDataManager::testRemoveDataProviderInvalidId()
{
  try
  {
    DataManager::getInstance().removeDataProvider(0);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    // test ok
  }
}

void TsDataManager::testAddDataSet()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSetPtr)));

  DataSetPtr dataSet = createDataSet();

  DataManager::getInstance().add(dataSet);

  QVERIFY2(spy.count() == 1, "Expect an emitted signal");

// assure we have a valid dataset identifier
  QVERIFY2(dataSet->id() > 0, "Id must be different than zero after save()!");

  // Test find dataset
  DataSetPtr findDataSet = DataManager::getInstance().findDataSet(dataSet->id());

}

void TsDataManager::testRemoveDataSet()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  qRegisterMetaType<DataSetPtr>("DataSetPtr");

  DataSetPtr dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);


  QSignalSpy spyDataSet(&DataManager::getInstance(), SIGNAL(dataSetRemoved(DataSetPtr)));
  QSignalSpy spyDataProvider(&DataManager::getInstance(), SIGNAL(dataProviderUpdated(DataProviderPtr)));

  DataManager::getInstance().removeDataSet(dataSet->id());

  QVERIFY2(spyDataProvider.count() == 1, "Expect an emitted signal for an updated data provider");
  QVERIFY2(spyDataSet.count() == 1, "Expect an emitted signal for a removed dataset");

  auto findDataSet = DataManager::getInstance().findDataSet(dataSet->id());

  QVERIFY2(findDataSet.get() == nullptr, "Find should return null after remove");

}

void TsDataManager::testRemoveDataSetInvalidId()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetRemoved(DataSetPtr)));

  try
  {
    DataManager::getInstance().removeDataSet(0);
    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");

    // test ok
  }
}

void TsDataManager::testFindDataSet()
{
  DataSetPtr dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);

  auto foundDataSet = DataManager::getInstance().findDataSet(dataSet->id());

  QCOMPARE(foundDataSet->kind(), dataSet->kind());
  QCOMPARE(foundDataSet->name(), dataSet->name());
  QCOMPARE(foundDataSet->dataFrequency(), dataSet->dataFrequency());



  QCOMPARE(foundDataSet->collectRules().size(), dataSet->collectRules().size());
  auto dsCollectRules = dataSet->collectRules();
  auto foundCollectRules = foundDataSet->collectRules();
  for(unsigned int i = 0; i < dsCollectRules.size(); ++i)
  {
    QCOMPARE(dsCollectRules[i].script, foundCollectRules[i].script);
  }

  QCOMPARE(foundDataSet->dataSetItemList().size(), dataSet->dataSetItemList().size());

}


void TsDataManager::testFindDataSetByName()
{
  DataSetPtr dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);

  auto foundDataSet = DataManager::getInstance().findDataSet(dataSet->name());

  QCOMPARE(foundDataSet->kind(), dataSet->kind());
  QCOMPARE(foundDataSet->name(), dataSet->name());
  QCOMPARE(foundDataSet->dataFrequency(), dataSet->dataFrequency());



  QCOMPARE(foundDataSet->collectRules().size(), dataSet->collectRules().size());
  auto dsCollectRules = dataSet->collectRules();
  auto foundCollectRules = foundDataSet->collectRules();
  for(unsigned int i = 0; i < dsCollectRules.size(); ++i)
  {
    QCOMPARE(dsCollectRules[i].script, foundCollectRules[i].script);
  }

  QCOMPARE(foundDataSet->dataSetItemList().size(), dataSet->dataSetItemList().size());

}


void TsDataManager::testUpdateDataSet()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSetPtr)));

  DataSetPtr dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);

  te::dt::TimeDuration schedule(12,0,0);
  dataSet->setSchedule(schedule);

  te::dt::TimeDuration scheduleTimeout(0,30,0);
  dataSet->setScheduleTimeout(scheduleTimeout);

  te::dt::TimeDuration scheduleRetry(0,5,0);
  dataSet->setScheduleRetry(scheduleRetry);

  dataSet->setStatus(DataSet::ACTIVE);

  dataSet->setDescription("Description...");
  dataSet->setName("New queimadas");

  // Change the collect rule script
  std::vector<DataSet::CollectRule>  collectRules = dataSet->collectRules();
  collectRules[0].script = "... LUA SCRIPT UPDATE 1...";
  dataSet->setCollectRules(collectRules);

  // Remove the dataset item PCD_INPE

  auto dataSetItemList = dataSet->dataSetItemList();
  dataSetItemList.erase(dataSetItemList.begin());

  // Updates the data from FIRE_POINTS_TYPE
  dataSetItemList[0]->setMask("Queimadas_*");

  // Add a new dataset item of type PCD_TOA5_TYPE
  DataSetItemPtr dataSetItem(new DataSetItem(dataSet, DataSetItem::PCD_TOA5_TYPE));
  dataSetItemList.push_back(dataSetItem);
  dataSet->setDataSetItemList(dataSetItemList);

  DataManager::getInstance().update(dataSet);

  QVERIFY2(spy.count() == 1, "Expect an emitted signal");

  auto foundDataSet = DataManager::getInstance().findDataSet(dataSet->id());


  QVERIFY2(foundDataSet.get() != nullptr, "Find should return a valid dataset");

  QVERIFY2(dataSet->name() == foundDataSet->name(), "Name must be the same!");
  QVERIFY2(dataSet->kind() == foundDataSet->kind(), "Kind must be the same!");
  QVERIFY2(dataSet->status() == foundDataSet->status(), "Status must be the same!");
  QVERIFY2(dataSet->scheduleTimeout() == foundDataSet->scheduleTimeout(), "Schedule timeout must be the same!");
  QVERIFY2(dataSet->schedule() == foundDataSet->schedule(), "Schedule must be the same!");
  QVERIFY2(dataSet->scheduleRetry() == foundDataSet->scheduleRetry(), "Schedule retry must be the same!");
  QVERIFY2(dataSet->dataFrequency() == foundDataSet->dataFrequency(), "Data frequency must be the same!");

  QVERIFY2(collectRules[0].script == foundDataSet->collectRules()[0].script, "Collect rule script must be the same!");

  std::map<std::string, std::string> metadata = dataSet->metadata();
  QVERIFY2(metadata["key"] == foundDataSet->metadata()["key"], "Metadata key/value must be the same!");
  QVERIFY2(metadata["key1"] == foundDataSet->metadata()["key1"], "Metadata key1/value1 must be the same!");
  QVERIFY2(metadata["key2"] == foundDataSet->metadata()["key2"], "Metadata key2/value2 must be the same!");

  // Expected result is to remove the data PCD_INPE, update the FIRE_POINTS  and insert PCD_TOA5.

  QVERIFY2(foundDataSet->dataSetItemList().size() == 2, "dataSetItemList must have 2 itens!");

  auto dsItem0 = foundDataSet->dataSetItemList()[0];
  auto dsItem1 = foundDataSet->dataSetItemList()[1];

  QVERIFY2(dsItem0->kind() == DataSetItem::FIRE_POINTS_TYPE, "dataSetItemList[0] must be of the type FIRE_POINTS!");
  QVERIFY2(dsItem0->mask() == "Queimadas_*", "Mask should be 'Queimadas_*'!");
  QVERIFY2(dsItem1->kind() == DataSetItem::PCD_TOA5_TYPE, "dataSetItemList[1] must be of the type PCD-TOA5!");

  std::map<std::string, std::string> storageMetadata =  dsItem0->storageMetadata();
  QVERIFY2("value" == storageMetadata["key"], "Metadata key/value must be the same!");
  QVERIFY2("value1" == storageMetadata["key1"], "Metadata key1/value1 must be the same!");
  QVERIFY2("value2" == storageMetadata["key2"], "Metadata key2/value2 must be the same!");

}

void TsDataManager::testUpdateDataSetInvalidId()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSetPtr)));

  // Tries to update a dataset that doesn't have a valid ID
  try
  {
    DataSetPtr dataSet = createDataSet();
    DataManager::getInstance().update(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");

    // test ok
  }
}


void TsDataManager::testRemoveDataSetInUse()
{
  //TODO: Try to remove a dataset in use by an analysis, expected an exception: DataSetInUseError
}

void TsDataManager::testAddDataProviderWithId()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProviderPtr)));

  // Tries to add a data provider with an Id different than 0
  try
  {
    auto dataProvider = DataProviderPtr(new DataProvider("Server 1", DataProvider::FTP_TYPE, 1));
    DataManager::getInstance().add(dataProvider);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    // test ok
  }
}

void TsDataManager::testAddDataSetWihId()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSetPtr)));

  // Tries to update a dataset that doesn't have a valid ID
  try
  {
    DataProviderPtr dataProvider = createDataProvider();
    DataManager::getInstance().add(dataProvider);

    // create a new dataset and save it to the database
    DataSetPtr dataSet(new DataSet(dataProvider, "Queimadas", DataSet::OCCURENCE_TYPE, 1));

    DataManager::getInstance().add(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");

    // test ok
  }
}

void TsDataManager::testAddDataProviderWithDataSet()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProviderPtr)));

  auto dataProvider = createDataProvider();
  auto dataSets = dataProvider->dataSets();
  DataSetPtr dataSet(new DataSet(dataProvider, "Queimadas", DataSet::OCCURENCE_TYPE));
  dataSets.push_back(dataSet);
  dataProvider->setDataSets(dataSets);

  DataManager::getInstance().add(dataProvider);

  QVERIFY2(dataProvider->dataSets().size() != 0, "The dataset was not persisted!");

  foreach(auto ds, dataProvider->dataSets())
  {
    QVERIFY2(ds->id() != 0, "DataSet id wasn't set in the provider after insert!");
  }


  QVERIFY2(spy.count() == 1, "Expect an emitted signal");

  QVERIFY2(dataProvider->id() != 0, "The id wasn't set in the provider after insert!");
}

void TsDataManager::testAddNullDataProvider()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProviderPtr)));

  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    DataProviderPtr dataProvider;
    DataManager::getInstance().add(dataProvider);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
}


void TsDataManager::testAddNullDataSet()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSetPtr)));

  // Tries to update a dataset that doesn't have a valid ID
  try
  {
    DataSetPtr dataSet;
    DataManager::getInstance().add(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }

}


void TsDataManager::testUpdateNullDataProvider()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProviderPtr)));

  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    DataProviderPtr dataProvider;
    DataManager::getInstance().update(dataProvider);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
}

void TsDataManager::testUpdateNullDataSet()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSetPtr)));

  // Tries to update a dataset that doesn't have a valid ID
  try
  {
    DataSetPtr dataSet;
    DataManager::getInstance().update(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }

}

void TsDataManager::testFindNonExistentDataSet()
{
  DataProviderPtr foundDataProvider = DataManager::getInstance().findDataProvider(0);

  QVERIFY2(!foundDataProvider.get(), "Should return an empty shared pointer");

  foundDataProvider = DataManager::getInstance().findDataProvider(999);

  QVERIFY2(!foundDataProvider.get(), "Should return an empty shared pointer");
}

void TsDataManager::testUpdateNonexistentDataProvider()
{

  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProviderPtr)));

  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    auto dataProvider = DataProviderPtr(new DataProvider("Server 1", DataProvider::FTP_TYPE, 10));

    DataManager::getInstance().update(dataProvider);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
}

void TsDataManager::testFindNonExistentDataProvider()
{
  DataSetPtr dataSet = DataManager::getInstance().findDataSet(0);

  QVERIFY2(!dataSet.get(), "Should return an empty shared pointer");

  dataSet = DataManager::getInstance().findDataSet(999);

  QVERIFY2(!dataSet.get(), "Should return an empty shared pointer");
}

void TsDataManager::testRemoveNonExistentDataSet()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetRemoved(DataSetPtr)));

  // Tries to remove an nonexistent dataset
  try
  {
    DataManager::getInstance().removeDataSet(1);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");

    // test ok
  }
}

void TsDataManager::testRemoveNonExistentDataProvider()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataProviderAdded(DataProviderPtr)));

  // Tries to update a data provider that doesn't have a valid ID
  try
  {
    // Removes the data provider
    DataManager::getInstance().removeDataProvider(1);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
}

void TsDataManager::testAddDataSetWithNullProvider()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSetPtr)));

  // Tries to add an dataset with an invalid data provider
  try
  {
    DataProviderPtr nullProvider;
    DataSetPtr dataSet(new DataSet(nullProvider, "Queimadas", DataSet::OCCURENCE_TYPE));

    DataManager::getInstance().add(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }

}

void TsDataManager::testAddDataSetWithNonexistentProvider()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetAdded(DataSetPtr)));

  // Tries to add an dataset with an invalid data provider
  try
  {
    auto nonExistentProvider = DataProviderPtr(new DataProvider("Server 1", DataProvider::FTP_TYPE, 1));
    DataSetPtr dataSet(new DataSet(nonExistentProvider, "Queimadas", DataSet::OCCURENCE_TYPE));

    DataManager::getInstance().add(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
}

void TsDataManager::testRemoveDataProviderWithDataSet()
{
  qRegisterMetaType<DataProviderPtr>("DataProviderPtr");
  qRegisterMetaType<DataProviderPtr>("DataSetPtr");
  QSignalSpy spyDataProvider(&DataManager::getInstance(), SIGNAL(dataProviderRemoved(DataProviderPtr)));
  QSignalSpy spyDataSet(&DataManager::getInstance(), SIGNAL(dataSetRemoved(DataSetPtr)));


  auto dataProvider = createDataProvider();
  auto dataSets = dataProvider->dataSets();
  DataSetPtr dataSet(new DataSet(dataProvider, "Queimadas", DataSet::OCCURENCE_TYPE));
  dataSets.push_back(dataSet);
  dataProvider->setDataSets(dataSets);

  DataManager::getInstance().add(dataProvider);


  // Removes the data provider
  DataManager::getInstance().removeDataProvider(dataProvider->id());

  QVERIFY2(spyDataProvider.count() == 1, "Expect an emitted signal for a removed data provider");
  QVERIFY2(spyDataSet.count() == 1, "Expect an emitted signal for a removed dataset");

  // Lists all data providers
  auto vecDataProvider = DataManager::getInstance().providers();
  QVERIFY2(vecDataProvider.empty(), "List of providers should be empty after remove!");

  auto vecDataSets = DataManager::getInstance().dataSets();
  QVERIFY2(vecDataSets.empty(), "List of datasets should be empty after remove!");
}

void TsDataManager::testUpdateDataSetWithNullProvider()
{
  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSetPtr)));

  // Tries to add an dataset with an invalid data provider
  try
  {
    DataProviderPtr nullProvider;
    DataSetPtr dataSet(new DataSet(nullProvider, "Queimadas", DataSet::OCCURENCE_TYPE, 1));

    DataManager::getInstance().update(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }

}

void TsDataManager::testUpdateDataSetWithNonexistentProvider()
{

  DataSetPtr dataSet = createDataSet();
  DataManager::getInstance().add(dataSet);

  qRegisterMetaType<DataSetPtr>("DataSetPtr");
  QSignalSpy spy(&DataManager::getInstance(), SIGNAL(dataSetUpdated(DataSetPtr)));

  // Tries to add an dataset with an invalid data provider
  try
  {
    // Nonexistent data provider
    auto dataProvider = DataProviderPtr(new DataProvider("Server 1", DataProvider::FTP_TYPE, 10));

    DataSetPtr dataSet(new DataSet(dataProvider, "Queimadas", DataSet::OCCURENCE_TYPE, 1));

    DataManager::getInstance().update(dataSet);

    // An exception should be thrown, if not the test fails.
    QFAIL("terrama2::InvalidParameterError not thrown");
  }
  catch (terrama2::InvalidParameterError /*ex*/)
  {
    QVERIFY2(spy.count() == 0, "Should not emit a signal");
    // test ok
  }
}

