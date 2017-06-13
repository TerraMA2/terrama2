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
#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/core/utility/CurlWrapperFtp.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/Utils.hpp>


#include "MockCollectorLogger.hpp"
#include "IntRasterTs.hpp"

#include <terrama2/Config.hpp>

#include <QString>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>

void addInput(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
  ///////////////////////////////////////////////
  //     input
  // DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->id = 2;
  dataProvider->name = "DSA curso";
  dataProvider->uri = "ftp://ftp:JenkinsD%40t%40@jenkins-ftp.dpi.inpe.br:21/terrama2/";
  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FTP";
  dataProvider->active = true;

  dataManager->add(dataProviderPtr);

  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  // DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->id = 1;
  dataSeries->name = "Hidroestimador Amazonia_input";
  dataSeries->semantics = semanticsManager.getSemantics("GRID-grads");
  dataSeries->dataProviderId = dataProviderPtr->id;
  dataSeries->active = true;

  terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
  dataSet->id = 1;
  dataSet->active = true;
  dataSet->format.emplace("ctl_filename", "/grads/racc.ctl");
  dataSet->format.emplace("folder", "hidro");
  dataSet->format.emplace("srid", "4326");
  dataSet->format.emplace("timezone", "UTC+00");
  dataSet->format.emplace("data_type", "INT16");
  dataSet->format.emplace("number_of_bands", "1");
  dataSet->format.emplace("bytes_before", "0");
  dataSet->format.emplace("bytes_after", "0");
  dataSet->format.emplace("value_multiplier", "1");
  dataSet->format.emplace("temporal", "false");

  dataSeries->datasetList.emplace_back(dataSet);

  dataManager->add(dataSeriesPtr);
}

void addOutput(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
  ///////////////////////////////////////////////
  //     output

  // DataProvider information
  terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
  outputDataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/";

  outputDataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  outputDataProvider->active = true;
  outputDataProvider->id = 1;
  outputDataProvider->name = "Dados locais";
  outputDataProvider->dataProviderType = "FILE";
  outputDataProvider->active = true;

  dataManager->add(outputDataProviderPtr);

  // DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
  outputDataSeries->id = 2;
  outputDataSeries->name = "Hidroestimador Amazonia";
  outputDataSeries->active = true;
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  outputDataSeries->semantics = semanticsManager.getSemantics("GRID-geotiff");
  outputDataSeries->dataProviderId = outputDataProviderPtr->id;


  // DataSet information
  terrama2::core::DataSetGrid* outputDataSet = new terrama2::core::DataSetGrid();
  outputDataSet->active = true;
  outputDataSet->id = 2;
  outputDataSet->dataSeriesId = outputDataSeries->id;
  outputDataSet->format.emplace("mask", "/hidroestimador_crop/S10238225_%YYYY%MM%DD%hh%mm");
  outputDataSet->format.emplace("timestamp_property", "file_timestamp");
  outputDataSet->format.emplace("timezone", "0");

  outputDataSeries->datasetList.emplace_back(outputDataSet);

  dataManager->add(outputDataSeriesPtr);
}

void addCollector(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
  terrama2::services::collector::core::Collector* collector(new terrama2::services::collector::core::Collector());
  terrama2::services::collector::core::CollectorPtr collectorPtr(collector);
  collector->id = 1;
  collector->projectId = 1;
  collector->serviceInstanceId = 1;
  collector->active = true;

  collector->filter.region = terrama2::core::ewktToGeom("SRID=4326;POLYGON((-73.8036991603083 -9.81412714740936,-73.8036991603083 2.24662115728613,-56.097053202293 2.24662115728613,-56.097053202293 -9.81412714740936,-73.8036991603083 -9.81412714740936))");
  collector->filter.cropRaster = true;
  collector->filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-11-25T06:00:00.000-02:00", terrama2::core::TimeUtils::webgui_timefacet);
  collector->filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-11-25T12:00:00.000-02:00", terrama2::core::TimeUtils::webgui_timefacet);

  collector->inputDataSeries = 1;
  collector->outputDataSeries = 2;
  collector->inputOutputMap.emplace(1, 2);

  dataManager->add(collectorPtr);
}
size_t write_file_names(void* ptr, size_t size, size_t nmemb, void* data)
{
  size_t sizeRead = size * nmemb;

  std::string* block = (std::string*) data;
  block->append((char*)ptr, sizeRead);

  return sizeRead;
}

size_t write_response(void* ptr, size_t size, size_t nmemb, void* data)
{
  FILE* writehere = (FILE*)data;
  return fwrite(ptr, size, nmemb, writehere);
}

void downloadReferenceFiles()
{
  terrama2::core::CurlWrapperFtp curl;
  std::string referenceUrl = "ftp://ftp:JenkinsD%40t%40@jenkins-ftp.dpi.inpe.br:21/terrama2/reference_data/";

  try
  {
    curl.verifyURL(referenceUrl, 8);
  }
  catch(...)
  {
    QFAIL("FTP address is invalid.");
  }

  std::string outDir = TERRAMA2_DATA_DIR+"/hidroestimador_crop_reference";
  QDir dir(QString::fromStdString(outDir));
  if(!dir.mkpath(QString::fromStdString(outDir)))
    QFAIL("Unable to create reference folder.");

  std::vector<std::string> vectorFiles = curl.listFiles(te::core::URI(referenceUrl));
  for(const auto& file : vectorFiles)
  {
    std::string fileUri = referenceUrl + file;

    std::string filePath = outDir + "/" + file;
    curl.downloadFile(fileUri, filePath);
  }
}

void IntRasterTs::CollectAndCropRaster()
{

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
  te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);
  serviceManager.setLogConnectionInfo(uri);
  serviceManager.setInstanceId(1);

  auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();
  addInput(dataManager);
  addOutput(dataManager);
  addCollector(dataManager);

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

  service.addToQueue(1, terrama2::core::TimeUtils::nowUTC());

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  QCoreApplication::exec();

  downloadReferenceFiles();

  QDir testOutput(QString::fromStdString(TERRAMA2_DATA_DIR+"/hidroestimador_crop/"));
  auto fileList = testOutput.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);

  QDir testReference(QString::fromStdString(TERRAMA2_DATA_DIR+"/hidroestimador_crop_reference/"));

  QVERIFY2(5 == fileList.size(), "Wrong number of collected files.");
  for(const auto& fileInfo : fileList )
  {
    QImage output(fileInfo.absoluteFilePath());
    QImage reference(testReference.absoluteFilePath(fileInfo.fileName()));

    QString errMsg3 = QString("Collected file and reference file not equal.\n%1").arg(fileInfo.fileName());
    QVERIFY2(reference == output, errMsg3.toUtf8());
  }

   testOutput.removeRecursively();
   testReference.removeRecursively();
}
