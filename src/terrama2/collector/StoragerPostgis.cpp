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
#include "Utils.hpp"

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

//Qt
#include <QDebug>
#include <QObject>
#include <QUrl>

terrama2::collector::StoragerPostgis::StoragerPostgis(const std::map<std::string, std::string>& storageMetadata)
  : Storager(storageMetadata)
{

}

void terrama2::collector::StoragerPostgis::commitData(const std::string& destinationDataSetName,
                                                      std::shared_ptr<te::da::DataSource> datasourceDestination,
                                                      const std::shared_ptr<te::da::DataSetType> &dataSetType,
                                                      const std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec)
{
  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);



  std::map<std::string, std::string> options;
  std::shared_ptr<te::da::DataSetType> newDataSetType;

  if (!transactorDestination->dataSetExists(destinationDataSetName))
  {
    // create and save datasettype in the datasource destination
    newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(dataSetType->clone()));

    newDataSetType->setName(destinationDataSetName);
    transactorDestination->createDataSet(newDataSetType.get(),options);
  }
  else
  {
    newDataSetType = transactorDestination->getDataSetType(destinationDataSetName);
  }

  //Get original geometry to get srid
  te::gm::GeometryProperty* geom = GetFirstGeomProperty(dataSetType.get());
  //configure if there is a geometry property
  if(geom)
  {
    GetFirstGeomProperty(newDataSetType.get())->setSRID(geom->getSRID());
    GetFirstGeomProperty(newDataSetType.get())->setGeometryType(te::gm::GeometryType);
  }

  for(const auto& tempDataSet : datasetVec)
    transactorDestination->add(newDataSetType->getName(), tempDataSet.get(), options);

  scopedTransaction.commit();
}

std::string terrama2::collector::StoragerPostgis::store(const std::string& standardDataSetName,
                                                 const std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                                                 const std::shared_ptr<te::da::DataSetType> &dataSetType)
{
  QUrl uri;

  try
  {
    std::string dataSetName = standardDataSetName;

    std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("POSTGIS"));
    datasourceDestination->setConnectionInfo(storageMetadata_);
    OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination);

    std::map<std::string, std::string>::const_iterator dataSetNameIt = storageMetadata_.find("PG_TABLENAME");
    if(dataSetNameIt != storageMetadata_.end())
    {
      // let's open the destination datasource
      std::map<std::string, std::string>::const_iterator schemeIt = storageMetadata_.find("PG_SCHEME");
      if(schemeIt != storageMetadata_.end())
        dataSetName = schemeIt->second+"."+dataSetName;
    }

    // get a transactor to interact to the data source
    commitData(dataSetName, datasourceDestination, dataSetType, datasetVec);

    std::map<std::string, std::string>::const_iterator it_end = storageMetadata_.end();

    uri.setScheme("postgis");
    uri.setHost(QString::fromStdString((storageMetadata_.find("PG_HOST") != it_end) ? storageMetadata_.find("PG_HOST")->second : ""));
    uri.setPort(std::stoi((storageMetadata_.find("PG_PORT") != it_end) ? storageMetadata_.find("PG_PORT")->second : ""));
    uri.setUserName(QString::fromStdString((storageMetadata_.find("PG_USER") != it_end) ? storageMetadata_.find("PG_USER")->second : ""));
    uri.setPassword(QString::fromStdString((storageMetadata_.find("PG_PASSWORD") != it_end) ? storageMetadata_.find("PG_PASSWORD")->second : ""));
    QString path = "/" + QString::fromStdString(((storageMetadata_.find("PG_DB_NAME") != it_end) ? storageMetadata_.find("PG_DB_NAME")->second + "." + dataSetName : "." + dataSetName));
    uri.setPath(path);
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
    qDebug() << e.what();
    assert(0);
  }
  catch(std::exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
    assert(0);
  }

  return uri.url().toStdString();
}
