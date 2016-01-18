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
#include "DataSetItemDAO.hpp"
#include "IntersectionDAO.hpp"
#include "../DataSetItem.hpp"
#include "../Exception.hpp"
#include "../Intersection.hpp"
#include "../Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// Qt
#include <QObject>

//Boost
#include <boost/date_time.hpp>
#include <boost/format.hpp>

void
terrama2::core::dao::DataSetDAO::save(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
  if(dataset.id() != 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not save a dataset with an identifier different than 0."));

  if(dataset.provider() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("The dataset must be associated to a data provider in order to be saved."));

  try
  {
    boost::format query("INSERT INTO terrama2.dataset "
                                "(name, description, data_provider_id, kind, active, data_frequency, schedule, schedule_retry, schedule_timeout) "
                                "VALUES('%1%', '%2%', %3%, %4%, %5%, %6%, '%7%', %8%, %9%)");

    query.bind_arg(1, dataset.name());
    query.bind_arg(2, dataset.description());
    query.bind_arg(3, dataset.provider());
    query.bind_arg(4, static_cast<int>(dataset.kind()));
    query.bind_arg(5, ToString(ToBool(dataset.status())));
    query.bind_arg(6, dataset.dataFrequency().getTimeDuration().total_seconds());

    std::string schedule = std::to_string(dataset.schedule().getTimeDuration().hours()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().minutes()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().seconds());

    query.bind_arg(7, schedule);
    query.bind_arg(8, dataset.scheduleRetry().getTimeDuration().total_seconds());
    query.bind_arg(9, dataset.scheduleTimeout().getTimeDuration().total_seconds());

    transactor.execute(query.str());

    dataset.setId(transactor.getLastGeneratedId());

    // Persist the metadata
    saveMetadata(dataset, transactor);


    if(dataset.kind() == DataSet::OCCURENCE_TYPE)
    {
      dao::IntersectionDAO::save(dataset.intersection(), transactor);
    }

    if(shallowSave)
      return;

    for(auto& item: dataset.dataSetItems())
      dao::DataSetItemDAO::save(item, transactor);
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
    throw DataAccessException() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }
}

void
terrama2::core::dao::DataSetDAO::update(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallowSave)
{
  if(dataset.id() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  if(dataset.provider() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("The dataset must be associated to a data provider in order to be updated."));

  try
  {
    boost::format query("UPDATE terrama2.dataset SET name ='%1%',"
                                "description = '%2%',"
                                "data_provider_id = %3%,"
                                "kind = %4%,"
                                "active = %5%,"
                                "data_frequency = %6%,"
                                "schedule = '%7%',"
                                "schedule_retry = %8%,"
                                "schedule_timeout =%9% "
                                "WHERE id = %10%");


    query.bind_arg(1, dataset.name());
    query.bind_arg(2, dataset.description());
    query.bind_arg(3, dataset.provider());
    query.bind_arg(4, static_cast<int>(dataset.kind()));
    query.bind_arg(5, ToString(ToBool(dataset.status())));
    query.bind_arg(6, dataset.dataFrequency().getTimeDuration().total_seconds());

    std::string schedule = std::to_string(dataset.schedule().getTimeDuration().hours()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().minutes()) + ":" +
                           std::to_string(dataset.schedule().getTimeDuration().seconds());

    query.bind_arg(7, schedule);
    query.bind_arg(8, dataset.scheduleRetry().getTimeDuration().total_seconds());
    query.bind_arg(9, dataset.scheduleTimeout().getTimeDuration().total_seconds());
    query.bind_arg(10, dataset.id());

    transactor.execute(query.str());

    if(dataset.kind() == DataSet::OCCURENCE_TYPE)
    {
      IntersectionDAO::update(dataset.intersection(), transactor);
    }

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
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessException() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }

}

void
terrama2::core::dao::DataSetDAO::remove(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  try
  {
    std::string sql = "DELETE FROM terrama2.dataset WHERE id = " + std::to_string(id);
    transactor.execute(sql);
  }
  catch(...)
  {
    throw DataSetInUseException() << ErrorDescription(QObject::tr("Can not remove a dataset that is in use by an analysis."));
  }
}

terrama2::core::DataSet
terrama2::core::dao::DataSetDAO::load(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not load a dataset with identifier: 0."));

  try
  {
    std::string sql("SELECT * FROM terrama2.dataset WHERE id = " + std::to_string(id));

    std::auto_ptr<te::da::DataSet> queryResult = transactor.query(sql);

    if(queryResult->moveNext())
    {
      return getDataSet(queryResult, transactor);
    }

    return DataSet();
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
    throw DataAccessException() << ErrorDescription(QObject::tr("Could not retrieve the data provider list."));
  }

  return DataSet();
}

std::vector<terrama2::core::DataSet>
terrama2::core::dao::DataSetDAO::loadAll(uint64_t providerId, te::da::DataSourceTransactor& transactor)
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
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessException() << ErrorDescription(QObject::tr("Could not retrieve the dataset list."));
  }

  return std::move(datasets);
}


void terrama2::core::dao::DataSetDAO::loadMetadata(DataSet& dataSet, te::da::DataSourceTransactor& transactor)
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

void terrama2::core::dao::DataSetDAO::saveMetadata(DataSet& dataset, te::da::DataSourceTransactor& transactor)
{
  if(dataset.id() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not save the metadata with dataset identifier equals 0."));

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
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error saving the metadata for the dataset: %1"));

    err_msg = err_msg.arg(dataset.id());

    throw DataAccessException() << ErrorDescription(err_msg);
  }
}

terrama2::core::DataSet terrama2::core::dao::DataSetDAO::getDataSet(std::auto_ptr<te::da::DataSet>& queryResult, te::da::DataSourceTransactor& transactor)
{

    std::string name = queryResult->getAsString("name");
    terrama2::core::DataSet::Kind kind = ToDataSetKind(queryResult->getInt32("kind"));

    DataSet dataset(name, kind);
    dataset.setId(queryResult->getInt32("id"));
    dataset.setProvider(queryResult->getInt32("data_provider_id"));
    dataset.setDescription(queryResult->getString("description"));
    dataset.setStatus(ToDataSetStatus(queryResult->getBool("active")));

    u_int64_t dataFrequency = queryResult->getInt32("data_frequency");
    boost::posix_time::time_duration tdDataFrequency = boost::posix_time::seconds(dataFrequency);
    te::dt::TimeDuration teTDDataFrequency(tdDataFrequency);
    dataset.setDataFrequency(teTDDataFrequency);

    if(!queryResult->isNull("schedule"))
    {
      auto aptrDT = queryResult->getDateTime("schedule");
      te::dt::DateTime* scheduleDT = aptrDT.release();
      std::unique_ptr<te::dt::TimeDuration> schedule(dynamic_cast<te::dt::TimeDuration*>(scheduleDT));
      if(schedule != nullptr)
      {
        dataset.setSchedule(*schedule);
      }
    }

    u_int64_t scheduleRetry = queryResult->getInt32("schedule_retry");
    boost::posix_time::time_duration tdScheduleRetry = boost::posix_time::seconds(scheduleRetry);
    te::dt::TimeDuration teTDScheduleRetry(tdScheduleRetry);
    dataset.setScheduleRetry(teTDScheduleRetry);

    u_int64_t scheduleTimeout = queryResult->getInt32("schedule_timeout");
    boost::posix_time::time_duration tdScheduleTimeout = boost::posix_time::seconds(scheduleTimeout);
    te::dt::TimeDuration teTDScheduleTimeout(tdScheduleTimeout);
    dataset.setScheduleTimeout(teTDScheduleTimeout);

    // Sets the metadata
    loadMetadata(dataset, transactor);

    IntersectionDAO::load(dataset, transactor);

    std::vector<DataSetItem> items = DataSetItemDAO::loadAll(dataset.id(), transactor);

    for(auto& item : items)
      dataset.add(item);

    return dataset;

}

void terrama2::core::dao::DataSetDAO::updateMetadata(terrama2::core::DataSet& dataset,
                                                te::da::DataSourceTransactor& transactor)
{
  removeMetadata(dataset.id(), transactor);
  saveMetadata(dataset, transactor);
}


void terrama2::core::dao::DataSetDAO::removeMetadata(uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove the metadata with dataset identifier equals 0."));

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
    throw DataAccessException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error saving the collect rules for the dataset: %1"));

    err_msg = err_msg.arg(id);

    throw DataAccessException() << ErrorDescription(err_msg);
  }
}
