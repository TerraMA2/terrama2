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
#include "DataSet.hpp"
#include "DataProviderDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// STL
#include <vector>
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

  // Then, adds it to the data source
  transactor.add(dataSetName, memDataSet.get(), options);


  // Queries generated id
  std::string sql("SELECT * FROM " + dataSetName + " WHERE name = '" + dataSet->name() + "'");
  std::auto_ptr<te::da::DataSet> tempDataSet = transactor.query(sql);

  // Sets the id in the given provider
  if(tempDataSet->moveNext())
  {
    dataSet->setId(tempDataSet->getInt32("id"));
  }

  transactor.commit();
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
}

void terrama2::core::DataSetDAO::remove(int id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  std::string sql = "DELETE FROM " + dataSetName
      + " WHERE id = " + std::to_string(id);

  transactor.execute(sql);
}


terrama2::core::DataSetPtr terrama2::core::DataSetDAO::find(int id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidDataSetIdError() << ErrorDescription(QObject::tr("Can not update a dataset with identifier: 0."));

  std::string sql("SELECT * FROM " + dataSetName + " WHERE id = " + std::to_string(id));

  std::auto_ptr<te::da::DataSet> dataSet = transactor.query(sql);

  DataSetPtr dataset;

  if(dataSet->moveNext())
  {
    int64_t id = dataSet->getInt32("data_provider_id");
    DataProviderPtr dataProvider = DataProviderDAO::find(id, transactor);

    std::string name = dataSet->getAsString("name");    
    terrama2::core::DataSet::Kind kind = IntToDataSetKind(dataSet->getInt32("kind"));
    dataset.reset(new DataSet(dataProvider, name, kind));
    dataset->setId(dataSet->getInt32("id"));
    dataset->setDescription(dataSet->getString("description"));
    dataset->setStatus(BoolToDataSetStatus(dataSet->getBool("active")));

    u_int64_t dataFrequency = dataSet->getInt32("data_frequency");
    boost::posix_time::time_duration tdDataFrequency = boost::posix_time::seconds(dataFrequency);
    te::dt::TimeDuration teTDDataFrequency(tdDataFrequency);
    dataset->setDataFrequency(teTDDataFrequency);

    u_int64_t schedule = dataSet->getInt32("schedule");
    boost::posix_time::time_duration tdSchedule = boost::posix_time::seconds(schedule);
    te::dt::TimeDuration teTDSchedule(tdSchedule);
    dataset->setSchedule(teTDSchedule);


    u_int64_t scheduleRetry =  dataSet->getInt32("schedule_retry");
    boost::posix_time::time_duration tdScheduleRetry = boost::posix_time::seconds(scheduleRetry);
    te::dt::TimeDuration teTDScheduleRetry(tdScheduleRetry);
    dataset->setScheduleRetry(teTDScheduleRetry);


    u_int64_t scheduleTimeout = dataSet->getInt32("schedule_timeout");
    boost::posix_time::time_duration tdScheduleTimeout = boost::posix_time::seconds(scheduleTimeout);
    te::dt::TimeDuration teTDScheduleTimeout(tdScheduleTimeout);
    dataset->setScheduleTimeout(teTDScheduleTimeout);

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

    vecDataSets.push_back(dataSet);
  }

  return vecDataSets;
}
