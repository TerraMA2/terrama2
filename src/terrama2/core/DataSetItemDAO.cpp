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
  \file terrama2/core/DataSetDAO.hpp

  \brief Persistense layer for dataset items.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/


//TerraMA2
#include "DataSetItemDAO.hpp"
#include "DataSet.hpp"
#include "DataSetItem.hpp"
#include "DataProviderDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// Qt
#include <QObject>

//Boost
#include <boost/format.hpp>

void
terrama2::core::DataSetItemDAO::save(DataSetItem& item, te::da::DataSourceTransactor& transactor)
{
  if(item.id() != 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not save a dataset item with an identifier different than 0."));

  if(item.dataSet() == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset item must be associated to a dataset in order to be saved."));

  boost::format query("INSERT INTO terrama2.dataset_item (kind, active, dataset_id, mask, timezone) VALUES(%1%, %2%, %3%, '%4%', '%5%')");

  query.bind_arg(1, static_cast<uint32_t>(item.kind()));
  query.bind_arg(2, BoolToString(DataSetItemStatusToBool(item.status())));
  query.bind_arg(3, item.dataSet()->id());
  query.bind_arg(4, item.mask());
  query.bind_arg(5, item.timezone());

  try
  {
    transactor.execute(query.str());

    //item.setId(transactor.getLastGeneratedId());

// save the filter
    //if(item.filter() != nullptr)
    //  saveFilter(item.id(), *item.filter(), transactor);

    saveStorageMetadata(item.id(), item.storageMetadata(), transactor);
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not save dataset item."));
  }
}

void
terrama2::core::DataSetItemDAO::update(DataSetItem& item, te::da::DataSourceTransactor& transactor)
{
  if(item.id() == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a dataset item with an identifier: 0."));

  if(item.dataSet() == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset item must be associated to a dataset in order to be updated."));

  boost::format query("UPDATE terrama2.dataset_item SET active = %1%, "
                      "dataset_id = %2%, , kind = %3%, mask = %4%, timezone = %5% WHERE id = %6%");

  query.bind_arg(1, BoolToString(DataSetItemStatusToBool(item.status())));
  query.bind_arg(2, item.dataSet()->id());
  query.bind_arg(3, static_cast<uint32_t>(item.kind()));
  query.bind_arg(4, item.mask());
  query.bind_arg(5, item.timezone());
  query.bind_arg(6, item.id());
  
  try
  {
    transactor.execute(query.str());

    //if(item.filter() != nullptr)
    //  update(*item.filter(), transactor);

    //updateStorageMetadata(item.id(), item.storageMetadata(), transactor);
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not update dataset item."));
  }
}

void
terrama2::core::DataSetItemDAO::remove(uint64_t itemId, te::da::DataSourceTransactor& transactor)
{
  //if(itemId == 0)
  //  throw terrama2::InvalidDataSetItemError() << ErrorDescription(QObject::tr("Can not remove a dataset item with identifier: 0."));

  try
  {
    std::string sql("DELETE FROM terrama2.dataset_item WHERE id = ");
                sql += std::to_string(itemId);

    transactor.execute(sql);
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not update dataset item."));
  }
}

void
terrama2::core::DataSetItemDAO::loadItems(DataSet& dataset, te::da::DataSourceTransactor& transactor)
{
  if(dataset.id() == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not load dataset items for a dataset with an invalid identifier: 0."));

  std::string sql ("SELECT * FROM terrama2.dataset_item WHERE dataset_id = ");
              sql += std::to_string(dataset.id());

  try
  {
    std::auto_ptr<te::da::DataSet> items_result = transactor.query(sql);

    std::vector<DataSetItemPtr> items;

    while(items_result->moveNext())
    {
      DataSetItem::Kind kind = IntToDataSetItemKind(items_result->getInt32("kind"));

      /*DataSetItemPtr item(new DataSetItem(dataset, kind));

      item->setId(tempDataSet->getInt32("id"));
      item->setStatus(BoolToDataSetItemStatus(items_result->getBool("active")));
      item->setMask(items_result->getString("mask"));
      item->setTimezone(items_result->getString("timezone"));*/

// retrieve the filter
      //loadFilter(*item, transactor);
      //loadStorageMetadata(*item, transactor);

      //items.push_back(dataSetItem);
    }

    //dataset.set(items);
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not load dataset items."));
  }
}

void
terrama2::core::DataSetItemDAO::saveStorageMetadata(uint64_t datasetItemId,
                                                    const std::map<std::string, std::string>& metadata,
                                                    te::da::DataSourceTransactor& transactor)
{
}

void
terrama2::core::DataSetItemDAO::updateStorageMetadata(uint64_t datasetItemId,
                                                      std::map<std::string, std::string>& metadata,
                                                      te::da::DataSourceTransactor& transactor)
{
}

void
terrama2::core::DataSetItemDAO::removeStorageMetadata(uint64_t datasetItemId,
                                                      te::da::DataSourceTransactor& transactor)
{
}

void
terrama2::core::DataSetItemDAO::loadStorageMetadata(DataSetItem& item,
                                                    te::da::DataSourceTransactor& transactor)
{
}

void
terrama2::core::DataSetItemDAO::loadFilter(DataSetItem& item,
                                           te::da::DataSourceTransactor& transactor)
{
}

