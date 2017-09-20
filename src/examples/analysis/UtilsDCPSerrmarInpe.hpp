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
  \file terrama2/analysis/UtilsDCPSerrmarInpe.hpp

  \brief Specific exception classes from core module.

  \author Bianca Maciel, Jano Simas
 */


#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include <QJsonDocument>
#include <QJsonObject>

#ifndef __TERRAMA2_ANALYSIS_UTILSDCPSERRMARINPE_HPP__
#define __TERRAMA2_ANALYSIS_UTILSDCPSERRMARINPE_HPP__


namespace terrama2
{
  namespace examples
  {

  namespace analysis
  {

  namespace utilsdcpserrmarinpe
  {

    terrama2::core::DataProviderPtr dataProvider()
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
                               ).arg(QString::fromStdString(TERRAMA2_DATA_DIR + "/PCD_serrmar_INPE"));

        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        QJsonObject obj = doc.object();
        return terrama2::core::fromDataProviderJson(obj);

    }

    terrama2::core::DataSeriesPtr dataSeries(terrama2::core::DataProviderPtr dataProviderId)
    {

        QString json = QString(R"x(
                                    {
                                        "class": "DataSeries",
                                        "id": 2,
                                        "name": "Serra do Mar",
                                        "description": "null",
                                        "data_provider_id":  %1,
                                        "semantics": "DCP-inpe",
                                        "active": true,
                                        "datasets":[
                                             {
                                                "class": "DataSet",
                                                "id": 2,
                                                "data_series_id": 2,
                                                "active": true,
                                                "position": "SRID=4618;POINT(-44.46540 -23.00506)",
                                                "format": {
                                                        "mask": "69034.txt",
                                                        "alias": "dcp_69034",
                                                        "timezone": "-02:00"
                                                }
                                             },
                                             {
                                                "class": "DataSet",
                                                "id": 3,
                                                "data_series_id": 2,
                                                "active": true,
                                                "position": "SRID=4618;POINT(-46.121 -23.758)",
                                                "format": {
                                                    "mask": "30885.txt",
                                                    "alias": "dcp_30885",
                                                    "timezone": "-02:00"
                                                }
                                             }
                                        ]
                                       }
                                     )x"
                                   ).arg(dataProviderId->id);

         QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
         QJsonObject obj = doc.object();
         return terrama2::core::fromDataSeriesJson(obj);
     }

    }  // end namespace utilsdcpserrmarinpe
   }  // end namespace analysis
  }  // end namespace examples
}  // end namespace terrama2

#endif  // __TERRAMA2_ANALYSIS_UTILSDCPSERRMARINPE_HPP__
