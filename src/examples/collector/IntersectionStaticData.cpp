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
  \file examples/collector/IntersectionStaticData.Cpp

  \brief Example of the use of intersection to add the attribute 'state' to occurrence data series.

  \author Paulo R. M. Oliveira
 */

// TerraMA2
#include <terrama2/core/Shared.hpp>

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/DataStoragerFactory.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/core/Intersection.hpp>
#include <terrama2/services/collector/core/JSonUtils.hpp>

#include <terrama2/impl/Utils.hpp>

#include <terrama2/core/data-access/DataRetriever.hpp>

// STL
#include <memory>
#include <iostream>

// Qt
#include <QCoreApplication>
#include <QTimer>
#include <QUrl>

void addStaticDataSeries(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  std::shared_ptr<const terrama2::core::DataProvider> dataProviderPtr(dataProvider);
  dataProvider->name = "Provider";
  dataProvider->uri += TERRAMA2_DATA_DIR;
  dataProvider->uri += "/shapefile";
  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;
  dataProvider->id = 3;

  dataManager->add(dataProviderPtr);

  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->dataProviderId = dataProvider->id;
  dataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");
  dataSeries->semantics.dataSeriesType = terrama2::core::DataSeriesType::GEOMETRIC_OBJECT;
  dataSeries->name = "States Brazil";
  dataSeries->id = 3;
  dataSeries->dataProviderId = dataProvider->id;

  //DataSet information
  terrama2::core::DataSet* dataSet = new terrama2::core::DataSet;
  terrama2::core::DataSetPtr dataSetPtr(dataSet);
  dataSet->active = true;
  dataSet->format.emplace("mask", "estados_2010.shp");
  dataSet->format.emplace("srid", "4326");
  dataSet->format.emplace("identifier", "nome");
  dataSet->id = 1;
  dataSet->dataSeriesId = dataSeries->id;

  dataSeries->datasetList.push_back(dataSetPtr);
  dataManager->add(dataSeriesPtr);

}

void addInput(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
  ///////////////////////////////////////////////
  //     input
  // DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file://";
  dataProvider->uri += TERRAMA2_DATA_DIR;
  dataProvider->uri += "/fire_system";

  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->active = true;
  dataProvider->id = 1;
  dataProvider->name = "DataProvider queimadas local";
  dataProvider->dataProviderType = "FILE";

  dataManager->add(dataProviderPtr);

  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  // DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->id = 1;
  dataSeries->name = "DataProvider queimadas local";
  dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");
  dataSeries->dataProviderId = dataProviderPtr->id;

  terrama2::core::DataSetOccurrence* dataSet = new terrama2::core::DataSetOccurrence();
  dataSet->id = 1;
  dataSet->active = true;
  dataSet->format.emplace("mask", "exporta_%YYYY%MM%DD_%hh%mm.csv");
  dataSet->format.emplace("srid", "4326");

  dataSeries->datasetList.emplace_back(dataSet);

  dataManager->add(dataSeriesPtr);
}

void addOutput(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
  ///////////////////////////////////////////////
  //     output

  QUrl uri;
  uri.setScheme("postgis");
  uri.setHost(QString::fromStdString(TERRAMA2_DATABASE_HOST));
  uri.setPort(std::stoi(TERRAMA2_DATABASE_PORT));
  uri.setUserName(QString::fromStdString(TERRAMA2_DATABASE_USERNAME));
  uri.setPassword(QString::fromStdString(TERRAMA2_DATABASE_PASSWORD));
  uri.setPath(QString::fromStdString("/"+TERRAMA2_DATABASE_DBNAME));

  // DataProvider information
  terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
  outputDataProvider->id = 2;
  outputDataProvider->name = "DataProvider queimadas postgis";
  outputDataProvider->uri = uri.url().toStdString();
  outputDataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  outputDataProvider->dataProviderType = "POSTGIS";
  outputDataProvider->active = true;

  dataManager->add(outputDataProviderPtr);

  // DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
  outputDataSeries->id = 2;
  outputDataSeries->name = "DataProvider queimadas postgis";
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  outputDataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-postgis");
  outputDataSeries->dataProviderId = outputDataProviderPtr->id;

  dataManager->add(outputDataSeriesPtr);

  // DataSet information
  terrama2::core::DataSetOccurrence* outputDataSet = new terrama2::core::DataSetOccurrence();
  outputDataSet->active = true;
  outputDataSet->id = 2;
  outputDataSet->dataSeriesId = outputDataSeries->id;
  outputDataSet->format.emplace("table_name", "queimadas_test_table");

  outputDataSeries->datasetList.emplace_back(outputDataSet);
}

int main(int argc, char* argv[])
{
  try
  {
    terrama2::core::TerraMA2Init terramaRaii("example", 0);
    terrama2::core::registerFactories();


    {
      QCoreApplication app(argc, argv);
      auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);
      serviceManager.setLogConnectionInfo(uri);
      serviceManager.setInstanceId(1);

      auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();

      addInput(dataManager);
      addOutput(dataManager);
      addStaticDataSeries(dataManager);

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

      terrama2::services::collector::core::Intersection* intersection(new terrama2::services::collector::core::Intersection());
      terrama2::services::collector::core::IntersectionPtr intersectionPtr(intersection);

      // Adds the attribute "SIGLA" to the collected occurrences.
      intersection->collectorId = collector->id;
      std::vector<terrama2::services::collector::core::IntersectionAttribute> attrVec;
      terrama2::services::collector::core::IntersectionAttribute intersectionAttribute;
      intersectionAttribute.alias = "sigla_estado";
      intersectionAttribute.attribute = "sigla";
      attrVec.push_back(intersectionAttribute);
      intersection->attributeMap[3] = attrVec;
      collector->intersection = intersectionPtr;


      dataManager->add(collectorPtr);

      QTimer timer;
      QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
      timer.start(300000);
      app.exec();

      service.stopService();
    }


  }
  catch(...)
  {
    std::cout << "\n\nException...\n" << std::endl;
  }

  return 0;
}
