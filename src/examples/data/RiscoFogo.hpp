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
  \file terrama2/analysis/UtilsGeotiff.hpp

  \brief Specific exception classes from core module.

  \author Bianca Maciel, Jano Simas
 */


#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/core/utility/JSonUtils.hpp>

#include <QJsonDocument>
#include <QJsonObject>

#ifndef __TERRAMA2_ANALYSIS_RISCOFOGO_HPP__
#define __TERRAMA2_ANALYSIS_RISCOFOGO_HPP__


namespace terrama2
{
  namespace riscofogo
  {


     terrama2::core::DataProviderPtr dataProviderFileGrid()
     {
          QString json = QString(R"(
                                     {
                                         "class": "DataProvider",
                                         "uri": "%1",
                                         "project_id": 0,
                                         "id": 8,
                                         "name": "Provider",
                                         "intent":  "COLLECTOR_INTENT",
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


     terrama2::core::DataSeriesPtr dataSeriesResultAnalysisGrid(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 8,
                                         "name": "Output Grid",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 8,
                                                 "data_series_id": 8,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/resultadoRF/result%YYYY%MM%DD%hh%mm.tif",
                                                         "timezone": "00:00"
                                                 }
                                              }
                                         ]
                                        }
                                      )x"
                                    ).arg(dataProvider->id);
             QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
             QJsonObject obj = doc.object();
             return terrama2::core::fromDataSeriesJson(obj);
     }



     terrama2::core::DataSeriesPtr dataSeriesPrecipitation(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 9,
                                         "name": "precipitacao",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 9,
                                                 "data_series_id": 9,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/Coleta_FTP_RF/prec/S10648241_%YYYY%MM%DD%hh%mm.tif",
                                                         "timezone": "00:00"

                                                 }
                                              }
                                         ]
                                        }
                                      )x"
                                    ).arg(dataProvider->id);
             QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
             QJsonObject obj = doc.object();
             return terrama2::core::fromDataSeriesJson(obj);
     }


     terrama2::core::DataSeriesPtr dataSeriesTemperature(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 10,
                                         "name": "temperatura",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 10,
                                                 "data_series_id": 10,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/Coleta_FTP_RF/temp/BAM.tems.%YYYY%MM%DD%hh%mm.tif",
                                                         "timezone": "00:00"

                                                 }
                                              }
                                         ]
                                        }
                                      )x"
                                    ).arg(dataProvider->id);
             QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
             QJsonObject obj = doc.object();
             return terrama2::core::fromDataSeriesJson(obj);
     }


     terrama2::core::DataSeriesPtr dataSeriesLancover(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 11,
                                         "name": "lancover",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 11,
                                                 "data_series_id": 11,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/Coleta_FTP_RF/vegetacao/REF_5km.AS.tif",
                                                         "timezone": "00:00"

                                                 }
                                              }
                                         ]
                                        }
                                      )x"
                                    ).arg(dataProvider->id);
             QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
             QJsonObject obj = doc.object();
             return terrama2::core::fromDataSeriesJson(obj);
     }

     terrama2::core::DataSeriesPtr dataSeriesHumidity(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 12,
                                         "name": "umidade",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 12,
                                                 "data_series_id": 12,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/Coleta_FTP_RF/umid/BAM.umrs.%YYYY%MM%DD%hh%mm.tif",
                                                         "timezone": "00:00"

                                                 }
                                              }
                                         ]
                                        }
                                      )x"
                                    ).arg(dataProvider->id);
             QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
             QJsonObject obj = doc.object();
             return terrama2::core::fromDataSeriesJson(obj);
     }


  }  // end namespace geotiff
}  // end namespace terrama2

#endif  // __TERRAMA2_ANALYSIS_RISCOFOGO_HPP__
