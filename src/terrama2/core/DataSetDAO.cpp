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
#include "DataManager.hpp"
#include "DataSet.hpp"
#include "DataSetItem.hpp"
#include "DataSetItemDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/memory/DataSet.h>

// Qt
#include <QObject>

//Boost
#include <boost/date_time.hpp>
#include <boost/format.hpp>

void
terrama2::core::DataSetDAO::save(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
  if(dataset.id() != 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not save a dataset with an identifier different than 0."));

  if(dataset.provider() == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset must be associated to a data provider  in order to be saved."));

  try
  {
    boost::format query("INSERT INTO terrama2.dataset "
                                "(name, description, data_provider_id, kind, data_frequency, schedule, schedule_retry, schedule_timeout) "
                                "VALUES('%1%', '%2%', %3%, %4%, %5%, '%6%', %7%, %8%)");

    query.bind_arg(1, dataset.name());
    query.bind_arg(2, dataset.description());
    query.bind_arg(3, dataset.provider()->id());
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
    saveCollectRules(dataset, transactor);

    // Persist the metadata
    saveMetadata(dataset, transactor);

    if(!shallowSave)
    {
      for(const std::unique_ptr<terrama2::core::DataSetItem>& item: dataset.dataSetItems())
      {
        DataSetItemDAO::save(*item, transactor);
      }
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
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }



}

void
terrama2::core::DataSetDAO::update(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
  if(dataset.id() == 0)
  {
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));
  }

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
    query.bind_arg(3, dataset.provider()->id());
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


    // Removes all collect rules and than inserts the new ones.
    std::string sql = "DELETE FROM terrama2.dataset_collect_rule WHERE dataset_id = " + std::to_string(dataset.id());
    transactor.execute(sql);
    saveCollectRules(dataset, transactor);

    // Removes all metadata and than inserts the new ones.
    sql = "DELETE FROM terrama2.dataset_metadata WHERE dataset_id = " + std::to_string(dataset.id());
    transactor.execute(sql);
    saveMetadata(dataset, transactor);

    if(!shallowSave)
    {
      for(const std::unique_ptr<terrama2::core::DataSetItem>& item: dataset.dataSetItems())
      {
        DataSetItemDAO::save(*item, transactor);
      }
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
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }

}

void
terrama2::core::DataSetDAO::remove(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

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

std::unique_ptr<terrama2::core::DataSet>
terrama2::core::DataSetDAO::load(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));


  try
  {
    std::string sql("SELECT * FROM terrama2.dataset WHERE id = " + std::to_string(id));

    std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

    if(tempDataSet->moveNext())
    {
      int64_t id = tempDataSet->getInt32("data_provider_id");
      DataProviderPtr dataProvider = DataManager::getInstance().findDataProvider(id);

      std::string name = tempDataSet->getAsString("name");
      terrama2::core::DataSet::Kind kind = ToDataSetKind(tempDataSet->getInt32("kind"));

      std::unique_ptr<DataSet> dataset(new DataSet(provider.get(), name, kind));
      dataset->setId(tempDataSet->getInt32("id"));
      dataset->setDescription(tempDataSet->getString("description"));
      dataset->setStatus(ToDataSetStatus(tempDataSet->getBool("active")));

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

      u_int64_t scheduleRetry = tempDataSet->getInt32("schedule_retry");
      boost::posix_time::time_duration tdScheduleRetry = boost::posix_time::seconds(scheduleRetry);
      te::dt::TimeDuration teTDScheduleRetry(tdScheduleRetry);
      dataset->setScheduleRetry(teTDScheduleRetry);


      u_int64_t scheduleTimeout = tempDataSet->getInt32("schedule_timeout");
      boost::posix_time::time_duration tdScheduleTimeout = boost::posix_time::seconds(scheduleTimeout);
      te::dt::TimeDuration teTDScheduleTimeout(tdScheduleTimeout);
      dataset->setScheduleTimeout(teTDScheduleTimeout);

      // Sets the collect rules
      loadCollectRules(*dataset, transactor);

      // Sets the metadata
      loadMetadata(*dataset, transactor);

      std::vector<std::unique_ptr<DataSetItem> > items = DataSetItemDAO::load(id, transactor);
      
      dataset->setDataSetItems(std::move(items));

      return dataset;
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
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }

  return std::unique_ptr<DataSet>(nullptr);
}

std::vector<std::unique_ptr<terrama2::core::DataSet> >
terrama2::core::DataSetDAO::loadAll(uint64_t providerId, te::da::DataSourceTransactor& transactor)
{
  try
  {
    std::string query("SELECT * FROM terrama2.dataset WHERE data_provider_id = ");
                query += std::to_string(providerId);
                query += " ORDER BY id ASC"; 

    std::auto_ptr<te::da::DataSet> query_result = transactor.query(query);

    while(query_result->moveNext())
    {

      terrama2::core::DataSet::Kind kind = ToDataSetKind(query_result->getInt32("kind"));

      DataSetPtr dataSet(new DataSet(0, kind, &provider));

      dataSet->setName(query_result->getAsString("name"));
      dataSet->setId(query_result->getInt32("id"));
      dataSet->setDescription(query_result->getString("description"));
      dataSet->setStatus(ToDataSetStatus(query_result->getBool("active")));

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
      loadCollectRules(*dataSet, transactor);

// Sets the metadata
      loadMetadata(*dataSet, transactor);

      provider.add(dataSet);
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
}

void terrama2::core::DataSetDAO::loadCollectRules(DataSet& dataSet, te::da::DataSourceTransactor& transactor)
{
  std::vector<terrama2::core::DataSet::CollectRule> collectRules;

  std::string dataSetName = "terrama2.dataset_collect_rule";

  std::string sql("SELECT id, script FROM " + dataSetName + " WHERE dataset_id = " + std::to_string(dataSet.id()));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    terrama2::core::DataSet::CollectRule collectRule;
    collectRule.id = tempDataSet->getInt32("id");
    collectRule.script = tempDataSet->getString("script");

    collectRules.push_back(collectRule);
  }

  dataSet.setCollectRules(collectRules);

}

void terrama2::core::DataSetDAO::saveCollectRules(DataSet& dataSet, te::da::DataSourceTransactor& transactor)
{



  if(dataset.id() != 0)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("Can not save a dataset with an identifier different than 0."));

  if(dataset.provider() == nullptr)
    throw InvalidParameterError() << ErrorDescription(QObject::tr("The dataset must be associated to a data provider  in order to be saved."));

  try
  {
    std::vector<terrama2::core::DataSet::CollectRule> collectRules = dataSet.collectRules();
    for(auto collectRule : collectRules)
    {
      boost::format query("INSERT INTO terrama2.collect_rules "
                                  "(script, id)"
                                  "VALUES('%1%', %2%)");

      query.bind_arg(1, collectRule.script);
      query.bind_arg(2, dataSet.id());

      transactor.execute(query.str());

      collectRule.id = transactor.getLastGeneratedId();
    }

    dataSet.setCollectRules(collectRules);
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

    err_msg.arg(dataSet.id());

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

void terrama2::core::DataSetDAO::updateCollectRules(DataSet& dataset, te::da::DataSourceTransactor& transactor)
{
  //TODO
}

void terrama2::core::DataSetDAO::removeCollectRules(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  //TODO
}

void terrama2::core::DataSetDAO::loadMetadata(DataSet& dataSet, te::da::DataSourceTransactor& transactor)
{
  std::map<std::string, std::string> metadata;

  std::string dataSetName = "terrama2.dataset_metadata";

  std::string sql("SELECT key, value FROM " + dataSetName + " WHERE dataset_id = " + std::to_string(dataSet.id()));
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  while(tempDataSet->moveNext())
  {
    metadata[tempDataSet->getString("key")] = tempDataSet->getString("value");
  }

  dataSet.setMetadata(metadata);

}

void terrama2::core::DataSetDAO::saveMetadata(DataSet& dataSet, te::da::DataSourceTransactor& transactor)
{
  //TODO
}

