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

// TerraLib
#include <terralib/se.h>
#include <terralib/maptools/GroupingAlgorithms.h>
#include <terralib/fe.h>


// TerraMA2
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/impl/Utils.hpp>
#include "Utils.hpp"

#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/services/view/core/Service.hpp>
#include <terrama2/services/view/core/MemoryDataSetLayer.hpp>
#include <terrama2/services/view/core/View.hpp>


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
    view->imageName = "ExampleImage";
    view->imageType = te::map::ImageType(2);
    view->imageResolutionWidth = 1024;
    view->imageResolutionHeight = 768;

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

    view->stylesPerDataSeries.emplace(1, std::unique_ptr<te::se::Style>(MONO_0_Style()));

    view->stylesPerDataSeries.emplace(2, std::unique_ptr<te::se::Style>(CreateFeatureTypeStyle(te::gm::PolygonType, "#00c290")));

    view->stylesPerDataSeries.emplace(3, std::unique_ptr<te::se::Style>(CreateFeatureTypeStyle(te::gm::LineStringType, "#aff585")));

    dataManager->add(viewPtr);

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
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
