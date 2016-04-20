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
  \file terrama2/Exception.hpp

  \brief Base exception classes in TerraMA2.

  \author Gilberto Ribeiro de Queiroz
 */

// TerraMA2
#include <terrama2/core/Shared.hpp>

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/DataStoragerFactory.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/Collector.hpp>

#include <terrama2/impl/DataAccessorDcpInpe.hpp>
#include <terrama2/impl/DataAccessorDcpPostGIS.hpp>
#include <terrama2/impl/DataAccessorGeoTiff.hpp>
#include <terrama2/impl/DataAccessorOccurrenceMvf.hpp>
#include <terrama2/impl/DataAccessorOccurrencePostGis.hpp>
#include <terrama2/impl/DataAccessorStaticDataOGR.hpp>
#include <terrama2/impl/DataStoragerPostGis.hpp>

#include <terrama2/core/data-access/DataRetriever.hpp>

// STL
#include <memory>
#include <iostream>

// Qt
#include <QCoreApplication>
#include <QTimer>
#include <QUrl>

int main(int argc, char* argv[])
{
  try
  {
    terrama2::core::initializeTerraMA();

    terrama2::core::DataAccessorFactory::getInstance().add("DCP-inpe", terrama2::core::DataAccessorDcpInpe::make);
    terrama2::core::DataAccessorFactory::getInstance().add("DCP-postgis", terrama2::core::DataAccessorDcpPostGIS::make);
    terrama2::core::DataAccessorFactory::getInstance().add("GRID-geotiff", terrama2::core::DataAccessorGeoTiff::make);
    terrama2::core::DataAccessorFactory::getInstance().add("OCCURRENCE-mvf", terrama2::core::DataAccessorOccurrenceMvf::make);
    terrama2::core::DataAccessorFactory::getInstance().add("OCCURRENCE-postgis", terrama2::core::DataAccessorOccurrencePostGis::make);
    terrama2::core::DataAccessorFactory::getInstance().add("STATIC_DATA-ogr", terrama2::core::DataAccessorStaticDataOGR::make);

    terrama2::core::DataStoragerFactory::getInstance().add("POSTGIS", terrama2::core::DataStoragerPostGis::make);

    QCoreApplication app(argc, argv);

    auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();

    ///////////////////////////////////////////////
    //     input
    // DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    dataProvider->active = true;
    dataProvider->id = 1;
    dataProvider->name = "DataProvider queimadas local";
    dataProvider->dataProviderType = "FILE";

    dataManager->add(dataProviderPtr);

    // DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->id = 1;
    dataSeries->name = "DataProvider queimadas local";
    dataSeries->semantics.name = "OCCURRENCE-mvf";
    dataSeries->dataProviderId = dataProviderPtr->id;

    terrama2::core::DataSetOccurrence* dataSet = new terrama2::core::DataSetOccurrence();
    dataSet->id = 1;
    dataSet->active = true;
    dataSet->format.emplace("mask", "fires.csv");
    dataSet->format.emplace("timezone", "+00");
    dataSet->format.emplace("srid", "4326");

    dataSeries->datasetList.emplace_back(dataSet);

    dataManager->add(dataSeriesPtr);

    ///////////////////////////////////////////////
    //     output
    QUrl uri;
    uri.setScheme("postgis");
    uri.setHost("localhost");
    uri.setPort(5432);
    uri.setUserName("postgres");
    uri.setPassword("postgres");
    uri.setPath("/basedeteste");

    // DataProvider information
    terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
    outputDataProvider->id = 2;
    outputDataProvider->name = "DataProvider queimadas postgis";
    outputDataProvider->uri = uri.url().toStdString();
    outputDataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    outputDataProvider->dataProviderType = "POSTGIS";
    outputDataProvider->active = true;

    dataManager->add(outputDataProviderPtr);

    // DataSeries information
    terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
    outputDataSeries->id = 2;
    outputDataSeries->name = "DataProvider queimadas postgis";
    outputDataSeries->semantics.name = "OCCURRENCE-postgis";
    outputDataSeries->dataProviderId = outputDataProviderPtr->id;

    dataManager->add(outputDataSeriesPtr);

    // DataSet information
    terrama2::core::DataSetOccurrence* outputDataSet = new terrama2::core::DataSetOccurrence();
    outputDataSet->active = true;
    outputDataSet->id = 2;
    outputDataSet->format.emplace("table_name", "queimadas");

    outputDataSeries->datasetList.emplace_back(outputDataSet);

    terrama2::services::collector::core::Service service(dataManager);
    service.start();

    terrama2::services::collector::core::Collector* collector(new terrama2::services::collector::core::Collector());
    terrama2::services::collector::core::CollectorPtr collectorPtr(collector);
    collector->id = 1;
    collector->projectId = 1;
    collector->serviceInstanceId = 1;

    collector->inputDataSeries = 1;
    collector->outputDataSeries = 2;
    collector->inputOutputMap.emplace(1, 2);

    dataManager->add(collectorPtr);

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(30000);
    app.exec();

    service.stop();

    terrama2::core::finalizeTerraMA();
  }
  catch(...)
  {
    // TODO: o que fazer com uncaught exception
    std::cout << "\n\nException...\n" << std::endl;
  }

  return 0;
}
