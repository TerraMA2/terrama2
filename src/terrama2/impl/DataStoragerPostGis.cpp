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
  \file terrama2/core/data-access/DataStoragerPostGis.cpp

  \brief

  \author Jano Simas
 */

#include "DataStoragerPostGis.hpp"

#include "../core/data-model/DataProvider.hpp"
#include "../core/utility/Raii.hpp"

//terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

//Qt
#include <QUrl>

//STL
#include <algorithm>

void terrama2::core::DataStoragerPostGis::store(DataSetSeries series, DataSetPtr outputDataSet) const
{
  if(!dataProvider_)
  {
    QString errMsg = QObject::tr("Invalid data provider");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataProviderException() << ErrorDescription(errMsg);
  }

  QUrl url(dataProvider_->uri.c_str());

  std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("POSTGIS"));
  std::map<std::string, std::string> connInfo{{"PG_HOST", url.host().toStdString()},
                                              {"PG_PORT", std::to_string(url.port())},
                                              {"PG_USER", url.userName().toStdString()},
                                              {"PG_PASSWORD", url.password().toStdString()},
                                              {"PG_DB_NAME", url.path().section("/", 1, 1).toStdString()},
                                              {"PG_CONNECT_TIMEOUT", "4"},
                                              {"PG_CLIENT_ENCODING", "UTF-8"}};
  datasourceDestination->setConnectionInfo(connInfo);

  OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);
  if(!datasourceDestination->isOpened())
  {
    QString errMsg = QObject::tr("Could not connect to database");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataProviderException() << ErrorDescription(errMsg);
  }

  std::string destinationDataSetName = getDataSetTableName(outputDataSet);

  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);

  std::shared_ptr<te::da::DataSetType> datasetType = series.teDataSetType;


  std::map<std::string, std::string> options;
  std::shared_ptr<te::da::DataSetType> newDataSetType;
  if (!transactorDestination->dataSetExists(destinationDataSetName))
  {
    // create and save datasettype in the datasource destination
    newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(datasetType->clone()));
    if(!newDataSetType->getPrimaryKey())
    {
      std::string pkName = "\""+newDataSetType->getName()+"_pk\"";
      auto pk = new te::da::PrimaryKey(pkName, newDataSetType.get());

      te::dt::SimpleProperty* serialPk = new te::dt::SimpleProperty("pid", te::dt::INT32_TYPE, true);
      serialPk->setAutoNumber(true);
      newDataSetType->add(serialPk);
      pk->add(serialPk);
    }

    newDataSetType->setName(destinationDataSetName);
    transactorDestination->createDataSet(newDataSetType.get(),options);

    //Get original geometry to get srid
    te::gm::GeometryProperty* geom = GetFirstGeomProperty(datasetType.get());
    //configure if there is a geometry property
    if(geom)
    {
      GetFirstGeomProperty(newDataSetType.get())->setSRID(geom->getSRID());
      GetFirstGeomProperty(newDataSetType.get())->setGeometryType(te::gm::GeometryType);
    }

  }
  else
  {
    newDataSetType = transactorDestination->getDataSetType(destinationDataSetName);
  }

  const auto& oldPropertiesList = newDataSetType->getProperties();
  for(const auto & property : datasetType->getProperties())
  {
    auto it = std::find_if(oldPropertiesList.cbegin(), oldPropertiesList.cend(), std::bind(&terrama2::core::DataStoragerPostGis::isPropertyEqual, this, property, std::placeholders::_1));
    if(it == oldPropertiesList.cend())
      transactorDestination->addProperty(newDataSetType->getName(), property);
  }

  series.syncDataSet->dataset()->moveBeforeFirst();
  transactorDestination->add(newDataSetType->getName(), series.syncDataSet->dataset().get(), options);

  scopedTransaction.commit();
}

terrama2::core::DataStoragerPtr terrama2::core::DataStoragerPostGis::make(DataProviderPtr dataProvider)
{
  return std::make_shared<DataStoragerPostGis>(dataProvider);
}


std::string terrama2::core::DataStoragerPostGis::getDataSetTableName(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("table_name");
  }
  catch (...)
  {
    QString errMsg = QObject::tr("Undefined table name in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

bool terrama2::core::DataStoragerPostGis::isPropertyEqual(te::dt::Property* newProperty, te::dt::Property* oldMember) const
{
  std::string newPropertyName = newProperty->getName();
  std::transform(newPropertyName.begin(), newPropertyName.end(), newPropertyName.begin(), ::tolower);

  std::string oldPropertyName = oldMember->getName();
  std::transform(oldPropertyName.begin(), oldPropertyName.end(), oldPropertyName.begin(), ::tolower);

  bool noEqual = newPropertyName == oldPropertyName;
  if(!noEqual)
    return false;

  if(newProperty->getType() != oldMember->getType())
  {
    QString errMsg = QObject::tr("Wrong column type: %1").arg(QString::fromStdString(newProperty->getName()));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  return true;
}

std::string terrama2::core::DataStoragerPostGis::getCompleteURI(DataSetPtr outputDataSet) const
{
  std::string destinationDataSetName = getDataSetTableName(outputDataSet);
  return dataProvider_->uri + "/" + destinationDataSetName;
}