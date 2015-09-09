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
  \file unittest/ws/collector/core/TestCollectorService.cpp

  \brief Test Collector...

  \author Paulo R. M. Oliveira
*/


#include "terrama2/ws/collector/server/CollectorService.hpp"

//QT
#include <QtTest>
#include <QApplication>

// STL
#include <memory>

//terralib
#include <terralib/postgis/Utils.h>
#include <terralib/postgis/Connection.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>

#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

class TestCollectorService: public QObject
{
  Q_OBJECT

protected:

    void initializeTerralib();
    void finalizeTerralib();

private slots:
    void initTestCase() // Run before all tests
    {
        initializeTerralib();
    }
    void cleanupTestCase() // Run after all tests
    {
        finalizeTerralib();
    }

    void init(); //run before each test
    void cleanup(); //run before each test

    //******Test functions********

    /*!
     * \brief Test Description
     */
    void testCollectorService();



    //******End of Test functions****

};

void TestCollectorService::init()
{

}

void TestCollectorService::cleanup()
{

}

void TestCollectorService::testCollectorService()
{
  std::cerr << "Start" << std::endl;

//  // Create dataprovider
//  std::shared_ptr<terrama2::ws::collector::core::DataProvider> dataProvider(new terrama2::ws::collector::core::DataProvider());
//  std::shared_ptr<terrama2::ws::collector::core::DataProvider> dataProvider1(new terrama2::ws::collector::core::DataProvider());
//  std::shared_ptr<terrama2::ws::collector::core::DataProvider> dataProvider2(new terrama2::ws::collector::core::DataProvider());

//  terrama2::ws::collector::appserver::CollectorService service;

//  service.addProvider(dataProvider);
//  service.addProvider(dataProvider1);
//  service.addProvider(dataProvider2);

//  // Create dataset
//  QList<std::shared_ptr<terrama2::ws::collector::core::Data>> dataList;
//  dataList.append(std::shared_ptr<terrama2::ws::collector::core::Data>(new terrama2::ws::collector::core::Data("queimadas.csv")));
//  std::shared_ptr<terrama2::ws::collector::core::Dataset> dataset(new terrama2::ws::collector::core::Dataset(1, dataProvider, dataList));

//  service.addDataset(dataset);
//  service.start();
  
  std::cerr << "End" << std::endl;
}


void TestCollectorService::initializeTerralib()
{
  // Initialize the Terralib support
  TerraLib::getInstance().initialize();

  te::plugin::PluginInfo* info;
  std::string plugins_path = te::common::FindInTerraLibPath("share/terralib/plugins");
  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.pgis.teplg");
  te::plugin::PluginManager::getInstance().add(info);
  te::plugin::PluginManager::getInstance().loadAll();
}



void TestCollectorService::finalizeTerralib()
{
  TerraLib::getInstance().finalize();
}

QTEST_MAIN(TestCollectorService)
#include "TestCollectorService.moc"
