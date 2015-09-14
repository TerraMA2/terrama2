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
#include "DataSetDAO.hpp"
#include "ApplicationController.hpp"
#include "Utils.hpp"
#include "../Exception.hpp"

// STL
#include <vector>
#include <memory>
#include <cstdint>

// terralib
#include <terralib/common/StringUtils.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/memory.h>
#include <terralib/memory/DataSetItem.h>

const std::string dataSetName = "terrama2.data_provider";

terrama2::core::DataProviderDAO::DataProviderDAO()
{
  std::shared_ptr<te::da::DataSource> dataSource = ApplicationController::getInstance().getDataSource();

  if(!dataSource.get())
  {
    // PAULO-TODO: throw exception
  }
  transactor_ = dataSource->getTransactor();

}

terrama2::core::DataProviderDAO::~DataProviderDAO()
{

}


void terrama2::core::DataProviderDAO::save(terrama2::core::DataProviderPtr dataProvider)
{
  if(!transactor_.get())
  {
    // PAULO-TODO: Throw exception to inform that the database connection is not available.
  }

  transactor_->begin();

  // Removes the column id because it's an auto number
  std::auto_ptr<te::da::DataSetType> dataSetType = transactor_->getDataSetType(dataSetName);
  te::dt::Property* idProperty = dataSetType->getProperty(0);
  dataSetType->remove(idProperty);

  // Creates a memory dataset from the DataSetType without column id
  std::shared_ptr<te::mem::DataSet> dataSet(new te::mem::DataSet(dataSetType.get()));
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
  transactor_->add(dataSetName, dataSet.get(), options);

  transactor_->commit();

  // TODO: Remove this after getLastGeneratedId is implemented
  std::string sql("SELECT id FROM " + dataSetName + " WHERE name = '" + dataProvider->name() + "'");

  std::auto_ptr<te::da::DataSet> dataSetId = transactor_->query(sql);

  if(dataSetId->moveNext())
  {
    dataProvider->setId(dataSetId->getInt32("id"));
  }


  // Recovers the generated id and sets it in the provider
  // TODO: Implement getLastGeneratedId in TerraLib
  //dataProvider->setId(transactor_->getLastGeneratedId());

}


void terrama2::core::DataProviderDAO::update(terrama2::core::DataProviderPtr dataProvider)
{

  if(!transactor_.get())
  {
    // PAULO-TODO: Throw exception to inform that the database connection is not available.
  }

  if(dataProvider->id() == 0)
  {
    // PAULO-TODO: Throw exception to inform that the id is invalid.
  }

  transactor_->begin();

  std::string sql = "UPDATE " + dataSetName + " SET"
      + " name='" + dataProvider->name() + "'"
      + ", description='" + dataProvider->description() + "'"
      + ", kind=" + te::common::Convert2String((int)dataProvider->kind())
      + ", uri='" + dataProvider->uri() + "'"
      + ", active=" + terrama2::core::BoolToString(DataProviderStatusToBool(dataProvider->status()))
      + " WHERE id = " + te::common::Convert2String(dataProvider->id());

  transactor_->execute(sql);

  transactor_->commit();
}


void terrama2::core::DataProviderDAO::remove(DataProviderPtr dataProvider)
{  
  std::shared_ptr<te::da::DataSource> dataSource = ApplicationController::getInstance().getDataSource();
  transactor_ = dataSource->getTransactor();
  if(!transactor_.get())
  {
    // PAULO-TODO: Throw exception to inform that the database connection is not available.
  }

  transactor_->begin();

  // Tries to removes all the datasets that belong to this provider

  DataSetDAO datasetDAO;

  auto dataSetList = dataProvider->dataSetList();
  foreach (auto dataSet, dataSetList)
  {
    datasetDAO.remove(dataProvider->id(), transactor_);
  }

  std::string sql = "DELETE FROM " + dataSetName
      + " WHERE id = " + te::common::Convert2String(dataProvider->id());

  transactor_->execute(sql);

  transactor_->commit();
}


terrama2::core::DataProviderPtr terrama2::core::DataProviderDAO::find(const uint64_t id) const
{
  if(!transactor_.get())
  {
    // PAULO-TODO: Throw exception to inform that the database connection is not available.
  }

  std::string sql("SELECT * FROM " + dataSetName + " WHERE id = " + te::common::Convert2String(id));

  std::auto_ptr<te::da::DataSet> dataSet = transactor_->query(sql);

  DataProviderPtr provider;

  if(dataSet->moveNext())
  {

    terrama2::core::DataProvider::Kind kind = IntToDataProviderKind(dataSet->getInt32("kind"));
    std::string name = dataSet->getAsString("name");
    provider.reset(new DataProvider(name, kind));
    provider->setId(dataSet->getInt32("id"));
    provider->setDescription(dataSet->getString("description"));
    provider->setUri(dataSet->getString("uri"));
    provider->setStatus(BoolToDataProviderStatus(dataSet->getBool("active")));
  }

  return provider;
}

std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataProviderDAO::list() const
{

  if(!transactor_.get())
  {
    // PAULO-TODO: Throw exception to inform that the database connection is not available.
  }

  std::vector<terrama2::core::DataProviderPtr> vecProviders;

  std::auto_ptr<te::da::DataSet> dataSet = transactor_->getDataSet(dataSetName);

  while(dataSet->moveNext())
  {
    DataProviderPtr provider;
    terrama2::core::DataProvider::Kind kind = IntToDataProviderKind(dataSet->getInt32("kind"));
    std::string name = dataSet->getAsString("name");
    provider.reset(new DataProvider(name, kind));
    provider->setId(dataSet->getInt32("id"));
    provider->setDescription(dataSet->getString("description"));
    provider->setUri(dataSet->getString("uri"));
    provider->setStatus(BoolToDataProviderStatus(dataSet->getBool("active")));
    vecProviders.push_back(provider);
  }

  return vecProviders;
}
