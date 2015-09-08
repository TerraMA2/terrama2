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


#include <terrama2_config.hpp>
#include <terrama2/core/DataSetDAO.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/ApplicationController.hpp>

//QT
#include <QtTest>

// STL
#include <memory>
#include <map>
#include <exception>

//terralib
#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

class DataSetDAO: public QObject
{
  Q_OBJECT

protected:

    void initializeTerralib();
    void finalizeTerralib();

    void initializeTerraMA2();
    void finalizeTerraMA2();

private slots:
    void initTestCase() // Always run before all tests
    {
        initializeTerralib();
    }
    void cleanupTestCase() // Always run after all tests
    {
        finalizeTerralib();
    }

    void init(){ } //run before each test
    void cleanup(){ } //run before each test

    void testDataSetDAO();
};



void DataSetDAO::initializeTerralib()
{
  // Initialize the Terralib support
  TerraLib::getInstance().initialize();

  te::plugin::PluginInfo* info;
  std::string plugins_path = te::common::FindInTerraLibPath("share/terralib/plugins");
  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.pgis.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.gdal.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.ogr.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  te::plugin::PluginManager::getInstance().loadAll();
}

void DataSetDAO::finalizeTerralib()
{
  TerraLib::getInstance().finalize();
}

void DataSetDAO::initializeTerraMA2()
{
  std::string data_dir = TERRAMA2_DATA_DIR;
  QCOMPARE(terrama2::core::ApplicationController::getInstance().loadProject(data_dir + "/project.json"), true);
  std::auto_ptr<te::da::DataSourceTransactor> transactor = terrama2::core::ApplicationController::getInstance().getTransactor();
  QVERIFY(transactor.get());
}


void DataSetDAO::testDataSetDAO()
{
  std::auto_ptr<te::da::DataSourceTransactor> transactor = terrama2::core::ApplicationController::getInstance().getTransactor();
  QVERIFY(transactor.get());

  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("Server 1"));

  terrama2::core::DataSetPtr dataset(new terrama2::core::DataSet(dataProvider));
}


//QTEST_MAIN(TestDataSetDAO)
#include "TestDataSetDAO.moc"
