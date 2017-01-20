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
  \file examples/view/CreateGeoserverVectorLayer.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "Utils.hpp"

#include <terrama2/core/utility/Raii.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/services/view/core/View.hpp>
#include <terrama2/services/view/core/data-access/Geoserver.hpp>
#include <terrama2/services/view/core/JSonUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>

// Qt
#include <QTemporaryFile>
#include <QUrl>


int main(int argc, char** argv)
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  terrama2::core::registerFactories();

  try
  {
    auto dataManager = std::make_shared<terrama2::services::view::core::DataManager>();

    //DataProvider information
    QUrl uri;
    uri.setScheme("postgis");
    uri.setHost("localhost");
    uri.setPort(5432);
    uri.setUserName("postgres");
    uri.setPassword("postgres");
    uri.setPath("/terrama2");

    //DataProvider information
    terrama2::core::DataProvider* dataProviderPostGIS = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPostGISPtr(dataProviderPostGIS);
    dataProviderPostGIS->uri = uri.url().toStdString();
    dataProviderPostGIS->name = "postgis";
    dataProviderPostGIS->id = 1;

    dataProviderPostGIS->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
    dataProviderPostGIS->dataProviderType = "POSTGIS";
    dataProviderPostGIS->active = true;

    dataManager->add(dataProviderPostGISPtr);

    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

    //DataSeries information
    terrama2::core::DataSeries* dataSeriesGeometry = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesGeometryPtr(dataSeriesGeometry);
    dataSeriesGeometry->id = 2;
    dataSeriesGeometry->name = "dataSeriesGeometry";
    dataSeriesGeometry->dataProviderId = 1;
    dataSeriesGeometry->semantics = semanticsManager.getSemantics("STATIC_DATA-postgis");

    terrama2::core::DataSetGrid* dataSetGeometry = new terrama2::core::DataSetGrid();
    dataSetGeometry->active = true;
    dataSetGeometry->format.emplace("table_name", "estados_2010");

    dataSeriesGeometry->datasetList.emplace_back(dataSetGeometry);

    dataManager->add(dataSeriesGeometryPtr);

    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("POSTGIS",
                                                                                   dataProviderPostGIS->uri));

    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(datasource);

    if(!datasource->isOpened())
    {
      QString errMsg = QObject::tr("DataProvider could not be opened.");
      std::cout << std::endl << "An exception has occurred in GeoServer example: " << errMsg.toStdString() << std::endl;
      return EXIT_FAILURE;
    }

    std::unique_ptr< te::da::DataSetType > dataSetType = datasource->getDataSetType("estados_2010");

    terrama2::services::view::core::View::Legend legend;

    legend.ruleType = terrama2::services::view::core::View::Legend::VALUE;
    legend.band_number = 0;
    legend.column = "id";

    {
      terrama2::services::view::core::View::Legend::Rule rule;
      rule.title = "title1";
      rule.value = "1";
      rule.color = "#F10D0D";
      rule.isDefault = false;

      legend.rules.push_back(rule);
    }

    {
      terrama2::services::view::core::View::Legend::Rule rule;
      rule.title = "title2";
      rule.value = "2";
      rule.color = "#2E37CE";
      rule.isDefault = false;

      legend.rules.push_back(rule);
    }

    te::core::URI geoserverURI("http://admin:geoserver@localhost:8080/geoserver");

    terrama2::services::view::core::GeoServer geoserver(geoserverURI);
    geoserver.registerStyle("name3", legend, dataSetType);
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in GeoServer example: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in GeoServer example!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
