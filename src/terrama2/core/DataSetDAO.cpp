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
#include "DataSetItem.hpp"
#include "DataSetItemDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// Qt
#include <QObject>

//Boost
#include <boost/date_time.hpp>
#include <boost/format.hpp>

void
terrama2::core::DataSetDAO::save(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
  if(dataset.id() != 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not save a dataset with an identifier different than 0."));

  if(dataset.provider() == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("The dataset must be associated to a data provider in order to be saved."));

  try
  {
    boost::format query("INSERT INTO terrama2.dataset "
                                "(name, description, data_provider_id, kind, data_frequency, schedule, schedule_retry, schedule_timeout) "
                                "VALUES('%1%', '%2%', %3%, %4%, %5%, '%6%', %7%, %8%)");

    query.bind_arg(1, dataset.name());
    query.bind_arg(2, dataset.description());
    query.bind_arg(3, dataset.provider());
    query.bind_arg(4, static_cast<int>(dataset.kind()));
    query.bind_arg(5, dataset.dataFrequency().getTimeDuration().total_seconds());

    std::string schedule = std::to_string(dataset.schedule().getTimeDuration().hours()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().minutes()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().seconds());

    query.bind_arg(6, schedule);
    query.bind_arg(7, dataset.scheduleRetry().getTimeDuration().total_seconds());
    query.bind_arg(8, dataset.scheduleTimeout().getTimeDuration().total_seconds());

    transactor.execute(query.str());

    dataset.setId(transactor.getLastGeneratedId());


    // Persist the collect rules and sets the generated id
    for(auto collectRule : dataset.collectRules())
    {
      saveCollectRule(collectRule, transactor);
    }


    // Persist the metadata
    saveMetadata(dataset, transactor);

    if(shallowSave)
      return;
    
    for(auto& item: dataset.dataSetItems())
      DataSetItemDAO::save(item, transactor);
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
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }
}

void
terrama2::core::DataSetDAO::update(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
  if(dataset.id() == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  if(dataset.provider() == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("The dataset must be associated to a data provider in order to be updated."));

  try
  {
    boost::format query("UPDATE terrama2.dataset SET name ='%1%',"
                                "description = '%2%',"
                                "data_provider_id = %3%,"
                                "kind = %4%,"
                                "data_frequency = %5%,"
                                "schedule = '%6%',"
                                "schedule_retry = %7%,"
                                "schedule_timeout =%8% "
                                "WHERE id = %9%");


    query.bind_arg(1, dataset.name());
    query.bind_arg(2, dataset.description());
    query.bind_arg(3, dataset.provider());
    query.bind_arg(4, static_cast<int>(dataset.kind()));
    query.bind_arg(5, dataset.dataFrequency().getTimeDuration().total_seconds());

    std::string schedule = std::to_string(dataset.schedule().getTimeDuration().hours()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().minutes()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().seconds());

    query.bind_arg(6, schedule);
    query.bind_arg(7, dataset.scheduleRetry().getTimeDuration().total_seconds());
    query.bind_arg(8, dataset.scheduleTimeout().getTimeDuration().total_seconds());
    query.bind_arg(9, dataset.id());

    transactor.execute(query.str());

    updateCollectRules(dataset, transactor);

    updateMetadata(dataset, transactor);


    if(shallowSave)
      return;

    DataSetItemDAO::updateDataSetItems(dataset, transactor);


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
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }

}

void
terrama2::core::DataSetDAO::remove(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  try
  {
    std::string sql = "DELETE FROM terrama2.dataset WHERE id = " + std::to_string(id);
    transactor.execute(sql);
  }
  catch(...)
  {
    throw DataSetInUseError() << ErrorDescription(QObject::tr("Can not remove a dataset that is in use by an analysis."));
  }
}

terrama2::core::DataSet
terrama2::core::DataSetDAO::load(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not load a dataset with identifier: 0."));

  try
  {
    std::string sql("SELECT * FROM terrama2.dataset WHERE id = " + std::to_string(id));

    std::auto_ptr<te::da::DataSet> queryResult = transactor.query(sql);

    return getDataSet(queryResult, transactor);
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
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }

  return DataSet();
}

std::vector<terrama2::core::DataSet>
terrama2::core::DataSetDAO::loadAll(uint64_t providerId, te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataSet> datasets;

  try
  {
    std::string query("SELECT * FROM terrama2.dataset WHERE data_provider_id = ");
                query += std::to_string(providerId);
                query += " ORDER BY id ASC"; 

    std::auto_ptr<te::da::DataSet> queryResult = transactor.query(query);

    while(queryResult->moveNext())
    {
      datasets.push_back(getDataSet(queryResult, transactor));
    }
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the dataset list."));
  }
  
  return std::move(datasets);
}

void terrama2::core::DataSetDAO::loadCollectRules(DataSet& dataSet, te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataSet::CollectRule> collectRules;

  std::string sql("SELECT id, script FROM terrama2.dataset_collect_rule WHERE datasetId = " + std::to_string(dataSet.id()));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    terrama2::core::DataSet::CollectRule collectRule;
    collectRule.id = tempDataSet->getInt32("id");
    collectRule.script = tempDataSet->getString("script");
    collectRule.datasetId = dataSet.id();

    collectRules.push_back(collectRule);
  }

  dataSet.setCollectRules(collectRules);

}

void terrama2::core::DataSetDAO::saveCollectRule(DataSet::CollectRule& collectRule, te::da::DataSourceTransactor& transactor)
{

  if(collectRule.id != 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not save collect rule with a valid identifier."));

  if(collectRule.datasetId == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not save collect rules with dataset identifier equals 0."));

  try
  {
    boost::format query("INSERT INTO terrama2.dataset_collect_rule "
                                "(script, dataset_id)"
                                "VALUES('%1%', %2%)");

    query.bind_arg(1, collectRule.script);
    query.bind_arg(2, collectRule.datasetId);

    transactor.execute(query.str());

    collectRule.id = (uint64_t)transactor.getLastGeneratedId();

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
    QString err_msg(QObject::tr("Unexpected error saving the collect rule for the dataset: %1"));

    err_msg = err_msg.arg(collectRule.datasetId);

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}


void terrama2::core::DataSetDAO::updateCollectRules(terrama2::core::DataSet& dataset,
                                                    te::da::DataSourceTransactor& transactor)
{
  std::string sql = "SELECT id FROM terrama2.dataset_collect_rule WHERE dataset_id = " + std::to_string(dataset.id());

  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  std::vector<int32_t> ids;
  if(tempDataSet->moveNext())
  {
    int32_t itemId = tempDataSet->getInt32(0);
    ids.push_back(itemId);
  }


  for(auto& rule: dataset.collectRules())
  {
    // Id is 0 for new items
    if(rule.id == 0)
    {
      saveCollectRule(rule, transactor);
    }

    // Id exists just need to call update
    auto it = find (ids.begin(), ids.end(), rule.id);
    if (it != ids.end())
    {
      updateCollectRule(rule, transactor);

      // Remove from the list, so what is left in this vector are the items to remove
      ids.erase(it);
    }
  }

  for(auto itemId : ids)
  {
    removeCollectRule(itemId, transactor);
  }
}


void terrama2::core::DataSetDAO::updateCollectRule(DataSet::CollectRule& collectRule, te::da::DataSourceTransactor& transactor)
{
  if(collectRule.id == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not update a collect rule with identifier: 0."));

  if(collectRule.datasetId == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("The collect rule must be associated to a valid dataset in order to be updated."));

  try
  {
    boost::format query("UPDATE terrama2.dataset_collect_rule SET "
                                "script ='%1%',"
                                "WHERE id = %2%");


    query.bind_arg(1, collectRule.script);
    query.bind_arg(2, collectRule.id);

    transactor.execute(query.str());
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the dataset list."));
  }
}

void terrama2::core::DataSetDAO::removeCollectRule(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not remove the collect rules with identifier equals 0."));

  try
  {
    boost::format query("DELETE FROM terrama2.dataset_collect_rule WHERE id = %1%");
    query.bind_arg(1, id);
    transactor.execute(query.str());
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
    QString err_msg(QObject::tr("Unexpected error removing the collect rule with identifier: %1"));

    err_msg = err_msg.arg(id);

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

void terrama2::core::DataSetDAO::loadMetadata(DataSet& dataSet, te::da::DataSourceTransactor& transactor)
{
  std::map<std::string, std::string> metadata;

  std::string sql("SELECT key, value FROM terrama2.dataset_metadata WHERE dataset_id = " + std::to_string(dataSet.id()));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    metadata[tempDataSet->getString("key")] = tempDataSet->getString("value");
  }

  dataSet.setMetadata(metadata);

}

void terrama2::core::DataSetDAO::saveMetadata(DataSet& dataset, te::da::DataSourceTransactor& transactor)
{
  if(dataset.id() == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not save the metadata with dataset identifier equals 0."));

  try
  {
    auto metadata = dataset.metadata();
    for (auto it = metadata.begin(); it!= metadata.end(); ++it)
    {
      boost::format query("INSERT INTO terrama2.dataset_metadata "
                                  "(key, value, dataset_id) "
                                  "VALUES('%1%', '%2%', %3%)");

      query.bind_arg(1, it->first);
      query.bind_arg(2, it->second);
      query.bind_arg(3, dataset.id());

      transactor.execute(query.str());

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
    QString err_msg(QObject::tr("Unexpected error saving the metadata for the dataset: %1"));

    err_msg = err_msg.arg(dataset.id());

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

terrama2::core::DataSet terrama2::core::DataSetDAO::getDataSet(std::auto_ptr<te::da::DataSet>& queryResult, te::da::DataSourceTransactor& transactor)
{
  if(queryResult->moveNext())
  {
    std::string name = queryResult->getAsString("name");
    terrama2::core::DataSet::Kind kind = ToDataSetKind(queryResult->getInt32("kind"));

    DataSet dataset(name, kind);
    dataset.setId(queryResult->getInt32("id"));
    dataset.setDescription(queryResult->getString("description"));
    dataset.setStatus(ToDataSetStatus(queryResult->getBool("active")));

    u_int64_t dataFrequency = queryResult->getInt32("data_frequency");
    boost::posix_time::time_duration tdDataFrequency = boost::posix_time::seconds(dataFrequency);
    te::dt::TimeDuration teTDDataFrequency(tdDataFrequency);
    dataset.setDataFrequency(teTDDataFrequency);

    std::unique_ptr<te::dt::TimeDuration> schedule(dynamic_cast<te::dt::TimeDuration*>(queryResult->getDateTime("schedule").get()));
    if(schedule != nullptr)
    {
      dataset.setSchedule(*schedule);
    }

    u_int64_t scheduleRetry = queryResult->getInt32("schedule_retry");
    boost::posix_time::time_duration tdScheduleRetry = boost::posix_time::seconds(scheduleRetry);
    te::dt::TimeDuration teTDScheduleRetry(tdScheduleRetry);
    dataset.setScheduleRetry(teTDScheduleRetry);

    u_int64_t scheduleTimeout = queryResult->getInt32("schedule_timeout");
    boost::posix_time::time_duration tdScheduleTimeout = boost::posix_time::seconds(scheduleTimeout);
    te::dt::TimeDuration teTDScheduleTimeout(tdScheduleTimeout);
    dataset.setScheduleTimeout(teTDScheduleTimeout);

    // Sets the collect rules
    loadCollectRules(dataset, transactor);

    // Sets the metadata
    loadMetadata(dataset, transactor);

    std::vector<DataSetItem> items = DataSetItemDAO::loadAll(dataset.id(), transactor);

    for(const auto& item : items)
      dataset.add(item);

    return dataset;
  }

  return DataSet();
}

void terrama2::core::DataSetDAO::updateMetadata(terrama2::core::DataSet& dataset,
                                                te::da::DataSourceTransactor& transactor)
{
  removeMetadata(dataset.id(), transactor);
  saveMetadata(dataset, transactor);
}


void terrama2::core::DataSetDAO::removeMetadata(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not remove the metadata with dataset identifier equals 0."));

  try
  {
    boost::format query("DELETE FROM terrama2.dataset_metadata WHERE id = %1%");
    query.bind_arg(1, id);
    transactor.execute(query.str());
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
    QString err_msg(QObject::tr("Unexpected error saving the collect rules for the dataset: %1"));

    err_msg = err_msg.arg(id);

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}
