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
  \file /examples/core/AccessAndStoreDCPGenericFromLocal.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include <terrama2/Config.hpp>
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/data-access/DcpSeries.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/FileRemover.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/DataStoragerFactory.hpp>
#include <terrama2/impl/DataStoragerPostGIS.hpp>
#include <terrama2/impl/DataAccessorCSV.hpp>
#include <terrama2/impl/Utils.hpp>


//QT
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

// STL
#include <iostream>


int main(int, char**)
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  terrama2::core::registerFactories();

  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file://";
  dataProvider->uri+=TERRAMA2_DATA_DIR;

  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  dataSeries->semantics = semanticsManager.getSemantics("DCP-generic");


  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("folder", "/pcd_toa5/CPV/");
  dataSet->format.emplace("mask", "CPV_slow_%YYYY_%MM_%DD_%hh%mm.dat");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("srid", "4326");
  dataSet->format.emplace("header_size", "4");
  dataSet->format.emplace("properties_names_line", "2");
  dataSet->format.emplace("convert_all", "true");
  dataSet->format.emplace("default_type", "TEXT");

  QJsonArray fields;

  {
    QJsonObject obj;

    obj.insert("property_name", QString("TIMESTAMP"));
    obj.insert("alias", QString("datetime"));
    obj.insert("type", QString("DATETIME"));
    obj.insert("format", QString("%YYYY-%MM-%DD %hh:%mm:%ss"));

    fields.push_back(obj);
  }

  {
    QJsonObject obj;

    obj.insert("property_name", QString("RECORD"));
    obj.insert("alias", QString("record"));
    obj.insert("type", QString("INTEGER"));

    fields.push_back(obj);
  }

  {
    QJsonObject obj;

    obj.insert("property_name", QString("E_field_Avg"));
    obj.insert("alias", QString("efield"));
    obj.insert("type", QString("INTEGER"));

    fields.push_back(obj);
  }

  {
    QJsonObject obj;

    obj.insert("property_name", QString("panel_temp_Avg"));
    obj.insert("alias", QString("panel"));
    obj.insert("type", QString("INTEGER"));

    fields.push_back(obj);
  }

  {
    QJsonObject obj;

    obj.insert("property_name", QString("leakage_cur_Avg"));
    obj.insert("alias", QString("leak"));
    obj.insert("type", QString("FLOAT"));

    fields.push_back(obj);
  }

  {
    QJsonObject obj;

    obj.insert("property_name", QString("battery_volt_Avg"));
    obj.insert("alias", QString("bateria"));
    obj.insert("type", QString("FLOAT"));

    fields.push_back(obj);
  }

  {
    QJsonObject obj;

    obj.insert("property_name", QString("internal_RH_Avg"));
    obj.insert("alias", QString("interval"));
    obj.insert("type", QString("TEXT"));

    fields.push_back(obj);
  }

  QJsonObject obj;
  obj.insert("fields", fields);
  QJsonDocument doc(obj);

  dataSet->format.emplace("fields", QString(doc.toJson(QJsonDocument::Compact)).toStdString());

  dataSeries->datasetList.emplace_back(dataSet);

  //empty filter
  terrama2::core::Filter filter;

  //accessing data
  auto accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProviderPtr, dataSeriesPtr);
  auto remover = std::make_shared<terrama2::core::FileRemover>();
  auto uriMap = accessor->getFiles(filter, remover);

  auto dataMap = accessor->getSeries(uriMap, filter, remover);

  terrama2::core::DcpSeriesPtr dcpSeries = std::make_shared<terrama2::core::DcpSeries>();
  dcpSeries->addDcpSeries(dataMap);

  assert(dcpSeries->dcpSeriesMap().size() == 1);

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

  dataProviderPostGIS->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
  dataProviderPostGIS->dataProviderType = "POSTGIS";
  dataProviderPostGIS->active = true;

  //DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
  outputDataSeries->semantics = semanticsManager.getSemantics("DCP-postgis");

  terrama2::core::DataSetDcp* dataSetOutput = new terrama2::core::DataSetDcp();
  terrama2::core::DataSetPtr dataSetOutputPtr(dataSetOutput);
  dataSetOutput->active = true;
  dataSetOutput->format.emplace("table_name", "dcp_generic");
  dataSetOutput->format.emplace("timestamp_column", "datetime");

  auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(outputDataSeriesPtr, dataProviderPostGISPtr);

  for(auto& item : dataMap)
  {
    dataStorager->store( item.second, dataSetOutputPtr);
  }

  return 0;
}
