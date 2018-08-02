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

  \brief Json DCPSerramarInpe for tests of collector and analysis module.

  \author Bianca Maciel, Jano Simas
*/

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/core/utility/JSonUtils.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include <QJsonDocument>
#include <QJsonObject>

#ifndef __TERRAMA2_EXAMPLES_DCPSERRAMARINPE_HPP__
#define __TERRAMA2_EXAMPLES_DCPSERRAMARINPE_HPP__


namespace terrama2
{
/**
 * @brief Namespace for test metadata for Serramar
 * 
 */
  namespace serramar
  {


        QJsonObject dataProviderSerramarJson()
        {
             QString json = QString(R"(
                                        {
                                            "class": "DataProvider",
                                            "uri": "%1",
                                            "project_id": 0,
                                            "id": 2,
                                            "name": "Provider",
                                            "intent":  "COLLECTOR_INTENT",
                                            "data_provider_type": "FILE",
                                            "description": "null",
                                            "active": true
                                       }
                                     )"
                                   ).arg(QString::fromStdString(TERRAMA2_DATA_DIR));

            QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
            return doc.object();


        }

        terrama2::core::DataProviderPtr dataProviderSerramarInpe()
        {
             auto obj = dataProviderSerramarJson();
             return terrama2::core::fromDataProviderJson(obj);
        }

        QJsonObject dataSeriesDcpSerramarJson(terrama2::core::DataProviderPtr dataProvider)
        {

                   QString json = QString(R"x(
                                               {
                                                   "class": "DataSeries",
                                                   "id": 1,
                                                   "name": "Serra do Mar",
                                                   "description": null,
                                                   "data_provider_id":  %1,
                                                   "semantics": "DCP-inpe",
                                                   "active": true,
                                                   "datasets":[
                                                        {
                                                           "class": "DataSet",
                                                           "id": 1,
                                                           "data_series_id": 1,
                                                           "active": true,
                                                           "format": {
                                                               "mask": "/PCD_serrmar_INPE/30887.txt",
                                                               "alias": "pcd_picinguaba",
                                                               "timezone": "-02:00",
                                                               "projection": 4618

                                                           },
                                                           "position": "SRID=4618;POINT(-44.85 -23.355)"
                                                        },
                                                        {
                                                           "class": "DataSet",
                                                           "id": 2,
                                                           "data_series_id": 1,
                                                           "active": true,

                                                           "format": {
                                                               "mask": "/PCD_serrmar_INPE/30889.txt",
                                                               "alias": "pcd_itanhaem",
                                                               "timezone": "-02:00",
                                                               "projection": 4618
                                                           },
                                                           "position": "SRID=4618;POINT(-46.79 -24.174)"
                                                        },
                                                        {
                                                           "class": "DataSet",
                                                           "id": 3,
                                                           "data_series_id": 1,
                                                           "active": true,
                                                           "format": {
                                                                   "mask": "/PCD_serrmar_INPE/69031.txt",
                                                                   "alias": "pcd_ubatuba",
                                                                   "timezone": "-02:00",
                                                                   "projection": 4618
                                                           },
                                                           "position": "SRID=4618;POINT(-45.118 -23.391)"
                                                        },
                                                        {
                                                           "class": "DataSet",
                                                           "id": 4,
                                                           "data_series_id": 1,
                                                           "active": true,
                                                           "format": {
                                                                   "mask": "/PCD_serrmar_INPE/32524.txt",
                                                                   "alias": "pcd_ guaratingueta",
                                                                   "timezone": "-02:00"
                                                           },
                                                           "position": "SRID=4618;POINT(-45.189 -22.801)"
                                                        },
                                                        {
                                                           "class": "DataSet",
                                                           "id": 5,
                                                           "data_series_id": 1,
                                                           "active": true,
                                                           "format": {
                                                               "mask": "/PCD_serrmar_INPE/30885.txt",
                                                               "alias": "pcd_cunha",
                                                               "timezone": "-02:00"
                                                           },
                                                           "position": "SRID=4618;POINT(-44.941 -23.074)"
                                                        }
                                                   ]
                                                  }
                                                )x"
                                              ).arg(dataProvider->id);
                       QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
                       return doc.object();
         }

         terrama2::core::DataSeriesPtr dataSeriesDcpSerramar(terrama2::core::DataProviderPtr dataProvider)
         {
             auto obj = dataSeriesDcpSerramarJson(dataProvider);
             return terrama2::core::fromDataSeriesJson(obj);
         }

         QJsonObject dataProviderPostGisDCPJson()
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

        terrama2::core::DataProviderPtr dataProviderPostGisDCP()
        {
            auto obj = dataProviderPostGisDCPJson();
            return terrama2::core::fromDataProviderJson(obj);
        }

        terrama2::core::DataSeriesPtr dataSeriesDcpSerramarPostGis(terrama2::core::DataProviderPtr dataProvider)
        {

            QString json = QString(R"x(
                                            {
                                                "class": "DataSeries",
                                                "id": 2,
                                                "name": "Serra do Mar",
                                                "description": null,
                                                "data_provider_id":  %1,
                                                "semantics": "DCP-postgis",
                                                "active": true,
                                                "datasets": [

                                                  {
                                                    "class": "DataSet",
                                                    "id": 6,
                                                    "data_series_id": 2,
                                                    "active": true,
                                                    "format": {
                                                        "table_name": "pcd_picinguaba",
                                                        "alias": "pcd_picinguaba",
                                                        "timestamp_property": "datetime",
                                                        "timezone": "-02:00"
                                                    },
                                                    "position": "SRID=4618;POINT(-44.85 -23.355)"
                                                  },
                                                  {
                                                    "class": "DataSet",
                                                    "id": 7,
                                                    "data_series_id": 2,
                                                    "active": true,
                                                    "format": {
                                                        "table_name": "pcd_itanhaem",
                                                        "alias": "pcd_itanhaem",
                                                        "timestamp_property": "datetime",
                                                        "timezone": "-02:00"
                                                    },
                                                    "position": "SRID=4618;POINT(-46.79 -24.174)"
                                                  },
                                                  {
                                                     "class": "DataSet",
                                                     "id": 8,
                                                     "data_series_id": 2,
                                                     "active": true,
                                                     "format": {
                                                             "table_name": "pcd_ubatuba",
                                                             "alias": "pcd_ubatuba",
                                                             "timestamp_property": "datetime",
                                                             "timezone": "-02:00"

                                                     },
                                                     "position": "SRID=4618;POINT(-44.46540 -23.00506)"
                                                  },
                                                  {
                                                     "class": "DataSet",
                                                     "id": 9,
                                                     "data_series_id": 2,
                                                     "active": true,
                                                     "format": {
                                                                "table_name": "pcd_guaratingueta",
                                                                "alias": "pcd_guaratingueta",
                                                                "timestamp_property": "datetime",
                                                                "timezone": "-02:00"
                                                     },
                                                     "position": "SRID=4618;POINT(-45.189 -22.801)"
                                                  },
                                                  {
                                                     "class": "DataSet",
                                                     "id": 10,
                                                     "data_series_id": 2,
                                                     "active": true,
                                                     "format": {
                                                              "table_name": "pcd_cunha",
                                                              "alias": "pcd_cunha",
                                                              "timestamp_property": "datetime",
                                                              "timezone": "-02:00"
                                                     },
                                                     "position": "SRID=4618;POINT(-44.941 -23.074)"
                                                  }

                                                ]
                                            }
                                            )x"
                                        ).arg(dataProvider->id);



            QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
            QJsonObject obj = doc.object();
            return terrama2::core::fromDataSeriesJson(obj);
        }


   } // end namespace serramar
}  // end namespace terrama2

#endif  //__TERRAMA2_EXAMPLES_DCPSERRAMARINPE_HPP__
