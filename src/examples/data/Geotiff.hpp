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

#ifndef __TERRAMA2_ANALYSIS_GEOTIFF_HPP__
#define __TERRAMA2_ANALYSIS_GEOTIFF_HPP__


namespace terrama2
{
  namespace geotiff
  {

      namespace nameoutputgrid
      {

        const std::string output_history = "/geotiff/historical/output_history_grid.tif";
        const std::string output_grid = "/geotiff/output_grid.tif";
        const std::string output_novaFriburgo = "/AnResultTesteNF/output_novaFriburgo.tif";
       }
     terrama2::core::DataProviderPtr dataProviderFileGrid()
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
                                         "description": null,
                                         "active": true
                                    }
                                  )"
                                ).arg(QString::fromStdString(TERRAMA2_DATA_DIR));

         QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
         QJsonObject obj = doc.object();
         return terrama2::core::fromDataProviderJson(obj);

     }


     terrama2::core::DataSeriesPtr resultAnalysisGrid(terrama2::core::DataProviderPtr dataProvider, std::string nameoutputgrid)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 6,
                                         "name": "Output Grid",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 6,
                                                 "data_series_id": 6,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "%2",
                                                         "timezone": "00:00"
                                                 }
                                              }
                                         ]
                                        }
                                      )x"
                                    ).arg(dataProvider->id).arg(QString::fromStdString(nameoutputgrid));
             QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
             QJsonObject obj = doc.object();
             return terrama2::core::fromDataSeriesJson(obj);
     }



     terrama2::core::DataSeriesPtr dataSeriesHistorical(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 1,
                                         "name": "geotiff 1",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 1,
                                                 "data_series_id": 1,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/geotiff/historical/%YYYY%MM%DD_%hh%mm%ss.tif",
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

     terrama2::core::DataSeriesPtr dataSeriesSpotVegetacao(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 2,
                                         "name": "geotiff 1",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-static_gdal",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 2,
                                                 "data_series_id": 2,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/geotiff/Spot_Vegetacao_Jul2001_SP.tif"

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

     terrama2::core::DataSeriesPtr dataSeriesL5219076(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 3,
                                         "name": "geotiff 1",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 3,
                                                 "data_series_id": 3,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/geotiff/L5219076_07620040908_r3g2b1.tif",
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

     terrama2::core::DataSeriesPtr dataSeriesHidro(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 4,
                                         "name": "Hidro",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-geotiff",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 4,
                                                 "data_series_id": 4,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/hidro_diario_jan2011/S11216377_%YYYY%MM%DD%hh%mm.tif",
                                                         "timezone": "UTC+00"

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

     terrama2::core::DataSeriesPtr dataSeriesSTRM_A(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 5,
                                         "name": "SRTM_a_latlong_sad69",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-static_gdal",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 5,
                                                 "data_series_id": 5,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/Rio_Friburgo/SRTM_a_latlong_sad69.tif"

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

     terrama2::core::DataSeriesPtr dataSeriesSTRM_S(terrama2::core::DataProviderPtr dataProvider)
     {

         QString json = QString(R"x(
                                     {
                                         "class": "DataSeries",
                                         "id": 7,
                                         "name": "SRTM_s_latlong_sad69",
                                         "description": null,
                                         "data_provider_id":  %1,
                                         "semantics": "GRID-static_gdal",
                                         "active": true,
                                         "datasets":[
                                              {
                                                 "class": "DataSet",
                                                 "id": 7,
                                                 "data_series_id": 7,
                                                 "active": true,
                                                  "format": {
                                                         "mask": "/Rio_Friburgo/SRTM_s_latlong_sad69.tif"


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

#endif  // __TERRAMA2_ANALYSIS_GEOTIFF_HPP__
