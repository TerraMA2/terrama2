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
  \file unittest/core/DataSetDAO.cpp

  \brief Test for DataSetDAO functionalities

  \author Paulo R. M. Oliveira
*/

// TerraMA2 Unittest
#include "TestDataSetDAO.hpp"

// TerraMA2
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/Filter.hpp>

// Qt
#include <QtTest>

void TestDataSetDAO::initTestCase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "DELETE FROM terrama2.dataset";
  transactor->execute(query);

  query = "DELETE FROM terrama2.data_provider";
  transactor->execute(query);

  transactor->commit();
}

void TestDataSetDAO::cleanupTestCase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "DELETE FROM terrama2.dataset";
  transactor->execute(query);

  query = "DELETE FROM terrama2.data_provider";
  transactor->execute(query);

  transactor->commit();
}

void TestDataSetDAO::testCRUDDataSet()
{
// create a new data provider and save it t the database
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("Server 1", terrama2::core::DataProvider::FTP_TYPE));

  terrama2::core::DataManager::getInstance().add(dataProvider);

// create a new dataset and save it to the database
  terrama2::core::DataSetPtr dataSet(new terrama2::core::DataSet(dataProvider, "Queimadas", terrama2::core::DataSet::OCCURENCE_TYPE));
  te::dt::TimeDuration dataFrequency(2,0,0);
  dataSet->setDataFrequency(dataFrequency);

  std::vector<terrama2::core::DataSet::CollectRule> collectRules;
  {
    terrama2::core::DataSet::CollectRule collectRule;
    collectRule.script = "... LUA SCRIPT 1...";
    collectRules.push_back(collectRule);
  }
  {
    terrama2::core::DataSet::CollectRule collectRule;
    collectRule.script = "... LUA SCRIPT 2...";
    collectRules.push_back(collectRule);
  }
  dataSet->setCollectRules(collectRules);

  std::map<std::string, std::string> metadata;
  metadata["key"] = "value";
  metadata["key1"] = "value1";
  metadata["key2"] = "value2";

  dataSet->setMetadata(metadata);


  // Creates a data list with two data's
  std::vector<terrama2::core::DataSetItemPtr> dataSetItemList;

  terrama2::core::DataSetItemPtr dataSetItem(new terrama2::core::DataSetItem(dataSet, terrama2::core::DataSetItem::PCD_INPE_TYPE));

  terrama2::core::FilterPtr filter(new terrama2::core::Filter(dataSetItem));
  filter->setByValueType(terrama2::core::Filter::GREATER_THAN_TYPE);
  filter->setByValue(100.);
  dataSetItem->setFilter(filter);
  dataSetItemList.push_back(dataSetItem);

  terrama2::core::DataSetItemPtr data2(new terrama2::core::DataSetItem(dataSet, terrama2::core::DataSetItem::FIRE_POINTS_TYPE));
  dataSetItemList.push_back(data2);
  dataSet->setDataSetItemList(dataSetItemList);

  terrama2::core::DataManager::getInstance().add(dataSet);

// assure we have a valid dataset identifier
  QVERIFY2(dataSet->id() > 0, "Id must be different than zero after save()!");


  te::dt::TimeDuration schedule(12,0,0);
  dataSet->setSchedule(schedule);

  te::dt::TimeDuration scheduleTimeout(0,30,0);
  dataSet->setScheduleTimeout(scheduleTimeout);

  te::dt::TimeDuration scheduleRetry(0,5,0);
  dataSet->setScheduleRetry(scheduleRetry);

  dataSet->setStatus(terrama2::core::DataSet::ACTIVE);

  dataSet->setDescription("Description...");
  dataSet->setName("New queimadas");

  // Change the collect rule script
  collectRules[0].script = "... LUA SCRIPT UPDATE 1...";
  dataSet->setCollectRules(collectRules);

  // Remove the data PCD_INPE
  dataSetItemList = dataSet->dataSetItemList();
  dataSetItemList.erase(dataSetItemList.begin());

  // Updates the data from FIRE_POINTS_TYPE
  dataSetItemList[0]->setMask("Queimadas_*");

  // Add a new data of type PCD_TOA5_TYPE
  dataSetItem.reset(new terrama2::core::DataSetItem(dataSet, terrama2::core::DataSetItem::PCD_TOA5_TYPE));
  dataSetItemList.push_back(dataSetItem);
  dataSet->setDataSetItemList(dataSetItemList);

  terrama2::core::DataManager::getInstance().update(dataSet);


  // Test find dataset
  terrama2::core::DataSetPtr findDataSet = terrama2::core::DataManager::getInstance().findDataSet(dataSet->id());

  QVERIFY2(findDataSet.get() != nullptr, "Find should return a valid dataset");

  QVERIFY2(dataSet->name() == findDataSet->name(), "Name must be the same!");
  QVERIFY2(dataSet->kind() == findDataSet->kind(), "Kind must be the same!");
  QVERIFY2(dataSet->status() == findDataSet->status(), "Status must be the same!");
  QVERIFY2(dataSet->scheduleTimeout() == findDataSet->scheduleTimeout(), "Schedule timeout must be the same!");
  QVERIFY2(dataSet->schedule() == findDataSet->schedule(), "Schedule must be the same!");
  QVERIFY2(dataSet->scheduleRetry() == findDataSet->scheduleRetry(), "Schedule retry must be the same!");
  QVERIFY2(dataSet->dataFrequency() == findDataSet->dataFrequency(), "Data frequency must be the same!");

  QVERIFY2(collectRules[0].script == findDataSet->collectRules()[0].script, "Collect rule script must be the same!");

  QVERIFY2(metadata["key"] == findDataSet->metadata()["key"], "Metadata key/value must be the same!");
  QVERIFY2(metadata["key1"] == findDataSet->metadata()["key1"], "Metadata key1/value1 must be the same!");
  QVERIFY2(metadata["key2"] == findDataSet->metadata()["key2"], "Metadata key2/value2 must be the same!");

  // Expected result is to remove the data PCD_INPE, update the FIRE_POINTS  and insert PCD_TOA5.
  QVERIFY2(findDataSet->dataSetItemList().size() == 2, "dataSetItemList must have 2 itens!");
  QVERIFY2(findDataSet->dataSetItemList()[0]->kind() == terrama2::core::DataSetItem::FIRE_POINTS_TYPE, "dataSetItemList[0] must be of the type FIRE_POINTS!");
  QVERIFY2(findDataSet->dataSetItemList()[0]->mask() == "Queimadas_*", "Mask should be 'Queimadas_*'!");
  QVERIFY2(findDataSet->dataSetItemList()[1]->kind() == terrama2::core::DataSetItem::PCD_TOA5_TYPE, "dataSetItemList[1] must be of the type PCD-TOA5!");

  // Test remove dataset
  terrama2::core::DataManager::getInstance().removeDataSet(dataSet->id());
  findDataSet = terrama2::core::DataManager::getInstance().findDataSet(dataSet->id());

  QVERIFY2(findDataSet.get() == nullptr, "Find should return null after remove");

  // Test list empty
  auto vecDataSet = terrama2::core::DataManager::getInstance().dataSets();
  QVERIFY2(vecDataSet.empty(), "List should be empty after remove!");


}

