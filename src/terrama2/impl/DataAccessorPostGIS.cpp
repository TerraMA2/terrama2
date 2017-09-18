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
  \file terrama2/core/data-access/DataAccessorPostGIS.hpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorPostGIS.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/TimeUtils.hpp"
#include "../core/data-access/SynchronizedDataSet.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

#include <terralib/geometry/MultiPolygon.h>

// QT
#include <QUrl>
#include <QObject>

//boost
#include <boost/algorithm/string/replace.hpp>

std::string terrama2::core::DataAccessorPostGIS::whereConditions(terrama2::core::DataSetPtr dataSet,
                                                                 const std::string datetimeColumnName,
                                                                 const terrama2::core::Filter& filter) const
{
  std::vector<std::string> whereConditions;
  addDateTimeFilter(datetimeColumnName, filter, whereConditions);
  addGeometryFilter(dataSet, filter, whereConditions);

  std::string conditions;
  if(!whereConditions.empty())
  {
    conditions = whereConditions.front();
    for(size_t i = 1; i < whereConditions.size(); ++i)
      conditions += " AND " + whereConditions.at(i);
  }

  std::string lastDatesJoin = addLastDatesFilter(dataSet, datetimeColumnName, filter, conditions);
  addValueFilter(filter, conditions);

  std::string where;
  if(!lastDatesJoin.empty())
    where += lastDatesJoin;

  if(!conditions.empty())
    where += " WHERE "+ conditions;

  return where;
}

void terrama2::core::DataAccessorPostGIS::addValueFilter(const terrama2::core::Filter& filter,
                                                         std::string& conditions) const
{
  std::string condition = filter.byValue;
  if(!filter.byValue.empty())
  {
    if(!conditions.empty())
      conditions +=" AND " + condition;
    else
      conditions = condition;
  }
}

terrama2::core::DataSetSeries terrama2::core::DataAccessorPostGIS::getSeries(const std::string& uri, const terrama2::core::Filter& filter,
    terrama2::core::DataSetPtr dataSet, std::shared_ptr<FileRemover> /*remover*/) const
{
  std::string tableName = getDataSetTableName(dataSet);

  // creates a DataSource to the data and filters the dataset,
  // also joins if the DCP comes from separated files
  std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceType(), uri));

  // RAII for open/closing the datasource
  OpenClose<std::shared_ptr<te::da::DataSource>> openClose(datasource);

  if(!datasource->isOpened())
  {
    QString errMsg = QObject::tr("DataProvider could not be opened.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw NoDataException() << ErrorDescription(errMsg);
  }

  // get a transactor to interact to the data source
  std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

  if (!transactor->dataSetExists(tableName))
  {
    QString errMsg = QObject::tr("Table '%1' doesn't exist.").arg(tableName.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw NoDataException() << ErrorDescription(errMsg);
  }

  // Get the datetime column name in teDataSet for filters
  std::string datetimeColumnName = "";

  try
  {
     datetimeColumnName = getTimestampPropertyName(dataSet, false);
  }
  catch(const UndefinedTagException& /*e*/)
  {
    // do nothing
  }

  if(datetimeColumnName.empty())
  {
    std::unique_ptr< te::da::DataSetType > dataSetType = datasource->getDataSetType(tableName);

    auto property = dataSetType->findFirstPropertyOfType(te::dt::DATETIME_TYPE);

    if(property)
    {
      datetimeColumnName = property->getName();
    }
  }

  std::string query = "SELECT ";
  query+="* ";
  query+= "FROM "+tableName+" AS t";
  query += whereConditions(dataSet, datetimeColumnName, filter);

//  TERRAMA2_LOG_DEBUG() << query;

  std::shared_ptr<te::da::DataSet> tempDataSet = transactor->query(query);

  if(tempDataSet->isEmpty())
  {
    QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_WARNING() << errMsg;
  }

  updateLastTimestamp(dataSet, transactor);

  DataSetSeries series;
  series.dataSet = dataSet;
  series.syncDataSet.reset(new terrama2::core::SynchronizedDataSet(tempDataSet));
  series.teDataSetType = transactor->getDataSetType(tableName);

  return series;
}

std::string terrama2::core::DataAccessorPostGIS::getDataSetTableName(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("table_name");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined table name in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorPostGIS::retrieveData(const DataRetrieverPtr /*dataRetriever*/, DataSetPtr /*dataSet*/, const Filter& /*filter*/, std::shared_ptr<terrama2::core::FileRemover> /*remover*/) const
{
  QString errMsg = QObject::tr("Non retrievable DataProvider.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw NoDataException() << ErrorDescription(errMsg);
}

void terrama2::core::DataAccessorPostGIS::addDateTimeFilter(const std::string datetimeColumnName,
    const terrama2::core::Filter& filter,
    std::vector<std::string>& whereConditions) const
{
  if(!(filter.discardBefore.get() || filter.discardAfter.get()))
    return;

  if(datetimeColumnName.empty())
  {
    QString errMsg = QObject::tr("Undefined column name to filter in PostGIS.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);;
  }

  if(filter.discardBefore.get())
    whereConditions.push_back("t."+datetimeColumnName+" > '"+filter.discardBefore->toString() + "'");

  if(filter.discardAfter.get())
    whereConditions.push_back("t."+datetimeColumnName+" <= '"+filter.discardAfter->toString() + "'");
}

void terrama2::core::DataAccessorPostGIS::addGeometryFilter(terrama2::core::DataSetPtr dataSet,
    const terrama2::core::Filter& filter,
    std::vector<std::string>& whereConditions) const
{
  if(filter.region.get())
  {
    std::unique_ptr<te::gm::Geometry> geom(static_cast<te::gm::Geometry*>(filter.region->clone()));
    geom->transform(4326);
    whereConditions.push_back("ST_INTERSECTS(ST_Transform(t." + getGeometryPropertyName(dataSet)
                              + ", " + std::to_string(geom->getSRID())
                              + "), ST_GeomFromEWKT('SRID=" + std::to_string(geom->getSRID()) + ";" +geom->asText()+"'))");
  }
}

void terrama2::core::DataAccessorPostGIS::updateLastTimestamp(DataSetPtr dataSet, std::shared_ptr<te::da::DataSourceTransactor> transactor) const
{
  std::string tableName = getDataSetTableName(dataSet);
  std::string query = "SELECT MAX(" + getTimestampPropertyName(dataSet) + ") FROM " + tableName;

  std::shared_ptr<te::da::DataSet> tempDataSet = transactor->query(query);
  if(tempDataSet->size() != 1)
  {
    QString errMsg = QObject::tr("Error retrieving last Date/Time.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  tempDataSet->moveFirst();
  std::shared_ptr<te::dt::DateTime> lastDateTime(tempDataSet->getDateTime(0));

  std::shared_ptr< te::dt::TimeInstantTZ > lastDateTimeTz;
  if(lastDateTime->getDateTimeType() == te::dt::TIME_INSTANT)
  {
    //NOTE: Depends on te::dt::TimeInstant toString implementation, it's doc is wrong
    std::string dateString = lastDateTime->toString();

    boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateString));
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(getTimeZone(dataSet)));

    boost::local_time::local_date_time boostLocalTime(boostDate, zone);
    lastDateTimeTz = std::make_shared<te::dt::TimeInstantTZ>(boostLocalTime);
  }
  else if(lastDateTime->getDateTimeType() == te::dt::TIME_INSTANT_TZ)
  {
    lastDateTimeTz = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(lastDateTime);
  }
  else
  {
    //This method expects a valid Date/Time, other formats are not valid.
    QString errMsg = QObject::tr("Unknown date format.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  *lastDateTime_ = *lastDateTimeTz;
}


std::string terrama2::core::DataAccessorPostGIS::addLastDatesFilter(terrama2::core::DataSetPtr dataSet,
                                                                    const std::string datetimeColumnName,
                                                                    const terrama2::core::Filter& filter,
                                                                    std::string whereCondition) const
{
  if(filter.lastValues)
  {
    if(datetimeColumnName.empty())
    {
      QString errMsg = QObject::tr("Undefined column name to filter in PostGIS.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);;
    }

    std::string join = " RIGHT JOIN (SELECT ";
    join += "DISTINCT(t." + datetimeColumnName + ") ";
    join += "FROM " + getDataSetTableName(dataSet)+" t";
    if(!whereCondition.empty())
      join += " WHERE " + whereCondition;

    join += " ORDER BY t." + datetimeColumnName + " DESC limit + " + std::to_string(*filter.lastValues.get()) + ") as last_dates ON ";
    join += "t." + datetimeColumnName + " = last_dates." + datetimeColumnName + " ";

    return join;
  }

  return "";
}
