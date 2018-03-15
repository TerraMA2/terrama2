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
  \file examples/OccurenceWFP.hpp

  \brief Json OCC-WFP for tests of collector and analysis module.

  \author Bianca Maciel, Jano Simas
*/

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/core/utility/JSonUtils.hpp>


#include <QJsonDocument>
#include <QJsonObject>

#ifndef __TERRAMA2_EXAMPLES_OCCURRENCEWFP_HPP__
#define __TERRAMA2_EXAMPLES_OCCURRENCEWFP_HPP__


namespace terrama2
{

  namespace occurrencewfp
  {

      QJsonObject dataProviderFileOccWFPJson()
      {
           QString json = QString(R"(
                                      {
                                          "class": "DataProvider",
                                          "uri": "%1",
                                          "project_id": 0,
                                          "id": 3,
                                          "name": "Provider",
                                          "intent":  "COLLECTOR_INTENT",
                                          "data_provider_type": "FILE",
                                          "description": null,
                                          "active": true
                                     }
                                   )"
                                 ).arg(QString::fromStdString(TERRAMA2_DATA_DIR));

          QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
           return doc.object();
      }

      terrama2::core::DataProviderPtr dataProviderFileOccWFP()
      {
          auto obj = dataProviderFileOccWFPJson();
          return terrama2::core::fromDataProviderJson(obj);
      }

      QJsonObject dataSeriesOccWFPJson(terrama2::core::DataProviderPtr dataProviderFileOccWFP)
      {

           QString json = QString(R"(
                                       {
                                           "class": "DataSeries",
                                           "id": 2,
                                           "name": "DataProvider queimadas local",
                                           "description": null,
                                           "data_provider_id":  %1,
                                           "semantics": "OCCURRENCE-wfp",
                                           "active": true,
                                           "datasets":[
                                                {
                                                   "class": "DataSet",
                                                   "id": 2,
                                                   "data_series_id": 2,
                                                   "active": true,
                                                   "format": {
                                                       "mask": "/fire_system/exporta_%YYYY%MM%DD_%hh%mm.csv",
                                                       "timestamp_property": "data_pas",
                                                       "input_geometry_property": "geom",
                                                       "geometry_property": "geom",
                                                       "latitude_property": "lat",
                                                       "longitude_property": "lon",
                                                       "timezone": "UTM+00",
                                                       "srid": 4326

                                                   }
                                                }
                                           ]
                                          }
                                        )"
                                      ).arg(dataProviderFileOccWFP->id);
               QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
               return doc.object();
       }

      terrama2::core::DataSeriesPtr dataSeriesOccWFP(terrama2::core::DataProviderPtr dataProviderFileOccWFP)
      {
               auto obj = dataSeriesOccWFPJson(dataProviderFileOccWFP);
               return terrama2::core::fromDataSeriesJson(obj);
      }

      terrama2::core::DataSeriesPtr dataSeriesOccWFPFile(terrama2::core::DataProviderPtr dataProvider)
      {

          QString json = QString(R"(
                                          {
                                              "class": "DataSeries",
                                              "id": 4,
                                              "name": "Occurrence",
                                              "description": null,
                                              "data_provider_id":  %1,
                                              "semantics": "OCCURRENCE-wfp",
                                              "active": true,
                                              "datasets": [
                                                {
                                                  "class": "DataSet",
                                                  "id": 4,
                                                  "data_series_id": 4,
                                                  "active": true,
                                                  "format": {
                                                      "mask": "/csv/queimadas_%YYYY%MM%DD_%hh%mm.csv",
                                                      "timestamp_property": "data_pas",
                                                      "input_geometry_property": "geom",
                                                      "geometry_property": "geom",
                                                      "latitude_property": "lat",
                                                      "longitude_property": "lon",
                                                      "timezone": "UTM+00",
                                                      "srid": 4326

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

      QJsonObject dataProviderPostGisOccWFPJson()
      {
          QString json = QString(R"(
                                    {
                                      "class": "DataProvider",
                                      "uri": "%1",
                                      "id": 4,
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


      terrama2::core::DataProviderPtr dataProviderPostGisOccWFP()
      {
          auto obj = dataProviderPostGisOccWFPJson();
          return terrama2::core::fromDataProviderJson(obj);
      }



      QJsonObject dataSeriesOccWFPPostGisJson(terrama2::core::DataProviderPtr dataProvider)
      {

          QString json = QString(R"(
                                          {
                                              "class": "DataSeries",
                                              "id": 3,
                                              "name": "Occurrence",
                                              "description": null,
                                              "data_provider_id":  %1,
                                              "semantics": "OCCURRENCE-postgis",
                                              "active": true,
                                              "datasets": [

                                                {
                                                  "class": "DataSet",
                                                  "id": 3,
                                                  "data_series_id": 3,
                                                  "active": true,
                                                  "format": {
                                                      "timestamp_property": "data_pas",
                                                      "geometry_property": "geom",
                                                      "table_name": "queimadas_test_table"

                                                  }
                                                }

                                              ]
                                          }
                                          )"
                                      ).arg(dataProvider->id);



          QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
          return doc.object();
      }

      terrama2::core::DataSeriesPtr dataSeriesOccWFPPostGis(terrama2::core::DataProviderPtr dataProvider)
      {
          QJsonObject obj = dataSeriesOccWFPPostGisJson(dataProvider);
          return terrama2::core::fromDataSeriesJson(obj);
      }


   } // end namespace occurrencewfp
}  // end namespace terrama2

#endif  //__TERRAMA2_EXAMPLES_OCCURRENCEWFP_HPP__
