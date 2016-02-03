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
  \file terrama2/collector/TsIntegration.cpp

  \brief Integrated tests for collector module.

  \author Jano Simas
*/

#include "TsIntegration.hpp"

//terrama
#include <terrama2/collector/CollectorService.hpp>
#include <terrama2/collector/Exception.hpp>

#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/Utils.hpp>

//terralib
#include <terralib/datatype/TimeDuration.h>
#include <terralib/common/Exception.h>

//QT
#include <QTimer>

void TsIntegration::TestReadCsvStorePostGis()
{
  QTemporaryDir dir;
  QTemporaryFile file(dir.path()+"/test_XXXXXX.csv");
  file.open();
  file.write("lat,lon,sat,data_pas\n");
  file.write("-10.7030,  30.3750,AQUA_M,2015-08-26 11:35:00\n");
  file.write("-10.7020,  30.3840,AQUA_M,2015-08-26 11:35:00\n");
  file.write("-10.4870,  30.4070,AQUA_M,2015-08-26 11:35:00\n");
  file.close();
  QFileInfo info(file);

  QUrl url = QUrl::fromLocalFile(info.canonicalPath());

  try
  {
    terrama2::core::DataProvider provider("dummy", terrama2::core::DataProvider::FILE_TYPE);
    provider.setStatus(terrama2::core::DataProvider::ACTIVE);
    provider.setUri(url.url().toStdString());

    terrama2::core::DataSet dataset("dummy", terrama2::core::DataSet::OCCURENCE_TYPE);
    dataset.setStatus(terrama2::core::DataSet::ACTIVE);

    te::dt::TimeDuration frequency(0, 50, 0);
    dataset.setDataFrequency(frequency);

    terrama2::core::DataSetItem item(terrama2::core::DataSetItem::FIRE_POINTS_TYPE);
    // item.setMask("exporta_20151101_0130.csv"); // adiciona o nome do arquivo
   // item.setMask("exporta_%A%M%d_%h%m.csv");
    item.setMask(info.fileName().toStdString());

    std::map<std::string, std::string> metadata{ {"KIND", "POSTGIS"},
                                                        {"PG_HOST", "localhost"},
                                                        {"PG_PORT", "5432"},
                                                        {"PG_USER", "postgres"},
                                                        {"PG_PASSWORD", "postgres"},
                                                        {"PG_DB_NAME", "basedeteste"},
                                                        {"PG_CONNECT_TIMEOUT", "4"},
                                                        {"PG_CLIENT_ENCODING", "UTF-8"},
                                                        {"PG_TABLENAME", "terrama2.integrationTest"} };
    item.setMetadata(metadata);

    terrama2::collector::CollectorService service;
    service.start(1);

    std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/collector/data/project.json");
    QJsonDocument jdoc = terrama2::core::ReadJsonFile(path);
    QJsonObject project = jdoc.object();
    QCOMPARE(terrama2::core::ApplicationController::getInstance().loadProject(project), true);

    auto& dataManager = terrama2::core::DataManager::getInstance();
    dataManager.add(provider);
    provider.add(dataset);

    dataset.add(item);
    dataManager.add(dataset);

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
    timer.start(15000);

    QApplication::exec();

    dataManager.removeDataProvider(provider.id());
  }
  catch(terrama2::Exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL("Terrama2 exception...");
  }
  catch(te::common::Exception& e)
  {
    qDebug() << e.what();
    QFAIL("Terralib exception...");
  }
  catch(std::exception& e)
  {
    qDebug() << e.what();
    QFAIL("Std exception...");
  }
}

void TsIntegration::TestCollectorFTP()
{
  QUrl url;
  // PCD INPE
/*
  url.setHost("200.137.4.175");
  url.setPath("/pcd/");
  url.setScheme("FTP");
  url.setPort(21);
  url.setUserName("ddpi");
  url.setPassword("mtv.0789");
*/
  // Fire
  url.setHost("ftp.dgi.inpe.br");
  url.setPath("/focos_operacao/");
  url.setScheme("FTP");
  url.setPort(21);
  url.setUserName("queimadas");
  url.setPassword("inpe_2012");
/*
  // Serie Grade CPTEC INPE PREC_ETA15km
 // url.setHost("ftp://ftp1.cptec.inpe.br"); // gera erro
  url.setHost("ftp1.cptec.inpe.br");
  url.setPath("/modelos/");
  //url.setPath("/modelos/io/tempo/regional/Eta15km/sismaden/Dir_10m/");
  url.setScheme("FTP");
  url.setPort(21);
  url.setUserName("anonymous");
  url.setPassword("");
*/
  try
  {
    terrama2::core::DataProvider provider("dummy", terrama2::core::DataProvider::FTP_TYPE);
    provider.setStatus(terrama2::core::DataProvider::ACTIVE);
    provider.setUri(url.url().toStdString());

    terrama2::core::DataSet dataset("dummy", terrama2::core::DataSet::OCCURENCE_TYPE);
    dataset.setStatus(terrama2::core::DataSet::ACTIVE);

    te::dt::TimeDuration frequency(1, 0, 0);
    dataset.setDataFrequency(frequency);

    terrama2::core::DataSetItem item(terrama2::core::DataSetItem::FIRE_POINTS_TYPE);

    item.setStatus(terrama2::core::DataSetItem::ACTIVE);

    item.setMask("exporta_%A%M%d_%h%m.csv");

//    std::map<std::string, std::string> metadata{ {"KIND", "POSTGIS"},
//                                                        {"PG_HOST", "localhost"},
//                                                        {"PG_PORT", "5432"},
//                                                        {"PG_USER", "postgres"},
//                                                        {"PG_PASSWORD", "postgres"},
//                                                        {"PG_DB_NAME", "baseteste"},
//                                                        {"PG_CONNECT_TIMEOUT", "4"},
//                                                        {"PG_CLIENT_ENCODING", "UTF-8"},
//                                                        {"PG_TABLENAME", "terrama2.data_provider"} };
//    item.setMetadata(metadata);

    terrama2::collector::CollectorService service;
    service.start(1);

    std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/collector/data/project.json");
    QJsonDocument jdoc = terrama2::core::ReadJsonFile(path);
    QJsonObject project = jdoc.object();
    QCOMPARE(terrama2::core::ApplicationController::getInstance().loadProject(project), true);

    auto& dataManager = terrama2::core::DataManager::getInstance();
    dataManager.add(provider);
    provider.add(dataset);

    dataset.add(item);
    dataManager.add(dataset);

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
    timer.start(15000);

    QApplication::exec();

    dataManager.removeDataProvider(provider.id());
  }
  catch(terrama2::Exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL("Terrama2 exception...");
  }
  catch(te::common::Exception& e)
  {
    qDebug() << e.what();
    QFAIL("Terralib exception...");
  }
  catch(std::exception& e)
  {
    qDebug() << e.what();
    QFAIL("Std exception...");
  }
}


void TsIntegration::TestReadPostgisStorePostGis()
{

  QUrl uri;
  uri.setScheme("postgis");
  uri.setHost("localhost");
  uri.setPort(5432);
  uri.setUserName("postgres");
  uri.setPassword("postgres");
  uri.setPath("/basedeteste");

  try
  {
    terrama2::core::DataProvider provider("dummy", terrama2::core::DataProvider::POSTGIS_TYPE);
    provider.setStatus(terrama2::core::DataProvider::ACTIVE);
    provider.setUri(uri.url().toStdString());

    terrama2::core::DataSet dataset("dummy", terrama2::core::DataSet::OCCURENCE_TYPE);
    dataset.setStatus(terrama2::core::DataSet::ACTIVE);

    te::dt::TimeDuration frequency(0, 0, 5);
    dataset.setDataFrequency(frequency);

    terrama2::core::DataSetItem item(terrama2::core::DataSetItem::FIRE_POINTS_TYPE);

    std::map<std::string, std::string> metadata{ {"KIND", "POSTGIS"},
                                                        {"PG_HOST", "localhost"},
                                                        {"PG_PORT", "5432"},
                                                        {"PG_USER", "postgres"},
                                                        {"PG_PASSWORD", "postgres"},
                                                        {"PG_DB_NAME", "basedeteste"},
                                                        {"PG_CONNECT_TIMEOUT", "4"},
                                                        {"PG_CLIENT_ENCODING", "UTF-8"},
                                                        {"PG_TABLENAME", "terrama2.teste_postgis"} };
    item.setMetadata(metadata);
    item.setMask("terrama2.nome_teste");

    terrama2::collector::CollectorService service;
    service.start();

    std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/collector/data/project.json");
    QJsonDocument jdoc = terrama2::core::ReadJsonFile(path);
    QJsonObject project = jdoc.object();
    QCOMPARE(terrama2::core::ApplicationController::getInstance().loadProject(project), true);

    auto& dataManager = terrama2::core::DataManager::getInstance();
    dataManager.add(provider);
    provider.add(dataset);

    dataset.add(item);
    dataManager.add(dataset);

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
    timer.start(15000);

    QApplication::exec();

    dataManager.removeDataProvider(provider.id());
  }
  catch(terrama2::Exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL("Terrama2 exception...");
  }
  catch(te::common::Exception& e)
  {
    qDebug() << e.what();
    QFAIL("Terralib exception...");
  }
  catch(std::exception& e)
  {
    qDebug() << e.what();
    QFAIL("Std exception...");
  }
}


