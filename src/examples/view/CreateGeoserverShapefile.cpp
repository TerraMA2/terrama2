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
  \file examples/view/CreateGeoServerShapefile.cpp

  \author Paulo R. M. Oliveira
 */

// TerraMA2

#include <memory>

#include <terrama2/services/view/core/Utils.hpp>
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/data-model/Filter.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/impl/Utils.hpp>

#include <QCoreApplication>
#include <QUrl>

int main(int argc, char** argv)
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  terrama2::core::registerFactories();

  {

    QCoreApplication app(argc, argv);


    terrama2::core::DataManagerPtr dataManager(new terrama2::core::DataManager());

    // DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file:///" + TERRAMA2_DATA_DIR + "/umidade";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;
    dataProvider->id = 1;
    dataProvider->name = "Local Geotiff";

    dataManager->add(dataProviderPtr);

    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();



    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data Series 1
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    dataSet1->id = 1;
    dataSet1->dataSeriesId = dataSeries1->id;

    dataSeries1->datasetList.emplace_back(dataSetPtr);

    dataManager->add(dataSeries1Ptr);

    QUrl url(QString::fromStdString(dataProvider->uri));
    terrama2::core::Filter filter;
    terrama2::services::view::core::createGeoserverTempMosaic(dataManager, dataSetPtr, filter, "Umin", url.path().toStdString());
  }

  return 0;
}
