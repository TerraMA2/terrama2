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
  \file terrama2/core/data-access/DataStoragerTable.cpp

  \brief

  \author Jano Simas
 */

#include "DataStoragerTable.hpp"

#include "../core/data-model/DataProvider.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Utils.hpp"

//terralib
#include <terralib/dataaccess/datasource/DataSourceCapabilities.h>
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

//Boost
#include <boost/algorithm/string.hpp>

void terrama2::core::DataStoragerTable::store(DataSetSeries series, DataSetPtr outputDataSet) const
{
  if(!dataProvider_)
  {
    QString errMsg = QObject::tr("Invalid data provider");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataProviderException() << ErrorDescription(errMsg);
  }

  te::core::URI uri(getCompleteURI(outputDataSet));
  std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make(driver(), uri));

  OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);
  // if(!datasourceDestination->isOpened())
  // {
  //   QString errMsg = QObject::tr("Could not connect to database");
  //   TERRAMA2_LOG_ERROR() << errMsg;
  //   throw DataProviderException() << ErrorDescription(errMsg);
  // }

  // FIXME: verify if it's open
  // there is a bug is the method for CSV that returns false, check if it's fixed

  auto capabilities = datasourceDestination->getCapabilities();
  auto typeCapabilities = capabilities.getDataSetTypeCapabilities();

  std::string destinationDataSetName = boost::to_lower_copy(getDataSetName(outputDataSet));

  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);

  std::shared_ptr<te::da::DataSetType> datasetType = series.teDataSetType;

  std::shared_ptr<te::da::DataSetType> newDataSetType;
  if (!transactorDestination->dataSetExists(destinationDataSetName))
  {
    // create and save datasettype in the datasource destination
    newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(datasetType->clone()));

    if(typeCapabilities.supportsPrimaryKey() && !newDataSetType->getPrimaryKey())
    {
      std::string pkName = "\""+destinationDataSetName+"_pk\"";
      std::unique_ptr<te::da::PrimaryKey> pk(new te::da::PrimaryKey(pkName));

      std::unique_ptr<te::dt::SimpleProperty> serialPk(new te::dt::SimpleProperty ("pid", te::dt::INT32_TYPE, true));
      serialPk->setAutoNumber(true);
      pk->add(serialPk.get());
      newDataSetType->add(pk.release());
      newDataSetType->add(serialPk.release());
    }

    //Get original geometry to get srid
    te::gm::GeometryProperty* geom = GetFirstGeomProperty(datasetType.get());
    //configure if there is a geometry property
    if(geom)
    {
      auto geomPropertyName = getGeometryPropertyName(outputDataSet);
      te::gm::GeometryProperty* geomProperty = dynamic_cast<te::gm::GeometryProperty*>(newDataSetType->getProperty(geomPropertyName));
      geomProperty->setSRID(geom->getSRID());
      geomProperty->setGeometryType(geom->getGeometryType());

      //there is a limit in the size of the dataset that we can create an index
      if(typeCapabilities.supportsBTreeIndex() && series.syncDataSet->size() < 2712)
      {
        // the newDataSetType takes ownership of the pointer
        auto spatialIndex = new te::da::Index("spatial_index_" + destinationDataSetName, te::da::B_TREE_TYPE, {geomProperty});
        newDataSetType->add(spatialIndex);
      }
    }

    newDataSetType->setName(destinationDataSetName);
    transactorDestination->createDataSet(newDataSetType.get(), {});
  }
  else
  {
    newDataSetType = transactorDestination->getDataSetType(destinationDataSetName);
  }

  const auto& oldPropertiesList = newDataSetType->getProperties();
  for(const auto & property : datasetType->getProperties())
  {
    auto it = std::find_if(oldPropertiesList.cbegin(), oldPropertiesList.cend(), std::bind(&terrama2::core::DataStoragerTable::isPropertyEqual, this, property, std::placeholders::_1));
    if(it == oldPropertiesList.cend())
      transactorDestination->addProperty(newDataSetType->getName(), property);
  }

  series.syncDataSet->dataset()->moveBeforeFirst();
  transactorDestination->add(newDataSetType->getName(), series.syncDataSet->dataset().get(), {});

  scopedTransaction.commit();
}

std::string terrama2::core::DataStoragerTable::getGeometryPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "geometry_property");
}

bool terrama2::core::DataStoragerTable::isPropertyEqual(te::dt::Property* newProperty, te::dt::Property* oldMember) const
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
