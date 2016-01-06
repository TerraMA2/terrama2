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
#include "Exception.hpp"
#include "Filter.hpp"
#include "FilterDAO.hpp"
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
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not save a dataset item with an identifier different than 0."));

  if(item.dataset() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("The dataset item must be associated to a dataset in order to be saved."));

  boost::format query("INSERT INTO terrama2.dataset_item (kind, active, dataset_id, mask, timezone, path, srid) VALUES(%1%, %2%, %3%, '%4%', '%5%', '%6%', %7%)");

  query.bind_arg(1, static_cast<uint32_t>(item.kind()));
  query.bind_arg(2, ToString(ToBool(item.status())));
  query.bind_arg(3, item.dataset());
  query.bind_arg(4, item.mask());
  query.bind_arg(5, item.timezone());
  query.bind_arg(6, item.path());
  if(item.srid() == 0)
    query.bind_arg(7, "null");
  else
    query.bind_arg(7, item.srid());

  try
  {
    transactor.execute(query.str());

    item.setId(transactor.getLastGeneratedId());

    FilterDAO::save(item.filter(), transactor);

    saveStorageMetadata(item.id(), item.storageMetadata(), transactor);
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error saving dataset item: %1"));

    err_msg = err_msg.arg(item.id());

    throw DataAccessException() << ErrorDescription(err_msg);
  }
}


void terrama2::core::DataSetItemDAO::updateDataSetItems(DataSet& dataset, te::da::DataSourceTransactor& transactor)
{
  std::string sql = "SELECT id FROM terrama2.dataset_item WHERE dataset_id = " + std::to_string(dataset.id());

  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  std::vector<int32_t> ids;
  if(tempDataSet->moveNext())
  {
    int32_t itemId = tempDataSet->getInt32(0);
    ids.push_back(itemId);
  }


  for(auto item: dataset.dataSetItems())
  {
    // Id is 0 for new items
    if(item.id() == 0)
    {
      save(item, transactor);
    }

    // Id exists just need to call update
    auto it = find (ids.begin(), ids.end(), item.id());
    if (it != ids.end())
    {
      update(item, transactor);

      // Remove from the list, so what is left in this vector are the items to remove
      ids.erase(it);
    }
  }

  for(auto itemId : ids)
  {
    remove(itemId, transactor);
  }
}

void
terrama2::core::DataSetItemDAO::update(DataSetItem& item, te::da::DataSourceTransactor& transactor)
{
  if(item.id() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not update a dataset item with an identifier: 0."));

  if(item.dataset() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("The dataset item must be associated to a dataset in order to be updated."));

  boost::format query("UPDATE terrama2.dataset_item SET active = %1%, "
                      "dataset_id = %2%, kind = %3%, mask = '%4%', timezone = '%5%', path = '%6%', srid = %7% WHERE id = %8%");

  query.bind_arg(1, ToString(ToBool(item.status())));
  query.bind_arg(2, item.dataset());
  query.bind_arg(3, static_cast<uint32_t>(item.kind()));
  query.bind_arg(4, item.mask());
  query.bind_arg(5, item.timezone());
  query.bind_arg(6, item.path());
  if(item.srid() == 0)
    query.bind_arg(7, "null");
  else
    query.bind_arg(7, item.srid());
  query.bind_arg(8, item.id());

  try
  {
    transactor.execute(query.str());

// update the filter if one exists and remove it to assure it was not changed.
    FilterDAO::update(item.filter(), transactor);

    updateStorageMetadata(item.id(), item.storageMetadata(), transactor);
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error updating dataset item: %1"));

    err_msg = err_msg.arg(item.id());

    throw DataAccessException() << ErrorDescription(err_msg);
  }
}

void
terrama2::core::DataSetItemDAO::remove(uint64_t itemId, te::da::DataSourceTransactor& transactor)
{
  if(itemId == 0)
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove a dataset item with identifier: 0."));

  try
  {
    std::string sql("DELETE FROM terrama2.dataset_item WHERE id = ");
                sql += std::to_string(itemId);

    transactor.execute(sql);
  }
  catch(const std::exception& e)
  {
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error removing dataset item: %1"));

    err_msg = err_msg.arg(itemId);

    throw DataAccessException() << ErrorDescription(err_msg);
  }
}

std::vector<terrama2::core::DataSetItem>
terrama2::core::DataSetItemDAO::loadAll(uint64_t datasetId, te::da::DataSourceTransactor& transactor)
{
  if(datasetId == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not load dataset items for a dataset with an invalid identifier: 0."));

  std::string sql ("SELECT * FROM terrama2.dataset_item WHERE dataset_id = ");
              sql += std::to_string(datasetId);

  try
  {
    std::auto_ptr<te::da::DataSet> itemsResult = transactor.query(sql);

    std::vector<DataSetItem> items;

    while(itemsResult->moveNext())
    {
      DataSetItem::Kind kind = ToDataSetItemKind(itemsResult->getInt32("kind"));
      uint64_t id = itemsResult->getInt32("id");

      DataSetItem item(kind, id, datasetId);

      item.setStatus(ToDataSetItemStatus(itemsResult->getBool("active")));
      item.setMask(itemsResult->getString("mask"));
      item.setTimezone(itemsResult->getString("timezone"));
      item.setPath(itemsResult->getString("path"));

      if(itemsResult->isNull("srid"))
        item.setSrid(0);
      else
        item.setSrid(itemsResult->getInt32("srid"));

// retrieve the filter
      Filter f = FilterDAO::load(item, transactor);
      item.setFilter(f);

      loadStorageMetadata(item, transactor);

      items.push_back(item);
    }

    return std::move(items);
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error loading dataset items for dataset: %1"));

    err_msg = err_msg.arg(datasetId);

    throw DataAccessException() << ErrorDescription(err_msg);
  }
}

void
terrama2::core::DataSetItemDAO::saveStorageMetadata(uint64_t datasetItemId,
                                                    const std::map<std::string, std::string>& metadata,
                                                    te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Can not save metadata for a dataset item with identifier: 0."));

  try
  {
    for(auto m : metadata)
    {
      boost::format query("INSERT INTO terrama2.storage_metadata (key, value, dataset_item_id) VALUES('%1%', '%2%', %3%)");

      query.bind_arg(1, m.first);
      query.bind_arg(2, m.second);
      query.bind_arg(3, datasetItemId);

      transactor.execute(query.str());
    }
  }
  catch(const std::exception& e)
  {
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessException() << ErrorDescription(QObject::tr("Could not load dataset items."));
  }
}

void
terrama2::core::DataSetItemDAO::updateStorageMetadata(uint64_t datasetItemId,
                                                      std::map<std::string, std::string>& metadata,
                                                      te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Can not update metadata for a dataset item with identifier: 0."));

// remove all metadata in order to insert the new one
  removeStorageMetadata(datasetItemId, transactor);

// save all metadata to the database
  saveStorageMetadata(datasetItemId, metadata, transactor);
}

void
terrama2::core::DataSetItemDAO::removeStorageMetadata(uint64_t datasetItemId,
                                                      te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove metadata for a dataset item with identifier: 0."));

  std::string sql("DELETE FROM terrama2.storage_metadata WHERE dataset_item_id = ");
              sql += std::to_string(datasetItemId);

  try
  {
    transactor.execute(sql);
  }
  catch(const std::exception& e)
  {
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessException() << ErrorDescription(QObject::tr("Could not load dataset items."));
  }
}

void
terrama2::core::DataSetItemDAO::loadStorageMetadata(DataSetItem& item,
                                                    te::da::DataSourceTransactor& transactor)
{
  if(item.id() == 0)
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Can not load metadata information for a dataset item with an invalid identifier."));


  std::string sql("SELECT key, value FROM terrama2.storage_metadata WHERE dataset_item_id = ");
              sql += std::to_string(item.id());

  try
  {
    std::auto_ptr<te::da::DataSet> metadata_result = transactor.query(sql);

    std::map<std::string, std::string> metadata;

    while(metadata_result->moveNext())
      metadata[metadata_result->getString("key")] = metadata_result->getString("value");


    item.setStorageMetadata(metadata);
  }
  catch(const std::exception& e)
  {
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessException() << ErrorDescription(QObject::tr("Could not load dataset items."));
  }
}
