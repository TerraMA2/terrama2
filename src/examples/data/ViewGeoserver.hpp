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

#ifndef __TERRAMA2_EXAMPLES_VIEWGEOSERVER_HPP__
#define __TERRAMA2_EXAMPLES_VIEWGEOSERVER_HPP__


namespace terrama2
{

/**
 * @brief Namespace for metadata of geoserver tests
 * 
 */
  namespace geoserver
  {


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
                                 ).arg(QString::fromStdString("file://"+TERRAMA2_DATA_DIR));

          QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
          QJsonObject obj = doc.object();
          return terrama2::core::fromDataProviderJson(obj);

      }


      terrama2::core::DataSeriesPtr dataSeriesHumidity(terrama2::core::DataProviderPtr dataProvider)
      {

          QString json = QString(R"x(
                                      {
                                          "class": "DataSeries",
                                          "id": 9,
                                          "name": "geotiff 1",
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
                                                          "mask": "Coleta_FTP_RF/umid/BAM.umrs.%YYYY%MM%DD%hh%mm.tif",
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



   } // end namespace geoserver
}  // end namespace terrama2

#endif  //__TERRAMA2_EXAMPLES_VIEWGEOSERVER_HPP__
