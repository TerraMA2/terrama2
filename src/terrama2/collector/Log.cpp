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
  \file terrama2/collector/Log.cpp

  \brief Manage the log of data handled by collector service

  \author Vinicius Campanha
*/

// Boost
#include <boost/format.hpp>

// TerraMA2
#include "Exception.hpp"
#include "Log.hpp"
#include "../core/ApplicationController.hpp"


uint64_t terrama2::collector::Log::log(uint64_t dataSetItemId, std::string origin_uri, Status s)
{
  try
  {
    std::shared_ptr< te::da::DataSourceTransactor > transactor(terrama2::core::ApplicationController::getInstance().getTransactor());

    boost::format query("INSERT INTO terrama2.data_collection_log (dataset_item_id, origin_uri, status) VALUES('%1%', '%2%', %3%)");

    query.bind_arg(1, dataSetItemId);
    query.bind_arg(2, origin_uri);
    query.bind_arg(3, (int)s);

    transactor->execute(query.str());

    return transactor->getLastGeneratedId();
  }
  catch(te::common::Exception& e)
  {
    throw LogError() << ErrorDescription( e.what());;
  }
  catch(terrama2::Exception& e)
  {
    throw LogError() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(std::exception& e)
  {
    throw LogError() << ErrorDescription( e.what());
  }
  catch(...)
  {
    throw LogError() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
  return 0;
}

void terrama2::collector::Log::log(uint64_t dataSetItemId, std::vector< std::string > origin_uris, Status s)
{
  try
  {
    std::shared_ptr< te::da::DataSourceTransactor > transactor(terrama2::core::ApplicationController::getInstance().getTransactor());

    std::string query("INSERT INTO terrama2.data_collection_log (dataset_item_id, origin_uri, status) VALUES");

    int size = origin_uris.size();

    for(int i = 0; i < size; i++)
    {
      boost::format value("('%1%', '%2%', %3%)");

      value.bind_arg(1, dataSetItemId);
      value.bind_arg(2, origin_uris.at(i));
      value.bind_arg(3, (int)s);

      query += value.str();

      if(i != (size-1))
        query += ",";
    }

    transactor->execute(query);
  }
  catch(terrama2::Exception& e)
  {
    throw LogError() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(te::common::Exception& e)
  {
    throw LogError() << ErrorDescription(e.what());;
  }
  catch(std::exception& e)
  {
    throw LogError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw LogError() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
}

void terrama2::collector::Log::updateLog(uint64_t id, std::string uri, Status s, std::string data_timestamp)
{
  try
  {
    std::shared_ptr< te::da::DataSourceTransactor > transactor(terrama2::core::ApplicationController::getInstance().getTransactor());

    boost::format query("UPDATE terrama2.data_collection_log SET status=%2%, data_timestamp='%3%', uri='%4%', collect_timestamp=now() WHERE id=%1%");

    query.bind_arg(1, id);
    query.bind_arg(2, (int)s);
    query.bind_arg(3, data_timestamp);
    query.bind_arg(4, uri);

    transactor->execute(query.str());
  }
  catch(terrama2::Exception& e)
  {
    throw LogError() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(te::common::Exception& e)
  {
    throw LogError() << ErrorDescription( e.what());;
  }
  catch(std::exception& e)
  {
    throw LogError() << ErrorDescription( e.what());
  }
  catch(...)
  {
    throw LogError() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
}


void terrama2::collector::Log::updateLog(std::string origin_uri, std::string uri, Status s, std::string data_timestamp)
{
  try
  {
    std::shared_ptr< te::da::DataSourceTransactor > transactor(terrama2::core::ApplicationController::getInstance().getTransactor());

    boost::format query("UPDATE terrama2.data_collection_log SET status=%2%, data_timestamp='%3%', uri='%4%', collect_timestamp=now() WHERE origin_uri='%1%'");

    query.bind_arg(1, origin_uri);
    query.bind_arg(2, (int)s);
    query.bind_arg(3, data_timestamp);
    query.bind_arg(4, uri);

    transactor->execute(query.str());
  }
  catch(terrama2::Exception& e)
  {
    throw LogError() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(te::common::Exception& e)
  {
    throw LogError() << ErrorDescription( e.what());;
  }
  catch(std::exception& e)
  {
    throw LogError() << ErrorDescription( e.what());
  }
  catch(...)
  {
    throw LogError() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
}


std::shared_ptr<te::dt::DateTime> terrama2::collector::Log::getDataSetItemLastDateTime(uint64_t id)
{
  std::shared_ptr< te::da::DataSourceTransactor > transactor(terrama2::core::ApplicationController::getInstance().getTransactor());

  boost::format query("select MAX(data_timestamp) from terrama2.data_collection_log where dataset_item_id=%1%");

  query.bind_arg(1, id);

  std::shared_ptr< te::da::DataSet > dataset = std::shared_ptr< te::da::DataSet >(transactor->query(query.str()));

  if(dataset)
    return std::shared_ptr< te::dt::DateTime >(dataset->getDateTime("data_timestamp"));

  return nullptr;
}
