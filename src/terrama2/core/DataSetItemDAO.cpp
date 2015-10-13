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
#include "DataSetItem.hpp"
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
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not save a dataset item with an identifier different than 0."));

  if(item.dataset() == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset item must be associated to a dataset in order to be saved."));

  if(item.dataset()->id() == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset item must be associated to a valid dataset in order to be saved."));

  boost::format query("INSERT INTO terrama2.dataset_item (kind, active, dataset_id, mask, timezone) VALUES(%1%, %2%, %3%, '%4%', '%5%')");

  query.bind_arg(1, static_cast<uint32_t>(item.kind()));
  query.bind_arg(2, ToString(ToBool(item.status())));
  query.bind_arg(3, item.dataset()->id());
  query.bind_arg(4, item.mask());
  query.bind_arg(5, item.timezone());

  try
  {
    transactor.execute(query.str());

    item.setId(transactor.getLastGeneratedId());

// save the filter
    if(item.filter() != nullptr)
      FilterDAO::save(*item.filter(), transactor);

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
    QString err_msg(QObject::tr("Unexpected error saving dataset item: %1"));

    err_msg.arg(item.id());

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

void
terrama2::core::DataSetItemDAO::update(DataSetItem& item, te::da::DataSourceTransactor& transactor)
{
  if(item.id() == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a dataset item with an identifier: 0."));

  if(item.dataset() == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset item must be associated to a dataset in order to be updated."));

  if(item.dataset().id() == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset item must be associated to a valid dataset in order to be updated."));

  boost::format query("UPDATE terrama2.dataset_item SET active = %1%, "
                      "dataset_id = %2%, , kind = %3%, mask = '%4%', timezone = '%5%' WHERE id = %6%");

  query.bind_arg(1, BoolToString(DataSetItemStatusToBool(item.status())));
  query.bind_arg(2, item.dataset()->id());
  query.bind_arg(3, static_cast<uint32_t>(item.kind()));
  query.bind_arg(4, item.mask());
  query.bind_arg(5, item.timezone());
  query.bind_arg(6, item.id());

  try
  {
    transactor.execute(query.str());

// update the filter if one exists and remove it to assure it was not changed.
    if(item.filter() != nullptr)
      FilterDAO::update(*item.filter(), transactor);
    else
      FilterDAO::remove(item.id(), transactor);

    updateStorageMetadata(item.id(), item.storageMetadata(), transactor);
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
    QString err_msg(QObject::tr("Unexpected error updating dataset item: %1"));

    err_msg.arg(item.id());

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

void
terrama2::core::DataSetItemDAO::remove(uint64_t itemId, te::da::DataSourceTransactor& transactor)
{
  if(itemId == 0)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not remove a dataset item with identifier: 0."));

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
    QString err_msg(QObject::tr("Unexpected error removing dataset item: %1"));

    err_msg.arg(item.id());

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

std::vector<std::unique_ptr<terrama2::core::DataSetItemDAO::DataSetItem> >
terrama2::core::DataSetItemDAO::loadAll(uin64_t datasetId, te::da::DataSourceTransactor& transactor)
{
  if(datasetId == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not load dataset items for a dataset with an invalid identifier: 0."));

  std::string sql ("SELECT * FROM terrama2.dataset_item WHERE dataset_id = ");
              sql += std::to_string(datasetId);

  try
  {
    std::auto_ptr<te::da::DataSet> items_result = transactor.query(sql);

    std::vector<std::unique_ptr<DataSetItem> > items;

    while(items_result->moveNext())
    {
      DataSetItem::Kind kind = ToDataSetItemKind(items_result->getInt32("kind"));
      uint64_t id = items_result->getInt32("id")

      std::unique_ptr<DataSetItem> item(new DataSetItem(id, kind, nullptr));

      item->setStatus(ToDataSetItemStatus(items_result->getBool("active")));
      item->setMask(items_result->getString("mask"));
      item->setTimezone(items_result->getString("timezone"));

// retrieve the filter
      std::unique_ptr<Filter> f = FilterDAO::load(id, transactor);
      item->setFilter(f);

      loadStorageMetadata(*item, transactor);

      items.push_back(std::move(item));
    }
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
    QString err_msg(QObject::tr("Unexpected error loading dataset items for dataset: %1"));

    err_msg.arg(datasetId);

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

void
terrama2::core::DataSetItemDAO::saveStorageMetadata(uint64_t datasetItemId,
                                                    const std::map<std::string, std::string>& metadata,
                                                    te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not save metadata for a dataset item with identifier: 0."));
  
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
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not load dataset items."));
  }
}

void
terrama2::core::DataSetItemDAO::updateStorageMetadata(uint64_t datasetItemId,
                                                      std::map<std::string, std::string>& metadata,
                                                      te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not update metadata for a dataset item with identifier: 0."));
  
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
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not remove metadata for a dataset item with identifier: 0."));
  
  std::string sql("DELETE FROM terrama2.storage_metadata WHERE dataset_item_id = ");
              sql += std::to_string(datasetItemId);
  
  try
  {
    transactor.execute(sql);
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
terrama2::core::DataSetItemDAO::loadStorageMetadata(DataSetItem& item,
                                                    te::da::DataSourceTransactor& transactor)
{
  if(item.id() == 0)
    throw terrama2::InvalidParameterError() << ErrorDescription(QObject::tr("Can not load metadata information for a dataset item with an invalid identifier."));
  
 
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
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not load dataset items."));
  }
}
