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
  \file terrama2/core/data-access/DataStoragerDCPPostGIS.cpp

  \brief

  \author Jano Simas
 */

//TerraMA2
#include "DataStoragerDCPPostGIS.hpp"

#include "../core/data-model/DataProvider.hpp"
#include "../core/data-model/DataSetDcp.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Utils.hpp"

#include <terralib/dataaccess/datasource/DataSourceCapabilities.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/core/uri/URI.h>
#include <terralib/datatype/SimpleData.h>

//Boost
#include <boost/algorithm/string.hpp>

#include <set>

terrama2::core::DataStoragerPtr terrama2::core::DataStoragerDCPPostGIS::make(DataSeriesPtr dataSeries, DataProviderPtr dataProvider)
{
  return std::make_shared<DataStoragerDCPPostGIS>(dataSeries, dataProvider);
}

void terrama2::core::DataStoragerDCPPostGIS::store(const std::unordered_map<DataSetPtr,DataSetSeries >&  dataMap,
                                                   const std::vector< DataSetPtr >& dataSetLst,
                                                   const std::map<DataSetId, DataSetId>& inputOutputMap) const
{
  storePositions(dataMap, dataSetLst, inputOutputMap);
  DataStorager::store(dataMap, dataSetLst, inputOutputMap);
}

void terrama2::core::DataStoragerDCPPostGIS::storePositions(const std::unordered_map<DataSetPtr,DataSetSeries >&  dataMap,
                                                            const std::vector< DataSetPtr >& dataSetLst,
                                                            const std::map<DataSetId, DataSetId>& inputOutputMap) const
{
  // open connection
  te::core::URI uri(dataProvider_->uri);
  std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make(driver(), uri));
  OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);
  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);

  // Check datasource capabilities
  auto capabilities = datasourceDestination->getCapabilities();
  auto typeCapabilities = capabilities.getDataSetTypeCapabilities();

  std::shared_ptr<te::mem::DataSet> dataset;
  // DCP positions table name
  std::string destinationDataSetName = getDCPPositionsTableName(dataSeries_);
  std::shared_ptr<te::da::DataSetType> newDataSetType;

  std::set<DataSetId> idSet;
  if (transactorDestination->dataSetExists(destinationDataSetName))
  {
    newDataSetType = transactorDestination->getDataSetType(destinationDataSetName);
    dataset = std::make_shared<te::mem::DataSet>(newDataSetType.get());

    std::unique_ptr<te::da::DataSet> inDataBaseDataset(transactorDestination->getDataSet(destinationDataSetName));

    auto pos = newDataSetType->getPropertyPosition(ID_PROPERTY_NAME);

    inDataBaseDataset->moveBeforeFirst();
    while(inDataBaseDataset->moveNext())
    {
      idSet.insert(static_cast<DataSetId>(inDataBaseDataset->getInt32(pos)));
    }
  }
  else
  {
    newDataSetType =  std::make_shared<te::da::DataSetType>(destinationDataSetName);

    te::dt::SimpleProperty* serialPk = new te::dt::SimpleProperty(ID_PROPERTY_NAME, te::dt::INT32_TYPE, true);
    newDataSetType->add(serialPk);

    //add primary key if allowed
    if(typeCapabilities.supportsPrimaryKey() && !newDataSetType->getPrimaryKey())
    {
      std::string pkName = "\""+destinationDataSetName+"_pk\"";
      auto pk = new te::da::PrimaryKey(pkName, newDataSetType.get());
      pk->add(serialPk);
    }

  // create properties
    auto geomProperty = new te::gm::GeometryProperty(GEOM_PROPERTY_NAME, true);
    geomProperty->setGeometryType(te::gm::PointType);
    geomProperty->setSRID(4326);
    newDataSetType->add(geomProperty);
    auto teableNameProperty = new te::dt::StringProperty(TABLE_NAME_PROPERTY_NAME);
    newDataSetType->add(teableNameProperty);

    auto aliasProperty = new te::dt::StringProperty(ALIAS_PROPERTY_NAME);
    newDataSetType->add(aliasProperty);

    //create dataset
    newDataSetType->setName(destinationDataSetName);
    transactorDestination->createDataSet(newDataSetType.get(), {});
    dataset = std::make_shared<te::mem::DataSet>(newDataSetType.get());
  }

  for(const auto& item : dataMap)
  {
    DataSetId inputDataSetId = item.first->id;
    // read each item
    DataSetId outputDataSetId = inputOutputMap.at(inputDataSetId);

    // if already in the table, continue
    if(idSet.find(outputDataSetId) != std::end(idSet))
      continue;

    auto outputDataSet = std::find_if(dataSetLst.cbegin(), dataSetLst.cend(), [outputDataSetId](terrama2::core::DataSetPtr dataSet) { return dataSet->id == outputDataSetId; });
    auto dataSetDcp = std::static_pointer_cast<const DataSetDcp>(*outputDataSet);

    std::string destinationDataSetName = boost::to_lower_copy(getDataSetName(dataSetDcp));
    
    auto dataSetItem = std::unique_ptr<te::mem::DataSetItem>(new te::mem::DataSetItem(dataset.get()));
    dataSetItem->setInt32(ID_PROPERTY_NAME, static_cast<int32_t>(outputDataSetId));
    std::unique_ptr<te::gm::Geometry>locale (dynamic_cast<te::gm::Geometry*>(dataSetDcp->position->clone()));
    locale->transform(4326);
    dataSetItem->setGeometry(GEOM_PROPERTY_NAME, locale.release());
    dataSetItem->setString(TABLE_NAME_PROPERTY_NAME, destinationDataSetName);
    dataSetItem->setString(ALIAS_PROPERTY_NAME, dataSetDcp->alias());

    dataset->add(dataSetItem.release());
  }

  dataset->moveBeforeFirst();
  transactorDestination->add(newDataSetType->getName(), dataset.get(), {});
  scopedTransaction.commit();
}
