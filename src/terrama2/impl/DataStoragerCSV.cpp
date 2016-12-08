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
  \file terrama2/core/data-access/DataStoragerCSV.cpp

  \brief

  \author Jano Simas
 */

#include "DataStoragerCSV.hpp"

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

void terrama2::core::DataStoragerCSV::store(DataSetSeries series, DataSetPtr outputDataSet) const
{
  if(!dataProvider_)
  {
    QString errMsg = QObject::tr("Invalid data provider");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataProviderException() << ErrorDescription(errMsg);
  }

  std::string destinationDataSetName = getMask(outputDataSet);

  te::core::URI uri(dataProvider_->uri+"/"+destinationDataSetName+".csv?&DRIVER=CSV");
  std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("OGR", uri));

  OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);
  // FIXME: verify if it's open
  // there is a bug is the method that returns false, check if it's fixed

  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);

  std::shared_ptr<te::da::DataSetType> datasetType = series.teDataSetType;


  std::map<std::string, std::string> options;
  std::shared_ptr<te::da::DataSetType> newDataSetType;
  if (!transactorDestination->dataSetExists(destinationDataSetName))
  {
    // create and save datasettype in the datasource destination
    newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(datasetType->clone()));
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
    auto it = std::find_if(oldPropertiesList.cbegin(), oldPropertiesList.cend(), std::bind(&terrama2::core::DataStoragerCSV::isPropertyEqual, this, property, std::placeholders::_1));
    if(it == oldPropertiesList.cend())
      transactorDestination->addProperty(newDataSetType->getName(), property);
  }

  series.syncDataSet->dataset()->moveBeforeFirst();
  transactorDestination->add(newDataSetType->getName(), series.syncDataSet->dataset().get(), options);

  scopedTransaction.commit();
}

terrama2::core::DataStoragerPtr terrama2::core::DataStoragerCSV::make(DataProviderPtr dataProvider)
{
  return std::make_shared<DataStoragerCSV>(dataProvider);
}


std::string terrama2::core::DataStoragerCSV::getMask(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("mask");
  }
  catch (...)
  {
    QString errMsg = QObject::tr("Undefined table name in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

bool terrama2::core::DataStoragerCSV::isPropertyEqual(te::dt::Property* newProperty, te::dt::Property* oldMember) const
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

std::string terrama2::core::DataStoragerCSV::getCompleteURI(DataSetPtr outputDataSet) const
{
  std::string destinationDataSetName = getMask(outputDataSet);
  return dataProvider_->uri + "/" + destinationDataSetName;
}
