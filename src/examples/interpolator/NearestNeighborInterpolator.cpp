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
 *
 * \file interpolator/main.cpp
 *
 * \brief Example of interpolator usage.
 *
 * \author Frederico Augusto Bedê
*/

// TerraMA2
#include <terrama2/core/data-access/DataSetSeries.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/impl/Utils.hpp>

#include <terrama2/services/interpolator/core/DataManager.hpp>
#include <terrama2/services/interpolator/core/Interpolator.hpp>
#include <terrama2/services/interpolator/core/InterpolatorFactories.h>
#include <terrama2/services/interpolator/core/InterpolatorLogger.hpp>
#include <terrama2/services/interpolator/core/JSonUtils.hpp>
#include <terrama2/services/interpolator/core/Service.hpp>

#include <terrama2_config.hpp>

// Qt
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>


std::string GetNNJson()
{
  return std::string(R"(
     {
         "class": "Interpolator",
         "active": "true",
         "bounding_rect": {
            "ll_corner": [
                -46.79,
                -24.174
            ],
            "ur_corner": [
                -44.85,
                -23.355
            ]
         },
         "filter": "",
         "id": 8,
         "input_data_series": 4,
         "interpolation_attribute": "Pluvio",
         "interpolator_strategy": "NEAREST-NEIGHBOR",
         "output_data_series": 9,
         "project_id": 0,
         "resolution_x": 0.11,
         "resolution_y": 0.11,
         "service_instance_id": 1,
         "srid": 4326,
         "number_of_neighbors": 2
     }
    )");
}

std::string GetAvgJson()
{
  return std::string(R"(
     {
         "class": "Interpolator",
         "active": "true",
         "bounding_rect": {
            "ll_corner": [
                -46.79,
                -24.174
            ],
            "ur_corner": [
                -44.85,
                -23.355
            ]
         },
         "filter": "",
         "id": 9,
         "input_data_series": 4,
         "interpolation_attribute": "Pluvio",
         "interpolator_strategy": "AVERAGE-NEIGHBOR",
         "output_data_series": 555,
         "project_id": 0,
         "resolution_x": 0.11,
         "resolution_y": 0.11,
         "service_instance_id": 1,
         "srid": 4326,
         "number_of_neighbors": 2
     }
    )");
}

std::string GetSqrAvgJson()
{
  return std::string(R"(
     {
         "class": "Interpolator",
         "active": "true",
         "bounding_rect": {
            "ll_corner": [
                -46.79,
                -24.174
            ],
            "ur_corner": [
                -44.85,
                -23.355
            ]
         },
         "filter": "",
         "id": 10,
         "input_data_series": 4,
         "interpolation_attribute": "Pluvio",
         "interpolator_strategy": "W-AVERAGE-NEIGHBOR",
         "output_data_series": 556,
         "project_id": 0,
         "resolution_x": 0.11,
         "resolution_y": 0.11,
         "service_instance_id": 1,
         "srid": 4326,
         "number_of_neighbors": 2,
         "power_factor": 3
     }
    )");
}


int main(int argc, char* argv[])
{
  //////////////////////////////////////////////////////
  /// Initializing TerraMA2.

  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  terrama2::core::registerFactories();
  //////////////////////////////////////////////////////

  QCoreApplication app(argc, argv);

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
  auto dataManager = std::make_shared<terrama2::services::interpolator::core::DataManager>();

  //////////////////////////////////////////////////////
  /// Starts the service and adds the interpolator.
  //////////////////////////////////////////////////////

  terrama2::services::interpolator::core::Service service(dataManager);
  serviceManager.setInstanceId(1);

  te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);

  auto logger = std::make_shared<terrama2::services::interpolator::core::InterpolatorLogger>();
  logger->setConnectionInfo(te::core::URI(uri));
  service.setLogger(logger);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(uri);

  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  // Registering input data provider
  terrama2::core::DataProvider* inputDataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr inputDataProviderPtr(inputDataProvider);
  inputDataProvider->id = 2;
  inputDataProvider->name = "DataProvider postgis";
  inputDataProvider->uri = uri.uri();
  inputDataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  inputDataProvider->dataProviderType = "POSTGIS";
  inputDataProvider->active = true;

  dataManager->add(inputDataProviderPtr);
  //////////////////////////////////////////////////////

  //////////////////////////////////////////////////////
  ///     Output
  /// Local folder - Interpolation output server.
  /// DataProvider information
  //////////////////////////////////////////////////////

  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->id = 3;
  dataProvider->name = "Servidor de Saída";
  dataProvider->uri = "file://" + std::string(TERRAMA2_DATA_DIR) + "/interpolation";
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;

  dataManager->add(dataProviderPtr);

  // DataSeries information - Nearest-neighbor
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries;
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->id = 9;
  dataSeries->name = "Interpolation nn";
  dataSeries->semantics = semanticsManager.getSemantics("GRID-geotiff");
  dataSeries->dataProviderId = dataProvider->id;
  dataSeries->active = true;

  terrama2::core::DataSet* dset = new terrama2::core::DataSetGrid;
  std::shared_ptr<terrama2::core::DataSet> dsetPtr(dset);

  dsetPtr->active = true;
  dsetPtr->dataSeriesId = dataSeries->id;
  dsetPtr->id = 10;
  dsetPtr->format.emplace("mask", "nn_%YYYY%MM%DD%hh%mm.tif");
  dsetPtr->format.emplace("timezone", "UTC+00");

  dataSeries->datasetList.emplace_back(dsetPtr);
  dataManager->add(dataSeriesPtr);


  // DataSeries information - Average-neighbor
  terrama2::core::DataSeries* dataSeries2 = new terrama2::core::DataSeries;
  terrama2::core::DataSeriesPtr dataSeriesPtr2(dataSeries2);
  dataSeries2->id = 555;
  dataSeries2->name = "Interpolation avg";
  dataSeries2->semantics = semanticsManager.getSemantics("GRID-geotiff");
  dataSeries2->dataProviderId = dataProvider->id;
  dataSeries2->active = true;

  terrama2::core::DataSet* dset2 = new terrama2::core::DataSetGrid;
  std::shared_ptr<terrama2::core::DataSet> dsetPtr2(dset2);

  dsetPtr2->active = true;
  dsetPtr2->dataSeriesId = dataSeries2->id;
  dsetPtr2->id = 111;
  dsetPtr2->format.emplace("mask", "avg_%YYYY%MM%DD%hh%mm.tif");
  dsetPtr2->format.emplace("timezone", "UTC+00");

  dataSeries2->datasetList.emplace_back(dsetPtr2);
  dataManager->add(dataSeriesPtr2);

  // DataSeries information - Square distance average-neighbor
  terrama2::core::DataSeries* dataSeries3 = new terrama2::core::DataSeries;
  terrama2::core::DataSeriesPtr dataSeriesPtr3(dataSeries3);
  dataSeries3->id = 556;
  dataSeries3->name = "Interpolation sqr-avg";
  dataSeries3->semantics = semanticsManager.getSemantics("GRID-geotiff");
  dataSeries3->dataProviderId = dataProvider->id;
  dataSeries3->active = true;

  terrama2::core::DataSet* dset3 = new terrama2::core::DataSetGrid;
  std::shared_ptr<terrama2::core::DataSet> dsetPtr3(dset3);

  dsetPtr3->active = true;
  dsetPtr3->dataSeriesId = dataSeries3->id;
  dsetPtr3->id = 112;
  dsetPtr3->format.emplace("mask", "sqr_avg_%YYYY%MM%DD%hh%mm.tif");
  dsetPtr3->format.emplace("timezone", "UTC+00");

  dataSeries3->datasetList.emplace_back(dsetPtr3);
  dataManager->add(dataSeriesPtr3);
  //////////////////////////////////////////////////////

  //////////////////////////////////////////////////////
  ///     Input
  /// PostGIS database - Interpolation input server.
  /// DataProvider information
  //////////////////////////////////////////////////////

  // PCD-1
  terrama2::core::DataSeries* pcdSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr pcdSeriesPtr(pcdSeries);
  pcdSeries->dataProviderId = inputDataProvider->id;
  pcdSeries->semantics = semanticsManager.getSemantics("DCP-postgis");
  pcdSeries->name = "Monitored PCD";
  pcdSeries->id = 4;
  pcdSeries->active = true;

  terrama2::core::DataSetDcp* dataSetPcd1 = new terrama2::core::DataSetDcp{};
  terrama2::core::DataSetPtr dataSetPcd1Ptr(dataSetPcd1);
  dataSetPcd1->active = true;
  dataSetPcd1->format.emplace("timestamp_property", "datetime");
  dataSetPcd1->format.emplace("alias", "itanhaem");
  dataSetPcd1->format.emplace("table_name", "itanhaem");
  dataSetPcd1->format.emplace("timezone", "0");
  dataSetPcd1->id = 5;
  dataSetPcd1->dataSeriesId = pcdSeries->id;
  dataSetPcd1->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-46.79, -24.174, 4326, te::gm::PointType,  nullptr));
  pcdSeries->datasetList.push_back(dataSetPcd1Ptr);
  // ------------------------------------------------------------------------

  // PCD-2
  terrama2::core::DataSetDcp* dataSetPcd2 = new terrama2::core::DataSetDcp{};
  terrama2::core::DataSetPtr dataSetPcd2Ptr(dataSetPcd2);
  dataSetPcd2->active = true;
  dataSetPcd2->format.emplace("timestamp_property", "datetime");
  dataSetPcd2->format.emplace("alias", "paraibuna");
  dataSetPcd2->format.emplace("table_name", "paraibuna");
  dataSetPcd2->format.emplace("timezone", "0");
  dataSetPcd2->id = 6;
  dataSetPcd2->dataSeriesId = pcdSeries->id;
  dataSetPcd2->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-45.6, -23.408, 4326, te::gm::PointType,  nullptr));
  pcdSeries->datasetList.push_back(dataSetPcd2Ptr);
  // ------------------------------------------------------------------------

  // PCD-3
  terrama2::core::DataSetDcp* dataSetPcd3 = new terrama2::core::DataSetDcp{};
  terrama2::core::DataSetPtr dataSetPcd3Ptr(dataSetPcd3);
  dataSetPcd3->active = true;
  dataSetPcd3->format.emplace("timestamp_property", "datetime");
  dataSetPcd3->format.emplace("alias", "picinguaba");
  dataSetPcd3->format.emplace("table_name", "picinguaba");
  dataSetPcd3->format.emplace("timezone", "0");
  dataSetPcd3->id = 7;
  dataSetPcd3->dataSeriesId = pcdSeries->id;
  dataSetPcd3->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-44.85, -23.355, 4326, te::gm::PointType,  nullptr));
  pcdSeries->datasetList.push_back(dataSetPcd3Ptr);

  dataManager->add(pcdSeriesPtr);
  // ------------------------------------------------------------------------
  //////////////////////////////////////////////////////

  //////////////////////////////////////////////////////
  ///     Interpolator
  /// Creating services by nearest-neighbor and
  /// starts the service.
  //////////////////////////////////////////////////////
  ///

  QJsonDocument doc1 = QJsonDocument::fromJson(QString::fromStdString(GetNNJson()).toUtf8());
  QJsonDocument doc2 = QJsonDocument::fromJson(QString::fromStdString(GetAvgJson()).toUtf8());
  QJsonDocument doc3 = QJsonDocument::fromJson(QString::fromStdString(GetSqrAvgJson()).toUtf8());

  terrama2::services::interpolator::core::InterpolatorParamsPtr pPtr1(terrama2::services::interpolator::core::fromInterpolatorJson(doc1.object(), dataManager.get()));
  terrama2::services::interpolator::core::InterpolatorParamsPtr pPtr2(terrama2::services::interpolator::core::fromInterpolatorJson(doc2.object(), dataManager.get()));
  terrama2::services::interpolator::core::InterpolatorParamsPtr pPtr3(terrama2::services::interpolator::core::fromInterpolatorJson(doc3.object(), dataManager.get()));

  dataManager->add(pPtr1);
  dataManager->add(pPtr2);
  dataManager->add(pPtr3);

  service.start();
  service.addToQueue(pPtr1->id_, terrama2::core::TimeUtils::nowUTC());
  service.addToQueue(pPtr2->id_, terrama2::core::TimeUtils::nowUTC());
  service.addToQueue(pPtr3->id_, terrama2::core::TimeUtils::nowUTC());
  //////////////////////////////////////////////////////

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);

  app.exec();

  service.stopService();

  return 0;
}
