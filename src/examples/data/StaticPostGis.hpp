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
  \file examples/PostGis.hpp

  \brief General mock classes for tests of collector module.

  \author Jano Simas
*/

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/core/utility/JSonUtils.hpp>


#include <QJsonDocument>
#include <QJsonObject>

#ifndef __TERRAMA2_EXAMPLES_STATICPOSTGIS_HPP__
#define __TERRAMA2_EXAMPLES_STATICPOSTGIS_HPP__


namespace terrama2
{

  namespace staticpostgis
  {
      QJsonObject dataProviderStaticPostGisJson()
      {
          QString json = QString(R"(
                                    {
                                      "class": "DataProvider",
                                      "uri": "%1",
                                      "id": 1,
                                      "project_id": 0,
                                      "name": "Provider",
                                      "intent": "COLLECTOR_INTENT",
                                      "data_provider_type": "POSTGIS",
                                      "description": null,
                                      "active": true
                                    }
                                   )"
                                 ).arg(QString::fromStdString("pgsql://"+TERRAMA2_DATABASE_USERNAME+ ":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME));


          QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
          return doc.object();
      }

      terrama2::core::DataProviderPtr dataProviderStaticPostGis()
      {
          auto obj = dataProviderStaticPostGisJson();
          return terrama2::core::fromDataProviderJson(obj);
      }

      terrama2::core::DataSeriesPtr dataSeriesEstados2010(terrama2::core::DataProviderPtr dataProvider)
      {
          QString json = QString(R"(
                                    {
                                      "class": "DataSeries",
                                      "id": 11,
                                      "name": "Monitored Object",
                                      "description": null,
                                      "data_provider_id":  %1,
                                      "semantics": "STATIC_DATA-postgis",
                                      "active": true,
                                      "datasets": [
                                          {
                                              "class": "DataSet",
                                              "id": 11,
                                              "data_series_id": 11,
                                              "active": true,
                                              "format": {
                                                  "table_name": "estados_2010"
                                              }
                                          }
                                      ]
                                    }
                                 )"
                               ).arg(dataProvider->id);


           QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
           QJsonObject obj = doc.object();
           return terrama2::core::fromDataSeriesJson(obj);

      }

      QJsonObject dataSeriesMunicSerrmarInpeJson(terrama2::core::DataProviderPtr dataProvider)
      {
          QString json = QString(R"(
                                    {
                                      "class": "DataSeries",
                                      "id": 2,
                                      "name": "Monitored Object",
                                      "description": null,
                                      "data_provider_id":  %1,
                                      "semantics": "STATIC_DATA-postgis",
                                      "active": true,
                                      "datasets": [
                                          {
                                              "class": "DataSet",
                                              "id": 2,
                                              "data_series_id": 2,
                                              "active": true,
                                              "format": {
                                                  "table_name": "munic_afetados_serrmar_inpe"
                                              }
                                          }
                                      ]
                                    }
                                 )"
                               ).arg(dataProvider->id);


           QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
           return doc.object();


      }

      terrama2::core::DataSeriesPtr dataSeriesMunicSerrmarInpe(terrama2::core::DataProviderPtr dataProvider)
      {
           auto obj = dataSeriesMunicSerrmarInpeJson(dataProvider);
           return terrama2::core::fromDataSeriesJson(obj);
      }


   } // end namespace staticpostgis
}  // end namespace terrama2

#endif  //__TERRAMA2_EXAMPLES_STATICPOSTGIS_HPP__
