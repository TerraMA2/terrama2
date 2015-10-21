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
#include <QDebug>

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

    std::string dataSetName   = storageMetadata_.at("PG_TABLENAME");

    // let's open the destination datasource
    std::map<std::string, std::string>::const_iterator schemeIt = storageMetadata_.find("PG_SCHEME");
    if(schemeIt != storageMetadata_.end())
        dataSetName = schemeIt->second+"."+dataSetName;

    std::auto_ptr<te::da::DataSource> datasourceDestination = te::da::DataSourceFactory::make("POSTGIS");
    datasourceDestination->setConnectionInfo(storageMetadata_);
    datasourceDestination->open();

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
    transactorDestination->begin();

    std::map<std::string, std::string> options;
    std::shared_ptr<te::da::DataSetType> newDataSetType;

    if (!transactorDestination->dataSetExists(dataSetName))
    {
      // create and save datasettype in the datasource destination
      newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(dataSetType->clone()));

      newDataSetType->setName(dataSetName);
      transactorDestination->createDataSet(newDataSetType.get(),options);
    }
    else
    {
      newDataSetType = transactorDestination->getDataSetType(dataSetName);
    }

    //Get original geometry to get srid
    te::gm::GeometryProperty* geom = GetFirstGeomProperty(dataSetType.get());
    //configure if there is a geometry property
    if(geom)
    {
      GetFirstGeomProperty(newDataSetType.get())->setSRID(geom->getSRID());
      GetFirstGeomProperty(newDataSetType.get())->setGeometryType(te::gm::GeometryType);
    }

    transactorDestination->add(newDataSetType->getName(), tempDataSet.get(), options);
    transactorDestination->commit();
  }
  catch(terrama2::Exception& e)
  {
    //TODO: log de erro
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    assert(0);
  }
  catch(te::common::Exception& e)
  {
    //TODO: log de erro
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    assert(0);
  }
  catch(...)
  {
    //TODO: log de erro
    assert(0);
  }

  return ;
}
