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

#ifndef __TERRAMA2_EXAMPLES_RESULTANALYSISPOSTGIS_HPP__
#define __TERRAMA2_EXAMPLES_RESULTANALYSISPOSTGIS_HPP__


namespace terrama2
{

  namespace resultanalysis
  {

      namespace tablename
      {
          const std::string dcp_result = "dcp_result";
          const std::string history_dcp_result = "history_dcp_result";
          const std::string dcp_history_interval_result = "dcp_history_interval_result";
          const std::string occurrence_analysis_result = "occurrence_analysis_result";
          const std::string zonal_history_ratio_analysis_result = "zonal_history_ratio_analysis_result";
          const std::string zonal_analysis_result = "zonal_analysis_result";
          const std::string analysis_dcp_result = "analysis_dcp_result";
          const std::string occurrence_aggregation_analysis_result = "occurrence_aggregation_analysis_result";
          const std::string buffer_analysis_result = "buffer_analysis_result";
          const std::string analysis_result = "analysis_result";
          const std::string reprocessing_result = "reprocessing_result";
      }


      QJsonObject dataProviderResultAnalysisJson()
      {
          QString json = QString(R"(
                                    {
                                      "class": "DataProvider",
                                      "uri": "%1",
                                      "id": 3,
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
          return  doc.object();
      }

      terrama2::core::DataProviderPtr dataProviderResultAnalysis()
      {

          auto obj = dataProviderResultAnalysisJson();
          return terrama2::core::fromDataProviderJson(obj);
      }


      QJsonObject  dataSeriesResultAnalysisPostGisJson(terrama2::core::DataProviderPtr dataProvider,
                                                       std::string nameTableResult,
                                                       terrama2::core::DataSeriesPtr dataSeries)
      {

          QString json = QString(R"(
                                          {
                                              "class": "DataSeries",
                                              "id": 4,
                                              "name": "Analysis result",
                                              "description": null,
                                              "data_provider_id":  %1,
                                              "semantics": "ANALYSIS_MONITORED_OBJECT-postgis",
                                              "active": true,
                                              "datasets": [
                                                {
                                                  "class": "DataSet",
                                                  "id": 4,
                                                  "data_series_id": 4,
                                                  "active": true,
                                                  "format": {
                                                      "table_name": "%2",
                                                      "monitored_object_id": "%3",
                                                      "monitored_object_pk": "uf"
                                                  }
                                                }
                                              ]
                                          }
                                          )"
                                      ).arg(dataProvider->id).arg(QString::fromStdString(nameTableResult)).arg(dataSeries->id);



          QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
          return doc.object();
      }

      terrama2::core::DataSeriesPtr dataSeriesResultAnalysisPostGis(terrama2::core::DataProviderPtr dataProvider,
                                                                    std::string nameTableResult,
                                                                    terrama2::core::DataSeriesPtr dataSeries)
      {
          auto obj = dataSeriesResultAnalysisPostGisJson(dataProvider, nameTableResult, dataSeries);
          return terrama2::core::fromDataSeriesJson(obj);
      }


   } // end namespace resultanalysis
}  // end namespace terrama2

#endif  //__TERRAMA2_EXAMPLES_RESULTANALYSISPOSTGIS_HPP__
