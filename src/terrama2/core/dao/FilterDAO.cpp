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
  \file terrama2/core/dao/FilterDAO.hpp

  \brief Persistense layer for filter information associated to dataset items.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/


//TerraMA2
#include "FilterDAO.hpp"
<<<<<<< HEAD:src/terrama2/core/dao/FilterDAO.cpp
#include "../Exception.hpp"
#include "../Utils.hpp"
=======
#include "Exception.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
>>>>>>> upstream/master:src/terrama2/core/FilterDAO.cpp

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/datatype/TimeInstantTZ.h>

// Qt
#include <QObject>

//Boost
#include <boost/format.hpp>

void
terrama2::core::dao::FilterDAO::save(const Filter& filter, te::da::DataSourceTransactor& transactor)
{
  if(filter.datasetItem() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("The dataset item associated to the filter must have a valid identifier (different than 0)."));

  boost::format query("INSERT INTO terrama2.filter VALUES(%1%, %2%, %3%, %4%, %5%, %6%, %7%, %8%, %9%)");

  query.bind_arg(1, filter.datasetItem());

  if(filter.discardBefore())
    query.bind_arg(2, "'" + filter.discardBefore()->toString() + "'");
  else
    query.bind_arg(2, "NULL");

  if(filter.discardAfter())
    query.bind_arg(3, "'" + filter.discardAfter()->toString() + "'");
  else
    query.bind_arg(3, "NULL");

// geom
  if(filter.geometry())
    query.bind_arg(4, "'" + filter.geometry()->asText() + "'") ;
  else
    query.bind_arg(4, "NULL");

  // TODO: persist fiter external data id
// external_data_id
  query.bind_arg(5, "NULL");

  if(filter.value())
    query.bind_arg(6, *filter.value());
  else
    query.bind_arg(6, "NULL");

// expression_type
  query.bind_arg(7, static_cast<uint32_t>(filter.expressionType()));

// within_external_data_id
  query.bind_arg(8, "NULL");

// band_filter
  query.bind_arg(9, "'" + filter.bandFilter() + "'");

  try
  {
    transactor.execute(query.str());
  }
  catch(const std::exception& e)
  {
    const char* message = e.what();
    TERRAMA2_LOG_ERROR() << message;
    throw DataAccessException() << ErrorDescription(message);
  }
  catch(...)
  {
    QString message(QObject::tr("Unexpected error saving filter information for dataset item: %1"));

    message = message.arg(filter.datasetItem());

    TERRAMA2_LOG_ERROR() << message;

    throw DataAccessException() << ErrorDescription(message);
  }
}

void
terrama2::core::dao::FilterDAO::update(const Filter& filter, te::da::DataSourceTransactor& transactor)
{
  if(filter.datasetItem() == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("The dataset item associated to the filter must have a valid identifier (different than 0)."));

  boost::format query("UPDATE terrama2.filter SET discard_before = %1%, "
                      "discard_after = %2%, geom = %3%, external_data_id = %4%, "
                      "value = %5%, expression_type = %6%, within_external_data_id = %7%, "
                      "band_filter = %8% WHERE dataset_item_id = %9%");

  if(filter.discardBefore())
    query.bind_arg(1, "'" + filter.discardBefore()->toString() + "'");
  else
    query.bind_arg(1, "NULL");

  if(filter.discardAfter())
    query.bind_arg(2, "'" + filter.discardAfter()->toString() + "'");
  else
    query.bind_arg(2, "NULL");

// geom
  if(filter.geometry())
    query.bind_arg(3, "'" + filter.geometry()->asText() + "'") ;
  else
    query.bind_arg(3, "NULL");

// external_data_id
  query.bind_arg(4, "NULL");

  if(filter.value())
    query.bind_arg(5, *filter.value());
  else
    query.bind_arg(5, "NULL");

// expression_type
  query.bind_arg(6, static_cast<uint32_t>(filter.expressionType()));

// within_external_data_id
  query.bind_arg(7, "NULL");

// band_filter
  query.bind_arg(8, "'" + filter.bandFilter() + "'");

  query.bind_arg(9, filter.datasetItem());

  try
  {
    transactor.execute(query.str());
  }
  catch(const std::exception& e)
  {
    const char* message = e.what();
    TERRAMA2_LOG_ERROR() << message;
    throw DataAccessException() << ErrorDescription(message);
  }
  catch(...)
  {
    QString message(QObject::tr("Unexpected error updating filter information for dataset item: %1"));

    message = message.arg(filter.datasetItem());

    TERRAMA2_LOG_ERROR() << message;

    throw DataAccessException() << ErrorDescription(message);
  }
}

void
terrama2::core::dao::FilterDAO::remove(uint64_t datasetItemId, te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not remove filter information for an invalid dataset item identifier: 0."));

  std::string sql("DELETE FROM terrama2.filter WHERE dataset_item_id = ");
              sql += std::to_string(datasetItemId);

  try
  {
    transactor.execute(sql);
  }
  catch(const std::exception& e)
  {
    const char* message = e.what();
    TERRAMA2_LOG_ERROR() << message;
    throw DataAccessException() << ErrorDescription(message);
  }
  catch(...)
  {
    QString message(QObject::tr("Unexpected error removing filter information for dataset item: %1"));

    message = message.arg(datasetItemId);

    TERRAMA2_LOG_ERROR() << message;

    throw DataAccessException() << ErrorDescription(message);
  }
}

terrama2::core::Filter
terrama2::core::dao::FilterDAO::load(const DataSetItem& datasetItem, te::da::DataSourceTransactor& transactor)
{
  uint64_t datasetItemId = datasetItem.id();
  if(datasetItemId == 0)
    throw InvalidArgumentException() << ErrorDescription(QObject::tr("Can not load filter information for an invalid dataset item identifier: 0."));

  std::string sql("SELECT * FROM terrama2.filter WHERE dataset_item_id = ");
              sql += std::to_string(datasetItemId);

  try
  {
    std::auto_ptr<te::da::DataSet> filter_result = transactor.query(sql);

    if(!filter_result->moveNext())
      return std::move(Filter());

    Filter filter(datasetItemId);

    if(!filter_result->isNull("discard_before"))
    {
      auto discardBefore = filter_result->getDateTime("discard_before");
      auto titz = dynamic_cast<te::dt::TimeInstantTZ*>(discardBefore.release());
      boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(datasetItem.timezone()));
      boost::local_time::local_date_time localtime = titz->getTimeInstantTZ().local_time_in(zone);
      filter.setDiscardBefore(std::unique_ptr<te::dt::TimeInstantTZ>(new te::dt::TimeInstantTZ(localtime)));
    }

    if(!filter_result->isNull("discard_after"))
    {
      auto discardAfter = filter_result->getDateTime("discard_after");
      auto titz = dynamic_cast<te::dt::TimeInstantTZ*>(discardAfter.release());
      boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(datasetItem.timezone()));
      boost::local_time::local_date_time_base<> localtime = titz->getTimeInstantTZ().local_time_in(zone);
      filter.setDiscardAfter(std::unique_ptr<te::dt::TimeInstantTZ>(new te::dt::TimeInstantTZ(localtime)));
    }

    if(!filter_result->isNull(3))
      filter.setGeometry(filter_result->getGeometry("geom"));

    filter.setExpressionType(ToFilterExpressionType(filter_result->getInt32("expression_type")));

    if(!filter_result->isNull("value"))
    {
      double* v = new double(atof(filter_result->getNumeric("value").c_str()));
      std::unique_ptr<double> byValue(v);
      filter.setValue(std::move(byValue));
    }
    else
    {
      std::unique_ptr<double> byValue(nullptr);
      filter.setValue(std::move(byValue));
    }

    if(!filter_result->isNull("band_filter"))
      filter.setBandFilter(filter_result->getString("band_filter"));

    return std::move(filter);
  }
  catch(const terrama2::Exception& e)
  {
    if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
      TERRAMA2_LOG_ERROR() << message->toStdString();
    throw;
  }
  catch(const std::exception& e)
  {
    const char* message = e.what();
    TERRAMA2_LOG_ERROR() << message;
    throw DataAccessException() << ErrorDescription(message);
  }
  catch(...)
  {
    QString message(QObject::tr("Unexpected error loading filter information for dataset item: %1"));

    message = message.arg(datasetItemId);

    TERRAMA2_LOG_ERROR() << message;

    throw DataAccessException() << ErrorDescription(message);
  }
}



