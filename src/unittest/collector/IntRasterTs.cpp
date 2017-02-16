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
  \file src/unittest/collector/IntRasterTs.cpp
  \brief Integration test for raster data
  \author Jano Simas
*/

#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/services/collector/core/Service.hpp>

#include "MockCollectorLogger.hpp"
#include "IntRasterTs.hpp"

#include <terrama2/Config.hpp>

#include <QString>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

void IntRasterTs::CollectAndCropRaster()
{
  QString json = QString::fromStdString("{\"Analysis\": [],"
                                         "\"DataSeries\": [{\"class\": \"DataSeries\",\"id\": 1,\"name\": \"Hidroestimador Amazonia_input\",\"description\": null,\"data_provider_id\": 2,\"semantics\": \"GRID-grads\",\"active\": true,\"datasets\": [{\"class\": \"DataSet\",\"id\": 1,\"data_series_id\": 1,\"active\": true,\"format\": {\"data_type\": \"INT16\",\"timezone\": \"0\",\"srid\": \"4326\",\"ctl_filename\": \"racc.ctl\",\"folder\": \"hidro\",\"temporal\": \"false\",\"bytes_after\": \"0\",\"bytes_before\": \"0\",\"value_multiplier\": \"1\",\"number_of_bands\": \"1\"}}]},{\"class\": \"DataSeries\",\"id\": 2,\"name\": \"Hidroestimador Amazonia\",\"description\": null,\"data_provider_id\": 1,\"semantics\": \"GRID-geotiff\",\"active\": true,\"datasets\": [{\"class\": \"DataSet\",\"id\": 2,\"data_series_id\": 2,\"active\": true,\"format\": {\"timestamp_property\": \"file_timestamp\",\"timezone\": \"0\",\"mask\": \"S10238225_%YYYY%MM%DD%hh%mm\",\"folder\": \"hidroestimador_crop\"}}]}],"
                                         "\"DataProviders\": [{\"class\": \"DataProvider\",\"id\": 1,\"project_id\": 1,\"data_provider_type\": \"FILE\",\"intent\": 1,\"name\": \"Dados locais\",\"description\": null,\"uri\": \"file://"+TERRAMA2_DATA_DIR+"\",\"active\": true},{\"class\": \"DataProvider\",\"id\": 2,\"project_id\": 1,\"data_provider_type\": \"FTP\",\"intent\": 1,\"name\": \"DSA curso\",\"description\": null,\"uri\": \"ftp://terrama:curso@server-ftpdsa.cptec.inpe.br:21/\",\"active\": true}],"
                                         "\"Collectors\": [{\"class\": \"Collector\",\"id\": 1,\"project_id\": 1,\"service_instance_id\": 1,\"input_data_series\": 1,\"output_data_series\": 2,\"input_output_map\": [{\"input\": 1,\"output\": 2}],\"schedule\": {\"class\": \"Schedule\",\"id\": 1,\"frequency\": 0,\"frequency_unit\": \"\",\"frequency_start_time\": \"\",\"schedule\": 0,\"schedule_time\": \"\",\"schedule_unit\": \"\",\"schedule_retry\": 0,\"schedule_retry_unit\": \"\",\"schedule_timeout\": 0,\"schedule_timeout_unit\": \"\"},\"filter\": {\"class\": \"Filter\",\"frequency\": null,\"frequency_unit\": null,\"discard_before\": \"2016-11-25T06:00:00.000-02:00\",\"discard_after\": \"2016-11-25T12:00:00.000-02:00\",\"region\": \"SRID=4326;POLYGON((-73.8036991603083 -9.81412714740936,-73.8036991603083 2.24662115728613,-56.097053202293 2.24662115728613,-56.097053202293 -9.81412714740936,-73.8036991603083 -9.81412714740936))\",\"by_value\": null,\"crop_raster\": true,\"collector_id\": 1},\"intersection\": {},\"active\": true}],"
                                         "\"Views\": []}");

  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
  te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);
  serviceManager.setLogConnectionInfo(uri);
  serviceManager.setInstanceId(1);

  auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();
  terrama2::services::collector::core::Service service(dataManager);

  auto loggerCopy = std::make_shared<terrama2::core::MockCollectorLogger>();

  EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));

  auto logger = std::make_shared<terrama2::core::MockCollectorLogger>();

  EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));

  logger->setConnectionInfo(uri);
  service.setLogger(logger);
  service.start();

  dataManager->addJSon(doc.object());

  service.addToQueue(1, terrama2::core::TimeUtils::nowUTC());

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(100000);
  QCoreApplication::exec();

  QDir testOutput(QString::fromStdString(TERRAMA2_DATA_DIR+"/hidroestimador_crop"));
  auto fileList = testOutput.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);

  QVERIFY2(12 == fileList.size(), "Wrong number of collected files.");
  for(const auto& fileInfo : fileList )
  {
    QFile output(fileInfo.absoluteFilePath());
    QFile reference(QString::fromStdString(TERRAMA2_DATA_DIR+"/hidroestimador_crop_reference/")+fileInfo.fileName());

    QVERIFY2(output.open(QIODevice::ReadOnly), "File not found.");
    QVERIFY2(reference.open(QIODevice::ReadOnly), "Reference file not found.");

    QVERIFY2(reference.readAll() == output.readAll(), "Collected file and reference file not equal.");
  }

  testOutput.removeRecursively();
}
