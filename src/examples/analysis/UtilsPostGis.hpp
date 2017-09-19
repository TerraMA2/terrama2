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
  \file terrama2/analysis/Utils.hpp

  \brief Specific exception classes from core module.

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

#ifndef __TERRAMA2_ANALYSIS_UTILS_HPP__
#define __TERRAMA2_ANALYSIS_UTILS_HPP__


namespace terrama2
{

namespace examples
{

  namespace analysis
  {

  namespace utilspostgis
  {

    const std::string dcp_result = "dcp_result";
    const std::string history_dcp_result = "history_dcp_result";
    const std::string dcp_history_interval_result = "dcp_history_interval_result";
    const std::string occurrence_analysis_result = "occurrence_analysis_result";
    const std::string zonal_history_ratio_analysis_result = "zonal_history_ratio_analysis_result";
    const std::string zonal_analysis_result = "zonal_analysis_result";
    const std::string analysis_dcp_result = "analysis_dcp_result";





    terrama2::core::DataProviderPtr dataProviderPostGis()
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
        QJsonObject obj = doc.object();
        return terrama2::core::fromDataProviderJson(obj);
    }

    terrama2::core::DataSeriesPtr dataSeriesPostGis(terrama2::core::DataProviderPtr dataProvider)
    {
        QString json = QString(R"(
                                  {
                                    "class": "DataSeries",
                                    "id": 1,
                                    "name": "Monitored Object",
                                    "description": null,
                                    "data_provider_id":  %1,
                                    "semantics": "STATIC_DATA-postgis",
                                    "active": true,
                                    "datasets": [
                                    {
                                        "class": "DataSet",
                                        "id": 1,
                                        "data_series_id": 1,
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

    terrama2::core::DataSeriesPtr occurrenceDataSeriesPostGis(terrama2::core::DataProviderPtr dataProvider)
    {

        QString json = QString(R"(
                                        {
                                            "class": "DataSeries",
                                            "id": 2,
                                            "name": "Occurrence",
                                            "description": "null",
                                            "data_provider_id":  %1,
                                            "semantics": "OCCURRENCE-postgis",
                                            "active": true,
                                            "datasets": [
                                              {
                                                "class": "DataSet",
                                                "id": 2,
                                                "data_series_id": 3,
                                                "active": true,
                                                "format": {
                                                    "table_name": "queimadas_test_table",
                                                    "timestamp_property": "data_pas",
                                                    "geometry_property": "geom",
                                                    "timezone": "UTC-03"
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



    terrama2::core::DataSeriesPtr outputDataSeriesPostGis(terrama2::core::DataProviderPtr outputDataProvider, std::string nameTableResult)
    {

        QString json = QString(R"(
                                        {
                                            "class": "DataSeries",
                                            "id": 3,
                                            "name": "Analysis result",
                                            "description": "null",
                                            "data_provider_id":  %1,
                                            "semantics": "ANALYSIS_MONITORED_OBJECT-postgis",
                                            "active": true,
                                            "datasets": [
                                              {
                                                "class": "DataSet",
                                                "id": 3,
                                                "data_series_id": 3,
                                                "active": true,
                                                "format": {
                                                    "table_name": "%2"
                                                }
                                              }
                                            ]
                                        }
                                        )"
                                    ).arg(outputDataProvider->id).arg(QString::fromStdString(nameTableResult));



        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        QJsonObject obj = doc.object();
        return terrama2::core::fromDataSeriesJson(obj);
    }


   } // end namespace utilspostgis
   } // end namespace analysis
  }  // end namespace examples
}  // end namespace terrama2

#endif  // __TERRAMA2_ANALYSIS_UTILS_HPP__
