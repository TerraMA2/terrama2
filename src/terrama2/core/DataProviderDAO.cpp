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

// TerraMA2
#include "DataProviderDAO.hpp"
#include "DataProvider.hpp"
#include "DataSetDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>

// Qt
#include <QObject>

static const std::string dataSetName = "terrama2.data_provider";

void terrama2::core::DataProviderDAO::save(terrama2::core::DataProviderPtr dataProvider, te::da::DataSourceTransactor& transactor)
{

  if(dataProvider->id() != 0)
    throw InvalidDataProviderIdError() << ErrorDescription(QObject::tr("Can not save a data provider with identifier different than 0."));

// Removes the column id because it's an auto number
  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);
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
  transactor.add(dataSetName, dataSet.get(), options);

  dataProvider->setId(transactor.getLastGeneratedId());

// save all datasets in this provider, it must be zero.
  foreach (auto ds, dataProvider->dataSets())
  {
    if(ds->id() != 0)
      throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not save a dataset with identifier different than 0."));

    DataSetDAO::save(ds, transactor);
  }

}


void terrama2::core::DataProviderDAO::update(terrama2::core::DataProviderPtr dataProvider, te::da::DataSourceTransactor& transactor)
{
  if(dataProvider->id() == 0)
    throw InvalidDataProviderIdError() << ErrorDescription(QObject::tr("Can not update a data provider with identifier: 0."));

  try
  {
    std::string sql = "UPDATE " + dataSetName + " SET"
        + " name='" + dataProvider->name() + "'"
        + ", description='" + dataProvider->description() + "'"
        + ", kind=" + std::to_string(static_cast<int>(dataProvider->kind()))
        + ", uri='" + dataProvider->uri() + "'"
        + ", active=" + terrama2::core::BoolToString(DataProviderStatusToBool(dataProvider->status()))
    + " WHERE id = " + std::to_string(dataProvider->id());

    transactor.execute(sql);
  }
  catch(...)
  {
    throw DataSetInUseError() << ErrorDescription(QObject::tr("Can not remove a data provider with datasets that are in use by analysis."));
  }
}


void terrama2::core::DataProviderDAO::remove(DataProviderPtr dataProvider, te::da::DataSourceTransactor& transactor)
{
  if(dataProvider->id() == 0)
    throw InvalidDataProviderIdError() << ErrorDescription(QObject::tr("Can not remove a data provider with identifier: 0."));

  std::string sql = "DELETE FROM " + dataSetName
                  + " WHERE id = " + std::to_string(dataProvider->id());

  transactor.execute(sql);
}


terrama2::core::DataProviderPtr terrama2::core::DataProviderDAO::find(const uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidDataProviderIdError() << ErrorDescription(QObject::tr("Invalid identifier: 0."));

  std::string sql("SELECT * FROM " + dataSetName + " WHERE id = " + std::to_string(id));

  std::auto_ptr<te::da::DataSet> dataSet = transactor.query(sql);

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

std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataProviderDAO::list(te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataProviderPtr> vecProviders;

  std::auto_ptr<te::da::DataSet> dataSet = transactor.getDataSet(dataSetName);

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
