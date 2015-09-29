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
  \file terrama2/collector/StoragerPostgis.cpp

  \brief Store a temporary terralib DataSet into the permanent PostGis storage area.

  \author Jano Simas
*/

#include "StoragerPostgis.hpp"

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

void terrama2::collector::Storager::store(const std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                                          const std::vector<std::shared_ptr<te::da::DataSetType> > &datasetTypeVec)
{
  assert(datasetVec.size() == datasetTypeVec.size());
  assert(datasetVec.size() == 1);//TODO: remove this!

  const std::shared_ptr<te::da::DataSet> tempDataSet = datasetVec.at(0);
  const std::shared_ptr<te::da::DataSetType> tempDataSetType = datasetTypeVec.at(0);

  // let's open the destination datasource
  std::map<std::string, std::string> pgisInfo;
  pgisInfo["PG_HOST"] = "localhost";
  pgisInfo["PG_PORT"] = "5432";
  pgisInfo["PG_USER"] = "postgres";
  pgisInfo["PG_PASSWORD"] = "postgres";
  pgisInfo["PG_DB_NAME"]  = "terrama2";
  pgisInfo["PG_CONNECT_TIMEOUT"] = "4";
  pgisInfo["PG_CLIENT_ENCODING"] = "UTF8";
  std::string dataSetScheme = "terrama2";
  std::string dataSetName   = "nome_teste";

  if(!dataSetScheme.empty())
    dataSetName = dataSetScheme+"."+dataSetName;

  std::auto_ptr<te::da::DataSource> dsDestination = te::da::DataSourceFactory::make("POSTGIS");
  dsDestination->setConnectionInfo(pgisInfo);
  dsDestination->open();

  // get a transactor to interact to the data source
  std::shared_ptr<te::da::DataSourceTransactor> tDestination(dsDestination->getTransactor());
  if (tDestination->dataSetExists(dataSetName))
  {
    assert(0);
    //TODO: what to do?
  }

  // create and save datasettype in the datasource destination
  te::da::DataSetType* newDataSet = static_cast<te::da::DataSetType*>(tempDataSetType->clone());
  newDataSet->setName(dataSetName);

  //Get original geometry to get srid
  te::gm::GeometryProperty* geom = GetFirstGeomProperty(tempDataSetType.get());
  if(geom)
    GetFirstGeomProperty(newDataSet)->setSRID(geom->getSRID());
  else
    GetFirstGeomProperty(newDataSet)->setSRID(4326);
  GetFirstGeomProperty(newDataSet)->setGeometryType(te::gm::GeometryType);


  std::map<std::string, std::string> options;

  tDestination->begin();
  tDestination->createDataSet(newDataSet,options);
  tDestination->add(newDataSet->getName(), tempDataSet.get(),options);
  tDestination->commit();

  return ;
}
