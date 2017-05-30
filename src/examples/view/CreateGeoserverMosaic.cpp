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
  \file examples/view/CreateGeoServerMosaic.cpp

  \author Vinicius Campanha
 */

// TerraMA2
#include "ViewLoggerMock.hpp"

#include <terrama2/services/view/core/View.hpp>
#include <terrama2/services/view/core/DataManager.hpp>
#include <terrama2/services/view/core/Shared.hpp>
#include <terrama2/services/view/core/Utils.hpp>
#include <terrama2/services/view/core/Service.hpp>

#include <terrama2/core/Shared.hpp>

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>

#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/data-model/Filter.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>

#include <terrama2/impl/Utils.hpp>

// Qt
#include <QCoreApplication>
#include <QUrl>
#include <QTimer>

// STL
#include <memory>

using ::testing::_;

terrama2::core::DataProviderPtr inputDataProvider()
{
  // DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file:///" + TERRAMA2_DATA_DIR + "/umidade/";

  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;
  dataProvider->id = 1;
  dataProvider->name = "Local Geotiff";

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr inputDataSeries()
{
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  terrama2::core::DataSeries* dataSeries1 = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeries1Ptr(dataSeries1);
  dataSeries1->semantics = semanticsManager.getSemantics("GRID-gdal");
  dataSeries1->name = "Umin";
  dataSeries1->id = 1;
  dataSeries1->dataProviderId = 1;

  terrama2::core::DataSetGrid* dataSet1 = new terrama2::core::DataSetGrid();
  terrama2::core::DataSetPtr dataSetPtr(dataSet1);
  dataSet1->active = true;
  dataSet1->format.emplace("mask", "urmin_obs_ams_1km_%YYYY%MM%DD.tif");
  dataSet1->format.emplace("srid", "4326");
  dataSet1->format.emplace("folder", "");
  dataSet1->id = 1;
  dataSet1->dataSeriesId = dataSeries1->id;

  dataSeries1->datasetList.emplace_back(dataSetPtr);

  return dataSeries1Ptr;
}

terrama2::services::view::core::ViewPtr newView()
{
  auto view = new terrama2::services::view::core::View();
  terrama2::services::view::core::ViewPtr viewPtr(view);

  view->id = 1;
  view->active = true;
  view->projectId = 1;
  view->serviceInstanceId = 1;

  view->viewName = "MosaicExample";

  view->dataSeriesID = 1;
  view->filter = terrama2::core::Filter();

  return viewPtr;
}

int main(int argc, char** argv)
{

    QCoreApplication a(argc, argv);

    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    std::locale::global(std::locale::classic());

    terrama2::core::TerraMA2Init terramaRaii("example", 0);
    terrama2::core::registerFactories();

    auto dataManager = std::make_shared<terrama2::services::view::core::DataManager>();

    dataManager->add(inputDataProvider());
    dataManager->add(inputDataSeries());
    auto view = newView();
    dataManager->add(view);

    auto logger = std::make_shared<ViewLoggerMock>();
    ::testing::DefaultValue<RegisterId>::Set(1);
    EXPECT_CALL(*logger, setConnectionInfo(_)).Times(::testing::AtLeast(1));
    EXPECT_CALL(*logger, start(_)).WillRepeatedly(::testing::Return(1));
    EXPECT_CALL(*logger, result(_, _, _));
    EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(logger));
    te::core::URI connInfoURI("postgis://postgres:postgres@localhost:5432/terrama2");
    EXPECT_CALL(*logger, getConnectionInfo()).WillRepeatedly(::testing::Return(connInfoURI));
    EXPECT_CALL(*logger,setStartProcessingTime(_,_));
    EXPECT_CALL(*logger,setEndProcessingTime(_,_));

    logger->setConnectionInfo(te::core::URI());

    QJsonObject additionalIfo;
    additionalIfo.insert("maps_server", QString("http://admin:geoserver@localhost:8080/geoserver"));

    terrama2::core::ServiceManager::getInstance().setInstanceId(1);

    terrama2::services::view::core::Service service(dataManager);

    service.setLogger(logger);
    service.updateAdditionalInfo(additionalIfo);
    service.start();
    service.addToQueue(view->id, terrama2::core::TimeUtils::nowUTC());

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    a.exec();

    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(logger.get()));

  return 0;
}
