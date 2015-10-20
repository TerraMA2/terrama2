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
  \file terrama2/core/FilterDAO.hpp

  \brief Persistense layer for filter information associated to dataset items.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/


//TerraMA2
#include "FilterDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// Qt
#include <QObject>

//Boost
#include <boost/format.hpp>

void
terrama2::core::FilterDAO::save(const Filter& filter, te::da::DataSourceTransactor& transactor)
{
  if(filter.datasetItem() == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("The dataset item associated to the filter must have a valid identifier (different than 0)."));

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

  // TODO: persist filter geometry
// geom
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
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error saving filter information for dataset item: %1"));

    err_msg = err_msg.arg(filter.datasetItem());

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

void
terrama2::core::FilterDAO::update(const Filter& filter, te::da::DataSourceTransactor& transactor)
{
  if(filter.datasetItem() == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("The dataset item associated to the filter must have a valid identifier (different than 0)."));

  boost::format query("UPDATE terrama2.filter SET discard_before = %1%, "
                      "discard_after = %2%, geom = %3%, external_data_id = %4%, "
                      "value = %5%, expression_type = %6%, within_external_data_id = %7%, "
                      "band_filter = %8% WHERE dataset_item_id = %9%)");

  if(filter.discardBefore())
    query.bind_arg(1, "'" + filter.discardBefore()->toString() + "'");
  else
    query.bind_arg(1, "NULL");

  if(filter.discardAfter())
    query.bind_arg(2, "'" + filter.discardAfter()->toString() + "'");
  else
    query.bind_arg(2, "NULL");

// geom
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
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    QString err_msg(QObject::tr("Unexpected error updating filter information for dataset item: %1"));

    err_msg = err_msg.arg(filter.datasetItem());

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

void
terrama2::core::FilterDAO::remove(uint64_t datasetItemId, te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not remove filter information for an invalid dataset item identifier: 0."));

  std::string sql("DELETE FROM terrama2.filter WHERE dataset_item_id = ");
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
    QString err_msg(QObject::tr("Unexpected error removing filter information for dataset item: %1"));

    err_msg = err_msg.arg(datasetItemId);

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}

terrama2::core::Filter
terrama2::core::FilterDAO::load(uint64_t datasetItemId, te::da::DataSourceTransactor& transactor)
{
  if(datasetItemId == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not load filter information for an invalid dataset item identifier: 0."));

  std::string sql("SELECT * FROM terrama2.filter WHERE dataset_item_id = ");
              sql += std::to_string(datasetItemId);

  try
  {
    std::auto_ptr<te::da::DataSet> filter_result = transactor.query(sql);

    if(!filter_result->moveNext())
      return std::move(Filter());

    Filter filter(datasetItemId);

    filter.setDiscardBefore(filter_result->getDateTime("discard_before"));
    filter.setDiscardAfter(filter_result->getDateTime("discard_after"));
    
    if(!filter_result->isNull(3))
      filter.setGeometry(filter_result->getGeometry("geom"));
    
    filter.setExpressionType(ToFilterExpressionType(filter_result->getInt32("by_value_type")));

    if(!filter_result->isNull("by_value"))
    {
      double v = atof(filter_result->getNumeric("by_value").c_str());
      std::unique_ptr<double> byValue(&v);
      filter.setValue(std::move(byValue));
    }
    else
    {
      std::unique_ptr<double> byValue(nullptr);
      filter.setValue(std::move(byValue));
    }
    
    filter.setBandFilter(filter_result->getString("band_filter"));
    
    return std::move(filter);
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
    QString err_msg(QObject::tr("Unexpected error loading filter information for dataset item: %1"));

    err_msg = err_msg.arg(datasetItemId);

    throw DataAccessError() << ErrorDescription(err_msg);
  }
}



