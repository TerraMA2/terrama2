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

//Boost
#include <boost/algorithm/string.hpp>

terrama2::core::DataStoragerPtr terrama2::core::DataStoragerDCPPostGIS::make(DataSeriesPtr dataSeries, DataProviderPtr dataProvider)
{
  return std::make_shared<DataStoragerDCPPostGIS>(dataSeries, dataProvider);
}

void terrama2::core::DataStoragerDCPPostGIS::store(const std::unordered_map<DataSetPtr,DataSetSeries >&  dataMap,
                                                   const std::vector< DataSetPtr >& dataSetLst,
                                                   const std::map<DataSetId, DataSetId>& inputOutputMap) const
{
//  storePositions(dataMap, dataSetLst, inputOutputMap);
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

  std::shared_ptr<te::mem::DataSet> dataset;
  // DCP positions table name
  std::string destinationDataSetName = "dcp_series_"+std::to_string(dataSeries_->id);
  auto newDataSetType = std::make_shared<te::da::DataSetType>(destinationDataSetName);
  if (!transactorDestination->dataSetExists(destinationDataSetName))
  {
    auto capabilities = datasourceDestination->getCapabilities();
    auto typeCapabilities = capabilities.getDataSetTypeCapabilities();

    te::dt::SimpleProperty* serialPk = new te::dt::SimpleProperty(ID_PROPERTY_NAME, te::dt::INT32_TYPE, true);
    newDataSetType->add(serialPk);

    //add primary key if allowed
    if(typeCapabilities.supportsPrimaryKey() && !newDataSetType->getPrimaryKey())
    {
      std::string pkName = "\""+destinationDataSetName+"_pk\"";
      auto pk = new te::da::PrimaryKey(pkName, newDataSetType.get());
      pk->add(serialPk);
    }

    auto geomProperty = new te::gm::GeometryProperty(GEOM_PROPERTY_NAME, true);
    geomProperty->setGeometryType(te::gm::GeometryType);
    newDataSetType->add(geomProperty);
    auto teableNameProperty = new te::dt::StringProperty(TABLE_NAME_PROPERTY_NAME);
    newDataSetType->add(teableNameProperty);

    //create dataset
    newDataSetType->setName(destinationDataSetName);
    transactorDestination->createDataSet(newDataSetType.get(), {});
    dataset = std::make_shared<te::mem::DataSet>(newDataSetType.get());
  }

  for(const auto& item : dataMap)
  {
    // read each item
    DataSetId outputDataSetId = inputOutputMap.at(item.first->id);
    auto outputDataSet = std::find_if(dataSetLst.cbegin(), dataSetLst.cend(), [outputDataSetId](terrama2::core::DataSetPtr dataSet) { return dataSet->id == outputDataSetId; });
    std::string destinationDataSetName = boost::to_lower_copy(getDataSetName(*outputDataSet));

    auto dataSetDcp = std::dynamic_pointer_cast<const DataSetDcp>(*outputDataSet);
    auto dataSetItem = std::unique_ptr<te::mem::DataSetItem>(new te::mem::DataSetItem(dataset.get()));
    dataSetItem->setInt32(ID_PROPERTY_NAME, outputDataSetId);
    dataSetItem->setGeometry(GEOM_PROPERTY_NAME, static_cast<te::gm::Geometry*>(dataSetDcp->position->clone()));
    dataSetItem->setString(TABLE_NAME_PROPERTY_NAME, destinationDataSetName);

    dataset->add(dataSetItem.release());
  }

  transactorDestination->add(newDataSetType->getName(), dataset.get(), {});
  scopedTransaction.commit();
}
