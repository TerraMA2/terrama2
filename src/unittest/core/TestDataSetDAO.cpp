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

#include "TestDataSetDAO.hpp"

#include <terrama2/core/DataSetDAO.hpp>
#include <terrama2/core/DataProviderDAO.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/ApplicationController.hpp>

//QT
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
  terrama2::core::DataProviderDAO dataProviderDAO;
  terrama2::core::DataSetDAO dataSetDAO;

  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("Server 1", terrama2::core::DataProvider::FTP_TYPE));
  dataProviderDAO.save(dataProvider);

  terrama2::core::DataSetPtr dataSet(new terrama2::core::DataSet(dataProvider, "Queimadas", terrama2::core::DataSet::OCCURENCE_TYPE));
  te::dt::TimeDuration dataFrequency(2,0,0);
  dataSet->setDataFrequency(dataFrequency);

  dataSetDAO.save(dataSet);

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

  dataSetDAO.update(dataSet);


  terrama2::core::DataSetPtr findDataSet = dataSetDAO.find(dataSet->id());

  QVERIFY2(dataSet->name() == findDataSet->name(), "Name must be the same!");
  QVERIFY2(dataSet->status() == findDataSet->status(), "Status must be the same!");
  QVERIFY2(dataSet->scheduleTimeout() == findDataSet->scheduleTimeout(), "Schedule timeout must be the same!");
  QVERIFY2(dataSet->schedule() == findDataSet->schedule(), "Schedule must be the same!");
  QVERIFY2(dataSet->scheduleRetry() == findDataSet->scheduleRetry(), "Schedule retry must be the same!");
  QVERIFY2(dataSet->dataFrequency() == findDataSet->dataFrequency(), "Data frequency must be the same!");

}


#include "TestDataSetDAO.moc"

