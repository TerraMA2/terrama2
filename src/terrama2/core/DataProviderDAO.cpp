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
  \file terrama2/core/DataProviderDAO.hpp

  \brief DataProvider DAO...

  \author Paulo R. M. Oliveira
*/

#include "DataProviderDAO.hpp"
#include "DataProvider.hpp"
#include "ApplicationController.hpp"
#include "Utils.hpp"
#include "../Exception.hpp"

// STL
#include <vector>
#include <memory>
#include <cstdint>

// terralib
#include <terralib/common/StringUtils.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/memory.h>
#include <terralib/memory/DataSetItem.h>


terrama2::core::DataProviderDAO::DataProviderDAO(std::shared_ptr<te::da::DataSource> dataSource)
  : dataSource_(dataSource)
{

}

terrama2::core::DataProviderDAO::~DataProviderDAO()
{

}


bool terrama2::core::DataProviderDAO::save(terrama2::core::DataProviderPtr dataProvider)
{
  std::string dataSetName = "terrama2.data_provider";

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();
  transactor->begin();

  // Removes the column id because it's an auto number
  //review
  std::auto_ptr<te::da::DataSetType> dataSetType = dataSource_->getDataSetType(dataSetName);
  std::auto_ptr<te::da::DataSetType> dataSetTypeClone(dataSetType);
  te::dt::Property* idProperty = dataSetTypeClone->getProperty(0);
  dataSetTypeClone->remove(idProperty);

  // Creates a memory dataset from the DataSetType without column id
  std::shared_ptr<te::mem::DataSet> dataSet(new te::mem::DataSet(dataSetTypeClone.get()));
  te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(dataSet.get());

  // Sets the values in the item
  dsItem->setString("name", dataProvider->name());
  dsItem->setString("description", dataProvider->description());
  dsItem->setInt32("kind", (int)dataProvider->kind());
  dsItem->setString("uri", dataProvider->uri());
  dsItem->setBool("active", DataProviderStatusToBool(dataProvider->status()));

  // Adds it to the dataset
  dataSet->add(dsItem);
  std::map<std::string, std::string> options;

  // Then, adds it to the data source
  dataSource_->add(dataSetName, dataSet.get(), options);


  // Queries generated id
  std::string sql("SELECT * FROM " + dataSetName + " WHERE name = '" + dataProvider->name() + "'");
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor->query(sql);

  // Sets the id in the given provider
  if(tempDataSet->moveNext())
  {
    dataProvider->setId(tempDataSet->getInt32("id"));
  }


  transactor->commit();

  return true;
}


bool terrama2::core::DataProviderDAO::update(terrama2::core::DataProviderPtr dataProvider)
{
  std::string dataSetName = "terrama2.data_provider";

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();
  transactor->begin();

  std::string sql = "UPDATE " + dataSetName + " SET"
      + " name='" + dataProvider->name() + "'"
      + ", description='" + dataProvider->description() + "'"
      + ", kind=" + te::common::Convert2String((int)dataProvider->kind())
      + ", uri='" + dataProvider->uri() + "'"
      + ", active=" + terrama2::core::BoolToString(DataProviderStatusToBool(dataProvider->status()))
      + " WHERE id = " + te::common::Convert2String(dataProvider->id());

  transactor->execute(sql);

  transactor->commit();

  return true;
}


bool terrama2::core::DataProviderDAO::remove(const uint64_t id)
{

  std::string dataSetName = "terrama2.data_provider";

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();
  transactor->begin();

  std::string sql = "DELETE FROM " + dataSetName
      + " WHERE id = " + te::common::Convert2String(id);

  transactor->execute(sql);

  transactor->commit();

  return false;
}


terrama2::core::DataProviderPtr terrama2::core::DataProviderDAO::find(const uint64_t id) const
{
  std::string dataSetName = "terrama2.data_provider";

  std::shared_ptr<te::da::DataSource> dataSource = ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();
  std::string sql("SELECT * FROM " + dataSetName + " WHERE id = " + te::common::Convert2String(id));

  std::auto_ptr<te::da::DataSet> dataSet = transactor->query(sql);

  DataProviderPtr provider;

  if(dataSet->moveNext())
  {
    provider.reset(new DataProvider(dataSet->getAsString("name")));
    provider->setId(dataSet->getInt32("id"));
    provider->setDescription(dataSet->getString("description"));
    provider->setKind(IntToDataProviderKind(dataSet->getInt32("kind")));
    provider->setUri(dataSet->getString("uri"));
    provider->setStatus(BoolToDataProviderStatus(dataSet->getBool("active")));
  }

  return provider;
}

std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataProviderDAO::list() const
{
  std::vector<terrama2::core::DataProviderPtr> vecCollectors;
  std::string dataSetName = "terrama2.data_provider";

  std::shared_ptr<te::da::DataSource> dataSource = ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSet> dataSet = dataSource->getDataSet(dataSetName);


  dataSet->moveBeforeFirst();
  while(dataSet->moveNext())
  {
    DataProviderPtr provider(new DataProvider(dataSet->getAsString("name")));
    provider->setId(dataSet->getInt32("id"));
    provider->setDescription(dataSet->getString("description"));
    provider->setKind(IntToDataProviderKind(dataSet->getInt32("kind")));
    provider->setUri(dataSet->getString("uri"));
    provider->setStatus(BoolToDataProviderStatus(dataSet->getBool("active")));
    vecCollectors.push_back(provider);
  }
  return vecCollectors;
}
