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
  \file unittest/ws/collector/core/TestCollector.cpp

  \brief Test Collector...

  \author Paulo R. M. Oliveira
*/


#include "terrama2/collector/Collector.hpp"

//QT
#include <QtTest>

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

class TestCollector: public QObject
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
    void testTiffCollector();



    //******End of Test functions****

private:
    std::auto_ptr<te::da::DataSource> ds_;
    std::auto_ptr<te::da::DataSourceTransactor> transactor_;
};

void TestCollector::init()
{
    std::map<std::string, std::string> connInfo;
    connInfo["PG_HOST"] = "localhost";
    connInfo["PG_PORT"] = "5432" ;
    connInfo["PG_USER"] = "postgres";
    connInfo["PG_PASSWORD"] = "postgres";
    connInfo["PG_DB_NAME"] = "bdgcurso";
    connInfo["PG_CONNECT_TIMEOUT"] = "4";
    connInfo["PG_CLIENT_ENCODING"] = "CP1252";

    ds_ = te::da::DataSourceFactory::make("POSTGIS");

    // as we are going to use the data source, let´s set the connection info
    ds_->setConnectionInfo(connInfo);

    // let's open it with the connection info above!
    ds_->open();

    transactor_ = ds_->getTransactor();
    QVERIFY2(transactor_.get() != nullptr, "NULL transactor.");
}

void TestCollector::cleanup()
{
    ds_->close();
}

void TestCollector::testTiffCollector()
{
  int id = 0;
  std::string name = "Collector1";
  std::string description = "...";
  std::string type = "Observação";
//  terrama2::ws::collector::core::Format format = terrama2::ws::collector::core::Format::TIFF;
  int updateFreqMinutes = 10;
  std::string prefix = "tiff";
  int srid = 0;
  std::string mask= "tiff_%y_%m_%d";
  std::string unit="";
  std::string timeZone = "+00:00";
  QJsonObject dynamicMetadata;

//  terrama2::ws::collector::core::Collector* collector = new terrama2::ws::collector::core::Tiff(id, name, description, type, format, updateFreqMinutes, prefix, srid, mask, unit, timeZone, dynamicMetadata);

//  terrama2::ws::collector::core::CollectorDAO collectorDAO(transactor_);
//  QVERIFY2(collectorDAO.save(collector), "Fail to save.");
}


void TestCollector::initializeTerralib()
{
  // Initialize the Terralib support
  TerraLib::getInstance().initialize();

  te::plugin::PluginInfo* info;
  std::string plugins_path = te::common::FindInTerraLibPath("share/terralib/plugins");
  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.pgis.teplg");
  te::plugin::PluginManager::getInstance().add(info);
  te::plugin::PluginManager::getInstance().loadAll();
}



void TestCollector::finalizeTerralib()
{
  TerraLib::getInstance().finalize();
}

//QTEST_MAIN(TestCollector)
#include "TestCollector.moc"
