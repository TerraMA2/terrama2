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
  \file terrama2/collector/ParserPostgis.cpp

  \brief Parsers postgres/postgis data and create a terralib DataSet.

  \author Jano Simas
*/

#include "ParserPostgis.hpp"

//QT
#include <QUrl>

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

std::vector<std::string> terrama2::collector::ParserPostgis::datasetNames(const std::string& uri) const
{

}

void terrama2::collector::ParserPostgis::read(const std::string& uri,
                                              terrama2::collector::DataFilterPtr filter,
                                              std::vector<std::shared_ptr<te::da::DataSet> >& datasetVec,
                                              std::shared_ptr<te::da::DataSetType>& datasetTypePtr)
{
  QUrl url(uri.c_str());

  std::map<std::string, std::string> storageMetadata{ {"KIND", "POSTGIS"},
                                                      {"PG_HOST", url.host().toStdString()},
                                                      {"PG_PORT", std::to_string(url.port())},
                                                      {"PG_USER", url.userName().toStdString()},
                                                      {"PG_PASSWORD", url.password().toStdString()},
                                                      {"PG_DB_NAME", url.path().toStdString()},
                                                      {"PG_CONNECT_TIMEOUT", "4"},
                                                      {"PG_CLIENT_ENCODING", "UTF-8"}
                                                    };

  std::auto_ptr<te::da::DataSource> datasourceDestination = te::da::DataSourceFactory::make("POSTGIS");
  datasourceDestination->setConnectionInfo(storageMetadata);
  datasourceDestination->open();


  // get a transactor to interact to the data source
  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  transactorDestination->begin();

  std::string dataSetName = "terrama2.nome_teste";
  if (!transactorDestination->dataSetExists(dataSetName))
  {
    //TODO: throw
  }

  std::shared_ptr<te::da::DataSet>      dataSet     = transactorDestination->getDataSet(dataSetName);
  std::shared_ptr<te::da::DataSetType>  dataSetType = transactorDestination->getDataSetType(dataSetName);

  datasetVec.push_back(dataSet);
  datasetTypePtr = dataSetType;

//                                                      {"PG_SCHEME", "terrama2"},
//                                                      {"PG_TABLENAME", "nome_teste"} };

}
