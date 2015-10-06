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
#include "Exception.hpp"

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

//Qt
#include <QObject>

terrama2::collector::StoragerPostgis::StoragerPostgis(const std::map<std::string, std::string>& storageMetadata)
  : Storager(storageMetadata)
{

}

void terrama2::collector::StoragerPostgis::store(const std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                                                 const std::shared_ptr<te::da::DataSetType> &dataSetType)
{
  assert(datasetVec.size() == 1);//TODO: remove this!

  try
  {
    const std::shared_ptr<te::da::DataSet> tempDataSet = datasetVec.at(0);

    // let's open the destination datasource
    std::string dataSetScheme = storageMetadata_.at("PG_SCHEME");
    std::string dataSetName   = storageMetadata_.at("PG_TABLENAME");

    if(!dataSetScheme.empty())
      dataSetName = dataSetScheme+"."+dataSetName;

    std::auto_ptr<te::da::DataSource> datasourceDestination = te::da::DataSourceFactory::make("POSTGIS");
    datasourceDestination->setConnectionInfo(storageMetadata_);
    datasourceDestination->open();

    // create and save datasettype in the datasource destination
    te::da::DataSetType* newDataSet = static_cast<te::da::DataSetType*>(dataSetType->clone());
    newDataSet->setName(dataSetName);
    std::map<std::string, std::string> options;

    //Get original geometry to get srid
    te::gm::GeometryProperty* geom = GetFirstGeomProperty(dataSetType.get());
    //configure if there is a geometry property
    if(geom)
    {
      GetFirstGeomProperty(newDataSet)->setSRID(geom->getSRID());
      GetFirstGeomProperty(newDataSet)->setGeometryType(te::gm::GeometryType);
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
    transactorDestination->begin();

    if (!transactorDestination->dataSetExists(dataSetName))
      transactorDestination->createDataSet(newDataSet,options);

    transactorDestination->add(newDataSet->getName(), tempDataSet.get(), options);
    transactorDestination->commit();
  }
  catch(te::common::Exception& e)
  {
    throw StoragerConnectionError() << terrama2::ErrorDescription(QObject::tr("Terralib exception: %1").arg(e.what()));
  }

  return ;
}
