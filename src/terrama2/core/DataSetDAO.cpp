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

  \brief DataSet DAO...

  \author Paulo R. M. Oliveira
*/


//TerraMA2
#include "DataSetDAO.hpp"
#include "DataProviderDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// STL
#include <vector>
#include <map>
#include <memory>

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/datatype/TimeDuration.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>

// Qt
#include <QObject>

//Boost
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time.hpp>

static const std::string dataSetName = "terrama2.dataset";


void terrama2::core::DataSetDAO::save(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
{

  if(dataSet->id() != 0)
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not add a dataset with identifier different than 0."));

// Removes the column id because it's an auto number
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
  dsItem->setInt32("kind", (int)dataSet->kind());
  dsItem->setInt32("data_frequency", dataSet->dataFrequency().getTimeDuration().total_seconds());
  dsItem->setInt32("schedule", dataSet->schedule().getTimeDuration().total_seconds());
  dsItem->setInt32("schedule_retry", dataSet->scheduleRetry().getTimeDuration().total_seconds());
  dsItem->setInt32("schedule_timeout", dataSet->scheduleTimeout().getTimeDuration().total_seconds());

  // Adds it to the dataset
  memDataSet->add(dsItem);
  std::map<std::string, std::string> options;

  // Adds it to the data source
  transactor.add(dataSetName, memDataSet.get(), options);


  // Queries generated id
  std::string sql("SELECT * FROM " + dataSetName + " WHERE name = '" + dataSet->name() + "'");
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  // Sets the id in the given provider
  if(tempDataSet->moveNext())
  {
    dataSet->setId(tempDataSet->getInt32("id"));
  }


  // Persist the collect rules and sets the generated id
  addCollectRules(dataSet, transactor);
  auto collectRules = getCollectRules(dataSet->id(), transactor);
  dataSet->setCollectRules(collectRules);

  addMetadata(dataSet, transactor);
  auto metadata = getMetadata(dataSet->id(), transactor);
  dataSet->setMetadata(metadata);
}


void terrama2::core::DataSetDAO::update(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
{
  if(dataSet->id() == 0)
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  std::string sql = "UPDATE " + dataSetName + " SET"
      + " name='" + dataSet->name() + "'"
      + ", description='" + dataSet->description() + "'"
      + ", active=" + terrama2::core::BoolToString(DataSetStatusToBool(dataSet->status()))
      + ", data_provider_id=" + std::to_string(dataSet->dataProvider()->id())
      + ", kind=" + std::to_string((int)dataSet->kind())
      + ", data_frequency=" + std::to_string(dataSet->dataFrequency().getTimeDuration().total_seconds())
      + ", schedule=" + std::to_string(dataSet->schedule().getTimeDuration().total_seconds())
      + ", schedule_retry=" + std::to_string(dataSet->scheduleRetry().getTimeDuration().total_seconds())
      + ", schedule_timeout=" + std::to_string(dataSet->scheduleTimeout().getTimeDuration().total_seconds())
      + " WHERE id = " + std::to_string(dataSet->id());

  transactor.execute(sql);

  // Removes all collect rules and than inserts the new ones.
  sql = "DELETE FROM terrama2.dataset_collect_rule WHERE dataset_id = " + std::to_string(dataSet->id());
  transactor.execute(sql);
  addCollectRules(dataSet, transactor);

  // Removes all metadata and than inserts the new ones.
  sql = "DELETE FROM terrama2.dataset_metadata WHERE dataset_id = " + std::to_string(dataSet->id());
  transactor.execute(sql);
  addMetadata(dataSet, transactor);

}

void terrama2::core::DataSetDAO::remove(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
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
}


terrama2::core::DataSetPtr terrama2::core::DataSetDAO::find(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
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

    u_int64_t schedule = tempDataSet->getInt32("schedule");
    boost::posix_time::time_duration tdSchedule = boost::posix_time::seconds(schedule);
    te::dt::TimeDuration teTDSchedule(tdSchedule);
    dataset->setSchedule(teTDSchedule);


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

  }

  return dataset;
}

std::vector<terrama2::core::DataSetPtr> terrama2::core::DataSetDAO::list(te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataSetPtr> vecDataSets;

  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.getDataSet(dataSetName);

  while(tempDataSet->moveNext())
  {

    int64_t id = tempDataSet->getInt32("data_provider_id");
    DataProviderPtr dataProvider = DataProviderDAO::find(id, transactor);

    std::string name = tempDataSet->getAsString("name");
    terrama2::core::DataSet::Kind kind = IntToDataSetKind(tempDataSet->getInt32("kind"));
    DataSetPtr dataSet(new DataSet(dataProvider, name, kind));
    dataSet->setId(tempDataSet->getInt32("id"));
    dataSet->setDescription(tempDataSet->getString("description"));
    dataSet->setStatus(BoolToDataSetStatus(tempDataSet->getBool("active")));

    u_int64_t dataFrequency = tempDataSet->getInt32("data_frequency");
    boost::posix_time::time_duration tdDataFrequency = boost::posix_time::seconds(dataFrequency);
    te::dt::TimeDuration teTDDataFrequency(tdDataFrequency);
    dataSet->setDataFrequency(teTDDataFrequency);

    u_int64_t schedule = tempDataSet->getInt32("schedule");
    boost::posix_time::time_duration tdSchedule = boost::posix_time::seconds(schedule);
    te::dt::TimeDuration teTDSchedule(tdSchedule);
    dataSet->setSchedule(teTDSchedule);


    u_int64_t scheduleRetry =  tempDataSet->getInt32("schedule_retry");
    boost::posix_time::time_duration tdScheduleRetry = boost::posix_time::seconds(scheduleRetry);
    te::dt::TimeDuration teTDScheduleRetry(tdScheduleRetry);
    dataSet->setScheduleRetry(teTDScheduleRetry);


    u_int64_t scheduleTimeout = tempDataSet->getInt32("schedule_timeout");
    boost::posix_time::time_duration tdScheduleTimeout = boost::posix_time::seconds(scheduleTimeout);
    te::dt::TimeDuration teTDScheduleTimeout(tdScheduleTimeout);
    dataSet->setScheduleTimeout(teTDScheduleTimeout);

    // Sets the collect rules
    auto collectRules = getCollectRules(dataSet->id(), transactor);
    dataSet->setCollectRules(collectRules);

    // Sets the metadata
    auto metadata = getMetadata(dataSet->id(), transactor);
    dataSet->setMetadata(metadata);

    vecDataSets.push_back(dataSet);
  }

  return vecDataSets;
}

std::vector<terrama2::core::DataSet::CollectRule> terrama2::core::DataSetDAO::getCollectRules(uint64_t dataSetId, te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataSet::CollectRule> collectRules;

  std::string dataSetName = "terrama2.dataset_collect_rule";

  std::string sql("SELECT id, script FROM " + dataSetName + " WHERE dataset_id = " + std::to_string(dataSetId));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    terrama2::core::DataSet::CollectRule collectRule;
    collectRule.id_ = tempDataSet->getInt32("id");
    collectRule.script_ = tempDataSet->getString("script");

    collectRules.push_back(collectRule);
  }

  return collectRules;
}


void terrama2::core::DataSetDAO::addCollectRules(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
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
    dsItem->setString("script", rule.script_);
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

void terrama2::core::DataSetDAO::addMetadata(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor)
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

