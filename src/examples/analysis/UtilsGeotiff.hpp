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

#ifndef __TERRAMA2_ANALYSIS_UTILSGEOTIFF_HPP__
#define __TERRAMA2_ANALYSIS_UTILSGEOTIFF_HPP__


namespace terrama2
{
  namespace examples
  {

  namespace analysis
  {

  namespace utilsgeotiff
  {

    terrama2::core::DataProviderPtr dataProviderFile()
    {
         QString json = QString(R"(
                                    {
                                        "class": "DataProvider",
                                        "uri": "%1",
                                        "project_id": 0,
                                        "id": 1,
                                        "name": "Provider",
                                        "intent":  "COLLECTOR_INTENT",
                                        "data_provider_type": "FILE",
                                        "description": null,
                                        "active": true
                                   }
                                 )"
                               ).arg(QString::fromStdString(TERRAMA2_DATA_DIR+"/geotiff"));

        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        QJsonObject obj = doc.object();
        return terrama2::core::fromDataProviderJson(obj);

    }

    terrama2::core::DataSeriesPtr outputDataSeriesHistory(terrama2::core::DataProviderPtr dataProvider)
    {

        QString json = QString(R"x(
                                    {
                                        "class": "DataSeries",
                                        "id": 5,
                                        "name": "Output Grid",
                                        "description": null,
                                        "data_provider_id":  %1,
                                        "semantics": "GRID-gdal",
                                        "active": true,
                                        "datasets":[
                                             {
                                                "class": "DataSet",
                                                "id": 5,
                                                "data_series_id": 5,
                                                "active": true,
                                                 "format": {
                                                        "mask": "/historical/output_history_grid.tif",
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

    terrama2::core::DataSeriesPtr outputDataSeries(terrama2::core::DataProviderPtr dataProvider)
    {

        QString json = QString(R"x(
                                    {
                                        "class": "DataSeries",
                                        "id": 6,
                                        "name": "Output Grid",
                                        "description": null,
                                        "data_provider_id":  %1,
                                        "semantics": "GRID-gdal",
                                        "active": true,
                                        "datasets":[
                                             {
                                                "class": "DataSet",
                                                "id": 6,
                                                "data_series_id": 6,
                                                "active": true,
                                                 "format": {
                                                        "mask": "output_grid.tif",
                                                        "timezone": "00:00",
                                                        "srid": 4326
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

    terrama2::core::DataSeriesPtr dataSeries(terrama2::core::DataProviderPtr dataProvider)
    {

        QString json = QString(R"x(
                                    {
                                        "class": "DataSeries",
                                        "id": 1,
                                        "name": "geotiff 1",
                                        "description": null,
                                        "data_provider_id":  %1,
                                        "semantics": "GRID-gdal",
                                        "active": true,
                                        "datasets":[
                                             {
                                                "class": "DataSet",
                                                "id": 1,
                                                "data_series_id": 1,
                                                "active": true,
                                                 "format": {
                                                        "mask": "/historical/%YYYY%MM%DD_%hh%mm%ss.tif",
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

    terrama2::core::DataSeriesPtr dataSeriesStaticGdal(terrama2::core::DataProviderPtr dataProvider)
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
                                                        "mask": "Spot_Vegetacao_Jul2001_SP.tif"

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

    terrama2::core::DataSeriesPtr dataSeriesGridGdal(terrama2::core::DataProviderPtr dataProvider)
    {

        QString json = QString(R"x(
                                    {
                                        "class": "DataSeries",
                                        "id": 3,
                                        "name": "geotiff 1",
                                        "description": null,
                                        "data_provider_id":  %1,
                                        "semantics": "GRID-gdal",
                                        "active": true,
                                        "datasets":[
                                             {
                                                "class": "DataSet",
                                                "id": 3,
                                                "data_series_id": 3,
                                                "active": true,
                                                 "format": {
                                                        "mask": "L5219076_07620040908_r3g2b1.tif",
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
    }  // end namespace utilsdcpserrmarinpe
   }  // end namespace analysis
  }  // end namespace examples
}  // end namespace terrama2

#endif  // __TERRAMA2_ANALYSIS_UTILSDCPSERRMARINPE_HPP__
