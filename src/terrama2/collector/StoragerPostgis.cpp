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
#include "../core/Logger.hpp"

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
                                                      std::vector<TransferenceData>& transferenceDataVec)
{
  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);

  std::shared_ptr<te::da::DataSetType> datasetType = transferenceDataVec.at(0).teDatasetType;

  std::map<std::string, std::string> options;
  std::shared_ptr<te::da::DataSetType> newDataSetType;


  if (!transactorDestination->dataSetExists(destinationDataSetName))
  {
    // create and save datasettype in the datasource destination
    newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(datasetType->clone()));

    newDataSetType->setName(destinationDataSetName);
    transactorDestination->createDataSet(newDataSetType.get(),options);
  }
  else
  {
    newDataSetType = transactorDestination->getDataSetType(destinationDataSetName);
  }

  //Get original geometry to get srid
  te::gm::GeometryProperty* geom = GetFirstGeomProperty(datasetType.get());
  //configure if there is a geometry property
  if(geom)
  {
    GetFirstGeomProperty(newDataSetType.get())->setSRID(geom->getSRID());
    GetFirstGeomProperty(newDataSetType.get())->setGeometryType(te::gm::GeometryType);
  }

  for(const auto& transferenceData : transferenceDataVec)
    transactorDestination->add(newDataSetType->getName(), transferenceData.teDataset.get(), options);

  scopedTransaction.commit();
}

void terrama2::collector::StoragerPostgis::store(std::vector<TransferenceData>& transferenceDataVec)
{
  if(transferenceDataVec.empty())
    return;

  try
  {
    const core::DataSetItem& dataSetItem = transferenceDataVec.at(0).datasetItem;

    std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("POSTGIS"));
    datasourceDestination->setConnectionInfo(storageMetadata_);
    OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);
    if(!datasourceDestination->isOpened())
      return; //TODO: throw exception...

    std::string dataSetName;
    std::map<std::string, std::string>::const_iterator dataSetNameIt = storageMetadata_.find("STORAGE_NAME");
    if(dataSetNameIt != storageMetadata_.end())
      dataSetName = dataSetNameIt->second;
    else
    {
      dataSetName = "terrama2.storager_";
      dataSetName.append(std::to_string(dataSetItem.id()));
    }

    // get a transactor to interact to the data source
    commitData(dataSetName, datasourceDestination, transferenceDataVec);

    QUrl uri;
    uri.setScheme("postgis");
    uri.setHost(QString::fromStdString(storageMetadata_.at("PG_HOST")));
    uri.setPort(std::stoi(storageMetadata_.at("PG_PORT")));
    uri.setUserName(QString::fromStdString(storageMetadata_.at("PG_USER")));
    uri.setPassword(QString::fromStdString(storageMetadata_.at("PG_PASSWORD")));
    QString path = "/" + QString::fromStdString(storageMetadata_.at("PG_DB_NAME") + "/" + dataSetName);
    uri.setPath(path);

    //update storage uri
    for(TransferenceData& transferenceData : transferenceDataVec)
      transferenceData.uri_storage = uri.url().toStdString();
  }
  catch(terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
  }
  catch(te::common::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }

  return;
}
