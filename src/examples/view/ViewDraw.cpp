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
  \file examples/view/ViewDraw.cpp

  \brief

  \author Vinicius Campanha
 */

// Qt
#include <QApplication>
#include <QTimer>

// TerraMA2
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/impl/Utils.hpp>

#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/services/view/core/Service.hpp>
#include <terrama2/services/view/core/DataManager.hpp>
#include <terrama2/services/view/core/MemoryDataSetLayer.hpp>
#include <terrama2/services/view/core/View.hpp>


void prepareExample(std::shared_ptr<terrama2::services::view::core::DataManager> dataManager)
{
  //DataProvider information
  terrama2::core::DataProvider* dataProviderRaster = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderRasterPtr(dataProviderRaster);
  dataProviderRaster->id = 1;
  dataProviderRaster->name = "dataProviderRaster";
  dataProviderRaster->uri = "file://";
  dataProviderRaster->uri += TERRAMA2_DATA_DIR;
  dataProviderRaster->uri += "/geotiff";

  dataProviderRaster->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProviderRaster->dataProviderType = "FILE";
  dataProviderRaster->active = true;

  dataManager->add(dataProviderRasterPtr);

  //DataSeries information
  terrama2::core::DataSeries* dataSeriesRaster = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesRasterPtr(dataSeriesRaster);
  dataSeriesRaster->id = 1;
  dataSeriesRaster->name = "dataSeriesRaster";
  dataSeriesRaster->dataProviderId = 1;
  dataSeriesRaster->semantics.code = "GRID-geotiff";

  terrama2::core::DataSetGrid* dataSetRaster = new terrama2::core::DataSetGrid();
  dataSetRaster->active = true;
  dataSetRaster->format.emplace("mask", "Spot_Vegetacao_Jul2001_SP.tif");
  dataSetRaster->format.emplace("timezone", "-03");

  dataSeriesRaster->datasetList.emplace_back(dataSetRaster);

  dataManager->add(dataSeriesRasterPtr);

  //DataProvider information
  terrama2::core::DataProvider* dataProviderGeometry = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderGeometryPtr(dataProviderGeometry);
  dataProviderGeometry->id = 2;
  dataProviderGeometry->name = "dataProviderGeometry";
  dataProviderGeometry->uri = "file://";
  dataProviderGeometry->uri += TERRAMA2_DATA_DIR;
  dataProviderGeometry->uri += "/shapefile";

  dataProviderGeometry->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProviderGeometry->dataProviderType = "FILE";
  dataProviderGeometry->active = true;

  dataManager->add(dataProviderGeometryPtr);

  //DataSeries information
  terrama2::core::DataSeries* dataSeriesGeometry = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesGeometryPtr(dataSeriesGeometry);
  dataSeriesGeometry->id = 2;
  dataSeriesGeometry->name = "dataSeriesGeometry";
  dataSeriesGeometry->dataProviderId = 2;
  dataSeriesGeometry->semantics.code = "STATIC_DATA-ogr";

  terrama2::core::DataSetGrid* dataSetGeometry = new terrama2::core::DataSetGrid();
  dataSetGeometry->active = true;
  dataSetGeometry->format.emplace("mask", "35MUE250GC_SIR.shp");
  dataSetGeometry->format.emplace("timezone", "-03");

  dataSeriesGeometry->datasetList.emplace_back(dataSetGeometry);

  dataManager->add(dataSeriesGeometryPtr);

  //DataSeries information
  terrama2::core::DataSeries* dataSeriesGeometry2 = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesGeometry2Ptr(dataSeriesGeometry2);
  dataSeriesGeometry2->id = 3;
  dataSeriesGeometry2->name = "dataSeriesGeometry2";
  dataSeriesGeometry2->dataProviderId = 2;
  dataSeriesGeometry2->semantics.code = "STATIC_DATA-ogr";

  terrama2::core::DataSetGrid* dataSetGeometry2 = new terrama2::core::DataSetGrid();
  dataSetGeometry2->active = true;
  dataSetGeometry2->format.emplace("mask", "Rod_Principais_SP_lin.shp");
  dataSetGeometry2->format.emplace("timezone", "-03");
  dataSetGeometry2->format.emplace("srid", "4326");

  dataSeriesGeometry2->datasetList.emplace_back(dataSetGeometry2);

  dataManager->add(dataSeriesGeometry2Ptr);
}

int main(int argc, char** argv)
{
  terrama2::core::initializeTerraMA();
  terrama2::core::registerFactories();

  QApplication app(argc, argv);

  try
  {
    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    std::map<std::string, std::string> connInfo { {"PG_HOST", TERRAMA2_DATABASE_HOST},
                                                  {"PG_PORT", TERRAMA2_DATABASE_PORT},
                                                  {"PG_USER", TERRAMA2_DATABASE_USERNAME},
                                                  {"PG_PASSWORD", TERRAMA2_DATABASE_PASSWORD},
                                                  {"PG_DB_NAME", TERRAMA2_DATABASE_DBNAME},
                                                  {"PG_CONNECT_TIMEOUT", "4"},
                                                  {"PG_CLIENT_ENCODING", "UTF-8"}
                                                };
    serviceManager.setLogConnectionInfo(connInfo);
    serviceManager.setInstanceId(1);

    std::shared_ptr<terrama2::services::view::core::DataManager> dataManager = std::make_shared<terrama2::services::view::core::DataManager>();

    prepareExample(dataManager);

    terrama2::services::view::core::Service service(dataManager);
    service.start();

    terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
    terrama2::services::view::core::ViewPtr viewPtr(view);

    view->id = 1;
    view->projectId = 1;
    view->serviceInstanceId = 1;
    view->active = true;
    view->resolutionWidth = 800;
    view->resolutionHeight = 600;

    terrama2::core::Schedule schedule;
    schedule.id = 1;
    schedule.frequency = 2;
    schedule.frequencyUnit = "min";

    view->schedule = schedule;

    view->dataSeriesList.push_back(1);
    view->dataSeriesList.push_back(2);
    view->dataSeriesList.push_back(3);

    terrama2::core::Filter filter;

    view->filtersPerDataSeries.emplace(1, filter);
    view->filtersPerDataSeries.emplace(2, filter);
    view->filtersPerDataSeries.emplace(3, filter);

    dataManager->add(viewPtr);

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(20000);
    app.exec();

    service.stopService();
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in DrawLayer example: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in DrawLayer example!" << std::endl;
    return EXIT_FAILURE;
  }

  terrama2::core::finalizeTerraMA();

  return EXIT_SUCCESS;
}
