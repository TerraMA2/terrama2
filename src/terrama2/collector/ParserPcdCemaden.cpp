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
  \file terrama2/collector/ParserPcdCemaden.cpp

  \brief Parser of Pcd Cemaden

  \author Evandro Delatin
*/


// TerraMA2
#include "ParserPcdCemaden.hpp"
#include "Parser.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "../core/Logger.hpp"

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/AttributeConverters.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/geometry.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/common/Exception.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include "CurlOpener.hpp"

//QT
#include <QDebug>
#include <QObject>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QLocale>

// Boost
#include <boost/format/exceptions.hpp>

// Curl
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>
#include <sstream>
#include <iostream>

// STL
#include <memory>


size_t terrama2::collector::ParserPcdCemaden::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  std::string data((const char*) ptr, (size_t) size * nmemb);
    *((std::stringstream*) stream) << data;
    return size * nmemb;
}

void terrama2::collector::ParserPcdCemaden::read(terrama2::collector::DataFilterPtr filter, std::vector<terrama2::collector::TransferenceData>& transferenceDataVec)
{

  for(TransferenceData& transferenceData : transferenceDataVec)
  {
    QUrl uri(transferenceData.uriTemporary.c_str());

    CURLcode res;
    CurlOpener curl;
    std::stringstream block;

    curl.init();

    if (curl.fcurl())
    {
      curl_easy_setopt(curl.fcurl(), CURLOPT_URL, uri.toString().toStdString().c_str());
      curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEFUNCTION, &terrama2::collector::ParserPcdCemaden::write_data);
      curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEDATA, &block);
      res = curl_easy_perform(curl.fcurl());

      if (res != CURLE_OK)
      {
        QString errMsg = QObject::tr("Could not perform the download. \n\n");
        errMsg.append(curl_easy_strerror(res));

        TERRAMA2_LOG_ERROR() << errMsg;
        throw ParserPcdCemadenException() << ErrorDescription(errMsg);
      }
      else
      {
        QJsonDocument jdoc;
        jdoc = QJsonDocument::fromJson(block.str().c_str());
        QJsonObject metadata = jdoc.object();
        QJsonArray cemaden = metadata["cemaden"].toArray();

        const int CODESTACAO = 0;
        const int GEOM = 1;
        const int CIDADE = 2;
        const int NOME = 3;
        const int TIPO = 4;
        const int UF = 5;
        const int CHUVA = 6;
        const int NIVEL = 7;
        const int DATAHORA = 8;

        double x, y;

        std::shared_ptr<te::da::DataSetType> datasetType(new te::da::DataSetType("datasettype"));

        datasetType->add( new te::dt::SimpleProperty("codestacao", te::dt::STRING_TYPE, true) );
        datasetType->add( new te::gm::GeometryProperty("geom", 4326, te::gm::PointType, true) );
        datasetType->add( new te::dt::SimpleProperty("cidade", te::dt::STRING_TYPE, true) );
        datasetType->add( new te::dt::SimpleProperty("nome", te::dt::STRING_TYPE, true) );
        datasetType->add( new te::dt::SimpleProperty("tipo", te::dt::STRING_TYPE, true) );
        datasetType->add( new te::dt::SimpleProperty("uf", te::dt::STRING_TYPE, true) );
        datasetType->add( new te::dt::SimpleProperty("chuva", te::dt::DOUBLE_TYPE, true) );
        datasetType->add( new te::dt::SimpleProperty("nivel", te::dt::DOUBLE_TYPE, true) );
        datasetType->add( new te::dt::SimpleProperty("datahora", te::dt::DATETIME_TYPE, true) );

        std::shared_ptr<te::mem::DataSet> dataSet(new te::mem::DataSet(datasetType.get()));

        for (auto&& element: cemaden)
        {
          te::mem::DataSetItem* dataSetItem = new te::mem::DataSetItem(dataSet.get());

          const QJsonObject& cemadenItem = element.toObject();

          dataSetItem->setString(CODESTACAO,cemadenItem["codestacao"].toString().toStdString().c_str());

          x = cemadenItem["latitude"].toDouble();
          y = cemadenItem["longitude"].toDouble();
          te::gm::Point* point = new te::gm::Point(x,y,4326);
          dataSetItem->setGeometry(GEOM,point);

          dataSetItem->setString(CIDADE,cemadenItem["cidade"].toString().toStdString().c_str());
          dataSetItem->setString(NOME,cemadenItem["nome"].toString().toStdString().c_str());
          dataSetItem->setString(TIPO,cemadenItem["tipo"].toString().toStdString().c_str());
          dataSetItem->setString(UF,cemadenItem["uf"].toString().toStdString().c_str());
          dataSetItem->setDouble(CHUVA,cemadenItem["chuva"].toDouble());
          dataSetItem->setDouble(NIVEL,cemadenItem["nivel"].toDouble());

          boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(cemadenItem["dataHora"].toString().toStdString().c_str()));
          boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(dataSetItem_.timezone()));
          boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);
          te::dt::TimeInstantTZ* dt = new te::dt::TimeInstantTZ(date);

          dataSetItem->setDateTime(DATAHORA,dt);

          dataSet->add(dataSetItem);
        }

        transferenceData.teDataSet = dataSet;
      }
    }
  }
}
