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

#include <terralib/dataaccess/query/LiteralDateTime.h>
#include <terralib/dataaccess/query/ST_Intersects.h>
#include <terralib/dataaccess/query/PropertyName.h>
#include <terralib/dataaccess/query/DataSetName.h>
#include <terralib/dataaccess/query/GreaterThan.h>
#include <terralib/dataaccess/query/LiteralGeom.h>
#include <terralib/dataaccess/query/LessThan.h>
#include <terralib/dataaccess/query/Fields.h>
#include <terralib/dataaccess/query/Select.h>
#include <terralib/dataaccess/query/SelectExpression.h>
#include <terralib/dataaccess/query/Field.h>
#include <terralib/dataaccess/query/Where.h>
#include <terralib/dataaccess/query/From.h>
#include <terralib/dataaccess/query/And.h>
#include <terralib/dataaccess/query/Max.h>
#include <terralib/dataaccess/query/EqualTo.h>

#include <terralib/geometry/MultiPolygon.h>

// QT
#include <QUrl>
#include <QObject>

std::string terrama2::core::DataAccessorPostGIS::whereConditions(terrama2::core::DataSetPtr dataSet, const terrama2::core::Filter& filter) const
{
  std::string where = " WHERE ";

  std::vector<std::string> whereConditions;
  addDateTimeFilter(dataSet, filter, whereConditions);
  addGeometryFilter(dataSet, filter, whereConditions);

  if(!whereConditions.empty())
  {
    where += whereConditions.front();
    for(size_t i = 1; i < whereConditions.size(); ++i)
      where += " AND " + whereConditions.at(i);
  }

  where = addLastValueFilter(dataSet, filter, where);
  if(!where.empty())
    where = " WHERE "+ where;

  std::cout << where << std::endl;
  return where;
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

  std::string query = "SELECT ";
  query+="* ";
  query+= "FROM "+tableName+" ";

  query += whereConditions(dataSet, filter);

  std::cout << query << std::endl;
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

void terrama2::core::DataAccessorPostGIS::addDateTimeFilter(terrama2::core::DataSetPtr dataSet,
    const terrama2::core::Filter& filter,
    std::vector<std::string>& whereConditions) const
{
  if(!(filter.discardBefore.get() || filter.discardAfter.get()))
    return;

  if(filter.discardBefore.get())
    whereConditions.push_back(getOutputTimestampPropertyName(dataSet)+" >= '"+filter.discardBefore->toString() + "'");

  if(filter.discardAfter.get())
    whereConditions.push_back(getOutputTimestampPropertyName(dataSet)+" <= '"+filter.discardAfter->toString() + "'");
}

void terrama2::core::DataAccessorPostGIS::addGeometryFilter(terrama2::core::DataSetPtr dataSet,
    const terrama2::core::Filter& filter,
    std::vector<std::string>& whereConditions) const
{
  if(filter.region.get())
    whereConditions.push_back(getGeometryPropertyName(dataSet)+".ST_INTERSECTS("+filter.region->asText()+")");
}

std::string terrama2::core::DataAccessorPostGIS::addLastValueFilter(terrama2::core::DataSetPtr dataSet,
                                                                       const terrama2::core::Filter& filter,
                                                                       std::string whereCondition) const
{
  if(filter.lastValue)
  {
    std::string maxSelect = "SELECT ";
    maxSelect += "MAX("+getTimestampPropertyName(dataSet)+") ";
    maxSelect += "FROM " + getDataSetTableName(dataSet)+" ";
    maxSelect += whereCondition;

    return getTimestampPropertyName(dataSet)+" = ("+maxSelect+") AND "+whereCondition;
  }

  return whereCondition;
}

void terrama2::core::DataAccessorPostGIS::updateLastTimestamp(DataSetPtr dataSet, std::shared_ptr<te::da::DataSourceTransactor> transactor) const
{
  std::string tableName = getDataSetTableName(dataSet);
  te::da::FromItem* t1 = new te::da::DataSetName(tableName);
  te::da::From* from = new te::da::From;
  from->push_back(t1);
  te::da::PropertyName* dateTimeProperty = new te::da::PropertyName(getTimestampPropertyName(dataSet));

  te::da::Fields* fields = new te::da::Fields;
  te::da::Expression* max = new te::da::Max(dateTimeProperty);
  te::da::Field* maxProperty = new te::da::Field(max);
  fields->push_back(maxProperty);

  te::da::Select select(fields, from);
  std::shared_ptr<te::da::DataSet> tempDataSet = transactor->query(select);

  //sanity check, must be 0 or 1
  assert(tempDataSet->size() < 2);

  if(tempDataSet->size() == 0)
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
