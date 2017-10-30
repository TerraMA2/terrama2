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

#include <terrama2/core/utility/CurlWrapperFtp.hpp>
#include <terrama2/core/utility/JSonUtils.hpp>


#include <QJsonDocument>
#include <QJsonObject>

#ifndef __TERRAMA2_EXAMPLES_GRADSFTP_HPP__
#define __TERRAMA2_EXAMPLES_GRADSFTP_HPP__


namespace terrama2
{

  namespace gradsftp
  {

      terrama2::core::DataProviderPtr dataProviderGradsFTP()
      {
           QString json = QString(R"(
                                      {
                                          "class": "DataProvider",
                                          "uri": "ftp://terrama:terr%40m%40inpe@server-ftpdsa.cptec.inpe.br",
                                          "project_id": 0,
                                          "id": 5,
                                          "name": "Provider",
                                          "intent":  "COLLECTOR_INTENT",
                                          "data_provider_type": "FTP",
                                          "description": null,
                                          "active": true
                                     }
                                   )"
                                 );

          QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
          QJsonObject obj = doc.object();
          return terrama2::core::fromDataProviderJson(obj);
      }


      terrama2::core::DataSeriesPtr dataSeriesGradsFTP(terrama2::core::DataProviderPtr dataProvider)
      {

           QString json = QString(R"(
                                       {
                                           "class": "DataSeries",
                                           "id": 5,
                                           "name": "Hidroestimador_input",
                                           "description": null,
                                           "data_provider_id":  %1,
                                           "semantics": "GRID-grads",
                                           "active": true,
                                           "datasets":[
                                                {
                                                   "class": "DataSet",
                                                   "id": 5,
                                                   "data_series_id": 5,
                                                   "active": true,
                                                   "format": {
                                                        "timestamp_property": "file_timestamp",
                                                        "number_of_bands": "1",
                                                        "value_multiplier": "1",
                                                        "data_type": "INT16",
                                                        "bytes_before": "0",
                                                        "bytes_after": "0",
                                                        "temporal": "false",
                                                        "ctl_filename": "/hidro/racc.ctl",
                                                        "srid": "4326",
                                                        "timezone": "0",
                                                        "binary_file_mask": "S10238225_%YYYY%MM%DD%hh%mm.bin"
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



      terrama2::core::DataProviderPtr dataProviderGradsFILE()
      {
          QString json = QString(R"(
                                    {
                                      "class": "DataProvider",
                                      "uri": "%1",
                                      "id": 6,
                                      "project_id": 0,
                                      "name": "Dados locais",
                                      "intent": "COLLECTOR_INTENT",
                                      "data_provider_type": "FILE",
                                      "description": null,
                                      "active": true
                                    }
                                   )"
                                 ).arg(QString::fromStdString(TERRAMA2_DATA_DIR));


          QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
          QJsonObject obj = doc.object();
          return terrama2::core::fromDataProviderJson(obj);
      }



      terrama2::core::DataSeriesPtr dataSeriesGradsFILE(terrama2::core::DataProviderPtr dataProvider)
      {

          QString json = QString(R"(
                                          {
                                              "class": "DataSeries",
                                              "id": 6,
                                              "name": "Hidro",
                                              "description": null,
                                              "data_provider_id":  %1,
                                              "semantics": "GRID-geotiff",
                                              "active": true,
                                              "datasets": [

                                                {
                                                  "class": "DataSet",
                                                  "id": 6,
                                                  "data_series_id": 6,
                                                  "active": true,
                                                  "format": {
                                                     "mask": "/grads/S10238225_%YYYY%MM%DD%hh%mm.tif"
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


   } // end namespace gradsftp
}  // end namespace terrama2

#endif  //__TERRAMA2_EXAMPLES_GRADSFTP_HPP__
