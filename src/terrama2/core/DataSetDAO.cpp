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

  \brief Persistence layer for datasets.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/


//TerraMA2
#include "DataSetDAO.hpp"
#include "DataSet.hpp"
#include "DataSetItem.hpp"
#include "DataSetItemDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/datatype/TimeDuration.h>

// Qt
#include <QObject>

//Boost
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>

void
terrama2::core::DataSetDAO::save(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
  if(dataset.id() != 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not save a dataset with an identifier different than 0."));

  if(dataset.dataProvider() == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset must be associated to a data provider  in order to be saved."));
/*
  boost::format query("INSERT INTO terrama2.dataset (kind, active, dataset_id, mask, timezone) VALUES(%1%, %2%, %3%, '%4%', '%5%')");

  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);
  te::dt::Property* idProperty = dataSetType->getProperty(0);
  dataSetType->remove(idProperty);

  // Creates a memory dataset from the DataSetType without column id
  std::shared_ptr<te::mem::DataSet> memDataSet(new te::mem::DataSet(dataSetType.get()));
  te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(memDataSet.get());

  // Sets the values in the item
  dsItem->setString("name", dataSet->name());
  dsItem->setString("description", dataSet->description());
  dsItem->setBool("active", DataSetStatusToBool(dataSet->status()));
  dsItem->setInt32("data_provider_id", dataSet->dataProvider()->id());
  dsItem->setInt32("kind", static_cast<int>(dataSet->kind()));
  dsItem->setInt32("data_frequency", dataSet->dataFrequency().getTimeDuration().total_seconds());
  te::dt::TimeDuration* tdSchedule = new  te::dt::TimeDuration(dataSet->schedule());
  dsItem->setDateTime("schedule", tdSchedule);
  dsItem->setInt32("schedule_retry", dataSet->scheduleRetry().getTimeDuration().total_seconds());
  dsItem->setInt32("schedule_timeout", dataSet->scheduleTimeout().getTimeDuration().total_seconds());

  // Adds it to the dataset
  memDataSet->add(dsItem);
  std::map<std::string, std::string> options;

  // Adds it to the data source
  transactor.add(dataSetName, memDataSet.get(), options);

  dataSet->setId(transactor.getLastGeneratedId());

  // Persist the collect rules and sets the generated id
  saveCollectRules(dataSet, transactor);
  auto collectRules = getCollectRules(dataSet->id(), transactor);
  dataSet->setCollectRules(collectRules);

  // Persist the metadata
  saveMetadata(dataSet, transactor);

  // Persist the collect rules and sets the generated id
  saveDataSetItemList(dataSet->id(), dataSet->dataSetItemList(), transactor);
*/
}

void
terrama2::core::DataSetDAO::update(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
/*  if(!dataSet.get())
  {
    throw InvalidDataSetError() << ErrorDescription(QObject::tr("Can not update an invalid dataset."));
  }

  if(dataSet->id() == 0)
  {
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));
  }

  std::string sql = "UPDATE " + dataSetName + " SET"
      + " name='" + dataSet->name() + "'"
      + ", description='" + dataSet->description() + "'"
      + ", active=" + terrama2::core::BoolToString(DataSetStatusToBool(dataSet->status()))
      + ", data_provider_id=" + std::to_string(dataSet->dataProvider()->id())
      + ", kind=" + std::to_string(static_cast<int>(dataSet->kind()))
      + ", data_frequency=" + std::to_string(dataSet->dataFrequency().getTimeDuration().total_seconds())
      + ", schedule='" + std::to_string(dataSet->schedule().getTimeDuration().hours()) + ":" +
                    std::to_string(dataSet->schedule().getTimeDuration().minutes()) + ":" +
                    std::to_string(dataSet->schedule().getTimeDuration().seconds()) + "'"
      + ", schedule_retry=" + std::to_string(dataSet->scheduleRetry().getTimeDuration().total_seconds())
      + ", schedule_timeout=" + std::to_string(dataSet->scheduleTimeout().getTimeDuration().total_seconds())
      + " WHERE id = " + std::to_string(dataSet->id());

  transactor.execute(sql);

  // Removes all collect rules and than inserts the new ones.
  sql = "DELETE FROM terrama2.dataset_collect_rule WHERE dataset_id = " + std::to_string(dataSet->id());
  transactor.execute(sql);
  saveCollectRules(dataSet, transactor);

  // Removes all metadata and than inserts the new ones.
  sql = "DELETE FROM terrama2.dataset_metadata WHERE dataset_id = " + std::to_string(dataSet->id());
  transactor.execute(sql);
  saveMetadata(dataSet, transactor);


  // Updates dataset item list
  updateDataSetItemList(dataSet, transactor);*/

}

void
terrama2::core::DataSetDAO::remove(uint64_t id, te::da::DataSourceTransactor& transactor)
{
/*  if(id == 0)
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  try
  {
    std::string sql = "DELETE FROM " + dataSetName + " WHERE id = " + std::to_string(id);
    transactor.execute(sql);
  }
  catch(...)
  {
    throw DataSetInUseError() << ErrorDescription(QObject::tr("Can not remove a dataset that is in use by an analysis."));
  }
*/
}

std::unique_ptr<terrama2::core::DataSet>
terrama2::core::DataSetDAO::load(uint64_t id, te::da::DataSourceTransactor& transactor)
{
/*  if(id == 0)
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  std::string sql("SELECT * FROM " + dataSetName + " WHERE id = " + std::to_string(id));

  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  DataSetPtr dataset;

  if(tempDataSet->moveNext())
  {
    int64_t id = tempDataSet->getInt32("data_provider_id");
    DataProviderPtr dataProvider = DataProviderDAO::find(id, transactor);

    std::string name = tempDataSet->getAsString("name");
    terrama2::core::DataSet::Kind kind = IntToDataSetKind(tempDataSet->getInt32("kind"));
    dataset.reset(new DataSet(dataProvider, name, kind));
    dataset->setId(tempDataSet->getInt32("id"));
    dataset->setDescription(tempDataSet->getString("description"));
    dataset->setStatus(BoolToDataSetStatus(tempDataSet->getBool("active")));

    u_int64_t dataFrequency = tempDataSet->getInt32("data_frequency");
    boost::posix_time::time_duration tdDataFrequency = boost::posix_time::seconds(dataFrequency);
    te::dt::TimeDuration teTDDataFrequency(tdDataFrequency);
    dataset->setDataFrequency(teTDDataFrequency);

    te::dt::TimeDuration* schedule = dynamic_cast<te::dt::TimeDuration*>(tempDataSet->getDateTime("schedule").get());
    if(schedule != nullptr)
    {
      dataset->setSchedule(*schedule);
      delete schedule;
    }

    u_int64_t scheduleRetry =  tempDataSet->getInt32("schedule_retry");
    boost::posix_time::time_duration tdScheduleRetry = boost::posix_time::seconds(scheduleRetry);
    te::dt::TimeDuration teTDScheduleRetry(tdScheduleRetry);
    dataset->setScheduleRetry(teTDScheduleRetry);


    u_int64_t scheduleTimeout = tempDataSet->getInt32("schedule_timeout");
    boost::posix_time::time_duration tdScheduleTimeout = boost::posix_time::seconds(scheduleTimeout);
    te::dt::TimeDuration teTDScheduleTimeout(tdScheduleTimeout);
    dataset->setScheduleTimeout(teTDScheduleTimeout);

    // Sets the collect rules
    auto collectRules = getCollectRules(id, transactor);
    dataset->setCollectRules(collectRules);

    // Sets the metadata
    auto metadata = getMetadata(id, transactor);
    dataset->setMetadata(metadata);

    dataset->setDataSetItemList(getDataSetItemList(dataset, transactor));

  }

  return dataset;*/
  return nullptr;
}

void
terrama2::core::DataSetDAO::load(DataProvider& provider, te::da::DataSourceTransactor& transactor)
{
/*  if(provider == nullptr)
    throw InvalidDataProviderError() << ErrorDescription(QObject::tr("Can not retrieve datasets for a NULL data provider."));

  try
  {
    std::string query("SELECT * FROM terrama2.dataset WHERE data_provider_id = ");
                query += std::to_string(provider->id());
                query += " ORDER BY id ASC"; 

    std::auto_ptr<te::da::DataSet> query_result = transactor.query(query);

    while(query_result->moveNext())
    {
      std::string name = query_result->getAsString("name");
      terrama2::core::DataSet::Kind kind = IntToDataSetKind(query_result->getInt32("kind"));

      DataSetPtr dataSet(new DataSet(provider, name, kind));

      dataSet->setId(query_result->getInt32("id"));
      dataSet->setDescription(query_result->getString("description"));
      dataSet->setStatus(BoolToDataSetStatus(query_result->getBool("active")));

      u_int64_t dataFrequency = query_result->getInt32("data_frequency");
      boost::posix_time::time_duration tdDataFrequency = boost::posix_time::seconds(dataFrequency);
      te::dt::TimeDuration teTDDataFrequency(tdDataFrequency);
      dataSet->setDataFrequency(teTDDataFrequency);

      u_int64_t schedule = query_result->getInt32("schedule");
      boost::posix_time::time_duration tdSchedule = boost::posix_time::seconds(schedule);
      te::dt::TimeDuration teTDSchedule(tdSchedule);
      dataSet->setSchedule(teTDSchedule);

      u_int64_t scheduleRetry =  query_result->getInt32("schedule_retry");
      boost::posix_time::time_duration tdScheduleRetry = boost::posix_time::seconds(scheduleRetry);
      te::dt::TimeDuration teTDScheduleRetry(tdScheduleRetry);
      dataSet->setScheduleRetry(teTDScheduleRetry);

      u_int64_t scheduleTimeout = query_result->getInt32("schedule_timeout");
      boost::posix_time::time_duration tdScheduleTimeout = boost::posix_time::seconds(scheduleTimeout);
      te::dt::TimeDuration teTDScheduleTimeout(tdScheduleTimeout);
      dataSet->setScheduleTimeout(teTDScheduleTimeout);

// Sets the collect rules
      auto collectRules = getCollectRules(dataSet->id(), transactor);
      dataSet->setCollectRules(collectRules);

// Sets the metadata
      auto metadata = getMetadata(dataSet->id(), transactor);
      dataSet->setMetadata(metadata);

      provider->add(dataSet);
    }
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the dataset list."));
  }*/
}
/*
std::vector<terrama2::core::DataSet::CollectRule> terrama2::core::DataSetDAO::getCollectRules(uint64_t dataSetId, te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataSet::CollectRule> collectRules;

  std::string dataSetName = "terrama2.dataset_collect_rule";

  std::string sql("SELECT id, script FROM " + dataSetName + " WHERE dataset_id = " + std::to_string(dataSetId));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    terrama2::core::DataSet::CollectRule collectRule;
    collectRule.id = tempDataSet->getInt32("id");
    collectRule.script = tempDataSet->getString("script");

    collectRules.push_back(collectRule);
  }

  return collectRules;
}


void terrama2::core::DataSetDAO::saveCollectRules(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
{
  std::string dataSetName = "terrama2.dataset_collect_rule";
  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);
  te::dt::Property* idProperty = dataSetType->getProperty(0);
  dataSetType->remove(idProperty);

  // Creates a memory dataset from the DataSetType without column id
  std::shared_ptr<te::mem::DataSet> memDataSet(new te::mem::DataSet(dataSetType.get()));
  std::map<std::string, std::string> options;

  foreach(auto rule, dataSet->collectRules())
  {
    te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(memDataSet.get());

    // Sets the values in the item
    dsItem->setString("script", rule.script);
    dsItem->setInt32("dataset_id", dataSet->id());
    memDataSet->add(dsItem);

  }

  // Adds it to the data source
  transactor.add(dataSetName, memDataSet.get(), options);
}


std::map<std::string, std::string> terrama2::core::DataSetDAO::getMetadata(uint64_t dataSetId, te::da::DataSourceTransactor& transactor)
{
  std::map<std::string, std::string> metadata;

  std::string dataSetName = "terrama2.dataset_metadata";

  std::string sql("SELECT key, value FROM " + dataSetName + " WHERE dataset_id = " + std::to_string(dataSetId));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    metadata[tempDataSet->getString("key")] = tempDataSet->getString("value");
  }

  return metadata;
}


void terrama2::core::DataSetDAO::saveMetadata(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
{
  std::string dataSetName = "terrama2.dataset_metadata";
  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);
  te::dt::Property* idProperty = dataSetType->getProperty(0);
  dataSetType->remove(idProperty);

  // Creates a memory dataset from the DataSetType without column id
  std::shared_ptr<te::mem::DataSet> memDataSet(new te::mem::DataSet(dataSetType.get()));
  std::map<std::string, std::string> options;

  std::map<std::string, std::string> metadata = dataSet->metadata();

  for(auto it = metadata.begin(); it != metadata.end(); ++it)
  {
    te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(memDataSet.get());

    // Sets the values in the item
    dsItem->setString("key", it->first);
    dsItem->setString("value", it->second);
    dsItem->setInt32("dataset_id", dataSet->id());
    memDataSet->add(dsItem);
  }

  // Adds it to the data source
  transactor.add(dataSetName, memDataSet.get(), options);
}

std::vector<terrama2::core::DataSetItemPtr> terrama2::core::DataSetDAO::getDataSetItemList(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataSetItemPtr> dataSetItemList;

  std::string dataSetName = "terrama2.dataset_item";

  std::string sql("SELECT * FROM " + dataSetName + " WHERE dataset_id = " + std::to_string(dataSet->id()));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    terrama2::core::DataSetItem::Kind kind = IntToDataSetItemKind(tempDataSet->getInt32("kind"));
    terrama2::core::DataSetItemPtr dataSetItem(new terrama2::core::DataSetItem(dataSet, kind));
    dataSetItem->setId(tempDataSet->getInt32("id"));
    dataSetItem->setStatus(BoolToDataSetItemStatus(tempDataSet->getBool("active")));
    dataSetItem->setMask(tempDataSet->getString("mask"));
    dataSetItem->setTimezone(tempDataSet->getString("timezone"));

    // Retrieve the filter
    dataSetItem->setFilter(getFilter(dataSetItem, transactor));

    dataSetItem->setStorageMetadata(getStorageMetadata(dataSetItem->id(), transactor));

    dataSetItemList.push_back(dataSetItem);
  }

  return dataSetItemList;
}


void terrama2::core::DataSetDAO::saveDataSetItemList(const int64_t dataSetId, const std::vector<DataSetItemPtr>& dataSetItemList, te::da::DataSourceTransactor& transactor)
{
  std::string dataSetName = "terrama2.dataset_item";
  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);
  te::dt::Property* idProperty = dataSetType->getProperty(0);
  dataSetType->remove(idProperty);

  std::map<std::string, std::string> options;


  for (unsigned int i = 0; i < dataSetItemList.size(); ++i)
  {

    // Creates a memory dataset from the DataSetType without column id
    std::shared_ptr<te::mem::DataSet> memDataSet(new te::mem::DataSet(dataSetType.get()));

    DataSetItemPtr dataSetItem = dataSetItemList[i];
    te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(memDataSet.get());

    // Sets the values in the item
    dsItem->setBool("active", DataSetItemStatusToBool(dataSetItem->status()));
    dsItem->setInt32("kind", static_cast<int>(dataSetItem->kind()));
    dsItem->setInt32("dataset_id", dataSetId);
    dsItem->setString("mask", dataSetItem->mask());
    dsItem->setString("timezone", dataSetItem->timezone());
    memDataSet->add(dsItem);


    // Adds it to the data source
    transactor.add(dataSetName, memDataSet.get(), options);

    // Sets the id
    dataSetItem->setId(transactor.getLastGeneratedId());

    // Save the filter
    if(dataSetItem->filter().get())
    {
      saveFilter(dataSetItem->id(), dataSetItem->filter(), transactor);
    }

    saveStorageMetadata(dataSetItem->id(), dataSetItem->storageMetadata(), transactor);

  }

}


void terrama2::core::DataSetDAO::updateDataSetItemList(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
{
  std::string dataSetName = "terrama2.dataset_item";

  // First we need to retrieve all ids from database to identify the new data.
  std::string sql = "SELECT id FROM " + dataSetName + " WHERE dataset_id = " + std::to_string(dataSet->id());
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);
  std::vector<int> ids;
  while(tempDataSet->moveNext())
  {
    ids.push_back(tempDataSet->getInt32("id"));
  }

  std::vector<DataSetItemPtr> dataListToInsert;

  foreach (auto data, dataSet->dataSetItemList())
  {
    // Update the data that already exists in database.
    auto itId = std::find(ids.begin(), ids.end(), data->id());
    if ( itId != ids.end() )
    {
      sql = "UPDATE " + dataSetName + " SET"
          + " active=" + terrama2::core::BoolToString(DataSetItemStatusToBool(data->status()))
          + ", dataset_id=" + std::to_string(dataSet->id())
          + ", kind=" + std::to_string(static_cast<int>(data->kind()))
          + ", mask='" + data->mask() + "'"
          + ", timezone='" + data->timezone() + "'"
          + " WHERE id = " + std::to_string(data->id());

      transactor.execute(sql);

      // Remove from the vector of id, so what is left in the vector should be removed from database.
      ids.erase(itId);
    }
    else
    {
      dataListToInsert.push_back(data);
    }
  }

  // Delete from database the dataset itens that don't exist anymore.
  for (unsigned int i = 0; i < ids.size(); ++i)
  {
    sql = "DELETE FROM " + dataSetName + " WHERE id = " + std::to_string(ids[i]);
    transactor.execute(sql);
  }

  // Inserts the new dataset itens.
  saveDataSetItemList(dataSet->id(), dataListToInsert, transactor);

  // Retrieves all data from database and sets in the dataset.
  auto dataSetItemList = getDataSetItemList(dataSet, transactor);
  dataSet->setDataSetItemList(dataSetItemList);

  foreach(auto dataSetItem, dataSetItemList)
  {
    if(dataSetItem->filter().get())
    {
      updateFilter(dataSetItem->filter(), transactor);
    }

    // Remove all metadata then insert the new one
    std::string sql = "DELETE FROM terrama2.dataset_metadata WHERE dataset_id = " + std::to_string(dataSetItem->id());
    transactor.execute(sql);

    saveStorageMetadata(dataSetItem->id(), dataSetItem->storageMetadata(), transactor);

  }
}

terrama2::core::FilterPtr terrama2::core::DataSetDAO::getFilter(terrama2::core::DataSetItemPtr dataSetItem, te::da::DataSourceTransactor& transactor)
{
  terrama2::core::FilterPtr filter(new terrama2::core::Filter(dataSetItem));

  std::string dataSetName = "terrama2.filter";

  std::string sql("SELECT * FROM " + dataSetName + " WHERE dataset_item_id = " + std::to_string(dataSetItem->id()));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    filter->setDiscardBefore(tempDataSet->getDateTime("discard_before"));
    filter->setDiscardAfter(tempDataSet->getDateTime("discard_after"));

    if(!tempDataSet->isNull(3))
    {
      filter->setGeometry(tempDataSet->getGeometry("geom"));
    }

    filter->setExpressionType(IntToFilterExpressionType(tempDataSet->getInt32("by_value_type")));
    filter->setValue(atof(tempDataSet->getNumeric("by_value").c_str()));
    filter->setBandFilter(tempDataSet->getString("band_filter"));
  }

  return filter;
}

void terrama2::core::DataSetDAO::saveFilter(const uint64_t dataSetItemId, terrama2::core::FilterPtr filter, te::da::DataSourceTransactor& transactor)
{
  std::string dataSetName = "terrama2.filter";
  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);

  // Creates a memory dataset from the DataSetType
  std::shared_ptr<te::mem::DataSet> memDataSet(new te::mem::DataSet(dataSetType.get()));
  std::map<std::string, std::string> options;

  te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(memDataSet.get());

  // Sets the values in the item
  if(filter->discardBefore() != nullptr)
    dsItem->setDateTime("discard_before", static_cast<te::dt::DateTime*>(filter->discardBefore()->clone()));
  
  if(filter->discardAfter() != nullptr)
    dsItem->setDateTime("discard_after", static_cast<te::dt::DateTime*>(filter->discardAfter()->clone()));

  dsItem->setInt32("dataset_item_id", dataSetItemId);
  dsItem->setInt32("expression_type", static_cast<int>(filter->expressionType()));
  dsItem->setNumeric("value", std::to_string(filter->value()));

  if(filter->geometry() != nullptr)
    dsItem->setGeometry("geom", static_cast<te::gm::Geometry*>(filter->geometry()->clone()));

  dsItem->setString("band_filter", filter->bandFilter());
  memDataSet->add(dsItem);


  // Adds it to the data source
  transactor.add(dataSetName, memDataSet.get(), options);
}

void terrama2::core::DataSetDAO::updateFilter(terrama2::core::FilterPtr filter, te::da::DataSourceTransactor& transactor)
{
  std::string dataSetName = "terrama2.filter";

  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);
  std::shared_ptr<te::mem::DataSet> memDataSet(new te::mem::DataSet(dataSetType.get()));

  std::vector< std::set<int> > properties;
  std::set<int> set;
  std::vector<size_t> ids = { 0 };

  te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(memDataSet.get());

  // Sets the values in the item
  dsItem->setInt32("dataset_item_id", filter->dataSetItem()->id());
  set.insert(0);

  if(filter->discardBefore() != nullptr)
  {
    dsItem->setDateTime("discard_before", static_cast<te::dt::DateTime*>(filter->discardBefore()->clone()));
    set.insert(1);
  }

  if(filter->discardAfter() != nullptr)
  {
    dsItem->setDateTime("discard_after", static_cast<te::dt::DateTime*>(filter->discardAfter()->clone()));
    set.insert(2);
  }

  if(filter->geometry() != nullptr)
  {
    dsItem->setGeometry("geom", static_cast<te::gm::Geometry*>(filter->geometry()->clone()));
    set.insert(3);
  }

  // TODO: What is an external data?
  //dsItem->setInt32("extenal_data_id", ...);
  //set.insert(4);

  dsItem->setInt32("expression_type", static_cast<int>(filter->expressionType()));
  set.insert(5);
  dsItem->setNumeric("value", std::to_string(filter->value()));
  set.insert(6);

  // TODO: What is an external data?
  //dsItem->setInt32("within_external_data_id", ...);
  //set.insert(7);

  dsItem->setString("band_filter", filter->bandFilter());
  set.insert(8);

  memDataSet->add(dsItem);

  properties.push_back(set);

  transactor.update(dataSetName, memDataSet.get(), properties, ids);

}

std::map<std::string, std::string> terrama2::core::DataSetDAO::getStorageMetadata(const uint64_t dataSetItemId,
                                                                                  te::da::DataSourceTransactor& transactor)
{
  std::map<std::string, std::string> metadata;

  std::string dataSetName = "terrama2.storage_metadata";

  std::string sql("SELECT key, value FROM " + dataSetName + " WHERE dataset_item_id = " + std::to_string(dataSetItemId));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    metadata[tempDataSet->getString("key")] = tempDataSet->getString("value");
  }

  return metadata;
}

void terrama2::core::DataSetDAO::saveStorageMetadata(const uint64_t dataSetItemId,
                                                     const std::map<std::string, std::string>& storageMetadata,
                                                     te::da::DataSourceTransactor& transactor)
{
  std::string dataSetName = "terrama2.storage_metadata";
  std::auto_ptr<te::da::DataSetType> dataSetType = transactor.getDataSetType(dataSetName);
  te::dt::Property* idProperty = dataSetType->getProperty(0);
  dataSetType->remove(idProperty);

  // Creates a memory dataset from the DataSetType without column id
  std::shared_ptr<te::mem::DataSet> memDataSet(new te::mem::DataSet(dataSetType.get()));
  std::map<std::string, std::string> options;


  for(auto it = storageMetadata.begin(); it != storageMetadata.end(); ++it)
  {
    te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(memDataSet.get());

    // Sets the values in the item
    dsItem->setString("key", it->first);
    dsItem->setString("value", it->second);
    dsItem->setInt32("dataset_item_id", dataSetItemId);
    memDataSet->add(dsItem);
  }

  // Adds it to the data source
  transactor.add(dataSetName, memDataSet.get(), options);
}
*/