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

#include "TestDataProviderDAO.hpp"

#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataProviderDAO.hpp>
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/ApplicationController.hpp>

//QT
#include <QtTest>

// STL
#include <vector>
#include <memory>



void TestDataProviderDAO::initTestCase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();
  std::string query = "DELETE FROM terrama2.data_provider";
  transactor->execute(query);
  transactor->commit();

}

void TestDataProviderDAO::cleanupTestCase()
{

}

void TestDataProviderDAO::testCRUDDataProvider()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();
  terrama2::core::DataProviderDAO dataProviderDAO(dataSource);

  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("Server 1"));
  dataProvider->setKind(terrama2::core::DataProvider::FTP_TYPE);

  // Inserts a new data provider
  QVERIFY2(dataProviderDAO.save(dataProvider), "Could not save the data provider!");



  // Updates a data provider
  dataProvider->setName("New server");
  dataProvider->setStatus(terrama2::core::DataProvider::ACTIVE);

  QVERIFY2(dataProviderDAO.update(dataProvider), "Could not update the data provider!");

  QVERIFY2(dataProvider->id() != 0, "The id wasn't set in the provider after insert!");



  // Recovers the same data provider by id
  terrama2::core::DataProviderPtr tempProvider = dataProviderDAO.find(dataProvider->id());

  QVERIFY2(tempProvider.get(), "Could not recover the data provider by id!");

  // Must be the same as the inserted one
  QCOMPARE(tempProvider->id(), dataProvider->id());
  QCOMPARE(tempProvider->name(), dataProvider->name());
  QCOMPARE(tempProvider->kind(), dataProvider->kind());
  QCOMPARE(tempProvider->status(), dataProvider->status());
  QCOMPARE(tempProvider->description(), dataProvider->description());

  // Lists all data providers
  std::vector<terrama2::core::DataProviderPtr> vecDataProvider = dataProviderDAO.list();


  QVERIFY2(!vecDataProvider.empty(), "Empty list but should have one data provider!");

  QVERIFY2(vecDataProvider.size() == 1, "Number of data providers recovered different than expected!");

  // Removes the data provider
  QVERIFY2(dataProviderDAO.remove(dataProvider->id()), "Could not remove the data provider!");


  QVERIFY2(vecDataProvider.empty(), "List should be empty after remove!");
}



#include "TestDataProviderDAO.moc"

