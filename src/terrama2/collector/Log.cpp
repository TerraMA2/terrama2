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

//Terrlib
#include <terralib/dataaccess/utils/Utils.h>

// TerraMA2
#include "Exception.hpp"
#include "Log.hpp"
#include "../core/ApplicationController.hpp"



terrama2::collector::Log::Log(std::shared_ptr < te::da::DataSourceTransactor > transactor)
{
  transactor_ = transactor;
}

uint64_t terrama2::collector::Log::log(const uint64_t dataSetItemId, const std::string& origin_uri, const Status s) const
{
  try
  {
    boost::format query("INSERT INTO terrama2.data_collection_log (dataset_item_id, origin_uri, status) VALUES('%1%', '%2%', %3%)");

    query.bind_arg(1, dataSetItemId);
    query.bind_arg(2, origin_uri);
    query.bind_arg(3, static_cast<int>s);

    transactor_->execute(query.str());

    transactor_->commit();

    return transactor_->getLastGeneratedId();
  }
  catch(te::common::Exception& e)
  {
    throw LogException() << ErrorDescription( e.what());;
  }
  catch(terrama2::Exception& e)
  {
    throw LogException() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(std::exception& e)
  {
    throw LogException() << ErrorDescription( e.what());
  }
  catch(...)
  {
    throw LogException() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
  return 0;
}

void terrama2::collector::Log::log(const std::vector<TransferenceData>& transferenceDataVec, const Status s) const
{
  if(transferenceDataVec.empty())
    throw LogException() << ErrorDescription("terrama2::collector::Log: No files to log.");

  try
  {
    std::string query("INSERT INTO terrama2.data_collection_log (dataset_item_id, origin_uri, uri, data_timestamp, collect_timestamp, status) VALUES");

    bool first = true;

    for( auto& transferenceData : transferenceDataVec)
    {
      boost::format value("(%1%, %2%, %3%, %4%, %5%, %6%)");

      value.bind_arg(1, transferenceData.dataSetItem.id());

      if(transferenceData.uriOrigin.empty())
        value.bind_arg(2, "NULL");
      else
        value.bind_arg(2, "'" + transferenceData.uriOrigin + "'");

      if(transferenceData.uriStorage.empty())
        value.bind_arg(3, "NULL");
      else
        value.bind_arg(3, "'" + transferenceData.uriStorage + "'");

      if(!transferenceData.dateData)
        value.bind_arg(4, "NULL");
      else
        value.bind_arg(4, "'" + transferenceData.dateData->toString() + "'");

      if(!transferenceData.dateCollect)
        value.bind_arg(5, "now()");
      else
        value.bind_arg(5, "'" + transferenceData.dateCollect->toString() + "'");

      value.bind_arg(6, static_cast<int>s);

      if(!first)
      {
        query += ",";
      }

      query += value.str();
      first = false;
    }

    transactor_->execute(query);

    transactor_->commit();
  }
  catch(terrama2::Exception& e)
  {
    throw LogException() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(te::common::Exception& e)
  {
    throw LogException() << ErrorDescription(e.what());;
  }
  catch(std::exception& e)
  {
    throw LogException() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw LogException() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
}

void terrama2::collector::Log::updateLog(const uint64_t id, const std::string& uri, const Status s, const std::string& data_timestamp) const
{
  try
  {
    boost::format query("UPDATE terrama2.data_collection_log SET status=%2%, data_timestamp='%3%', uri='%4%', collect_timestamp=now() WHERE id=%1%");

    query.bind_arg(1, id);
    query.bind_arg(2, static_cast<int>s);
    query.bind_arg(3, data_timestamp);
    query.bind_arg(4, uri);

    transactor_->execute(query.str());

    transactor_->commit();
  }
  catch(terrama2::Exception& e)
  {
    throw LogException() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(te::common::Exception& e)
  {
    throw LogException() << ErrorDescription( e.what());;
  }
  catch(std::exception& e)
  {
    throw LogException() << ErrorDescription( e.what());
  }
  catch(...)
  {
    throw LogException() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
}


void terrama2::collector::Log::updateLog(const std::vector< std::string >& origin_uris, const std::string& uri, const Status s, const std::string& data_timestamp) const
{
  if(origin_uris.empty())
    throw LogException() << ErrorDescription("terrama2::collector::Log: No files to update.");

  try
  {
    boost::format query("UPDATE terrama2.data_collection_log SET status=%2%, data_timestamp=%3%, uri='%4%', collect_timestamp=now() WHERE origin_uri=%1%");

    std::string uris;
    bool first = true;
    // VINICIUS: if are too many rows to update, split it
    if(origin_uris.size() > 1)
    {
      for(auto uri : origin_uris)
      {
        if(first)
        {
          uris = "'" + uri + "'";
          first =false;
        }
        else
          uris +=  " OR origin_uri = '" + uri + "'";
      }
    }
    else
    {
      uris = "'" + origin_uris.at(0) + "'";
    }

    query.bind_arg(1, uris);
    query.bind_arg(2, static_cast<int>s);

    if(data_timestamp.empty())
      query.bind_arg(3, "NULL");
    else
      query.bind_arg(3, "'" + data_timestamp + "'");

    query.bind_arg(4, uri);

    transactor_->execute(query.str());

    transactor_->commit();
  }
  catch(terrama2::Exception& e)
  {
    throw LogException() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(te::common::Exception& e)
  {
    throw LogException() << ErrorDescription( e.what());;
  }
  catch(std::exception& e)
  {
    throw LogException() << ErrorDescription( e.what());
  }
  catch(...)
  {
    throw LogException() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
}


void terrama2::collector::Log::updateLog(const std::vector<TransferenceData>& transferenceDataVec, const Status s) const
{
  if(transferenceDataVec.empty())
    throw LogException() << ErrorDescription("terrama2::collector::Log: No files to update.");

  try
  {
    for( auto& transferenceData : transferenceDataVec)
    {
      boost::format query("UPDATE terrama2.data_collection_log SET status=%2%, data_timestamp=%3%, uri='%4%', collect_timestamp=%5% WHERE origin_uri='%1%'");

      query.bind_arg(1, transferenceData.uriOrigin);
      query.bind_arg(2, static_cast<int>s);

      if(!transferenceData.dateData)
        query.bind_arg(3, "NULL");
      else
        query.bind_arg(3, "'" + transferenceData.dateData->toString() + "'");

      query.bind_arg(4, transferenceData.uriStorage);

      if(!transferenceData.dateCollect)
        query.bind_arg(5, "NULL");
      else
        query.bind_arg(5, "'" + transferenceData.dateCollect->toString() + "'");

      transactor_->execute(query.str());
    }

    transactor_->commit();

  }
  catch(terrama2::Exception& e)
  {
    throw LogException() << ErrorDescription(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(te::common::Exception& e)
  {
    throw LogException() << ErrorDescription( e.what());;
  }
  catch(std::exception& e)
  {
    throw LogException() << ErrorDescription( e.what());
  }
  catch(...)
  {
    throw LogException() << ErrorDescription("terrama2::collector::Log: Unknow error");
  }
}


std::shared_ptr<te::dt::TimeInstantTZ> terrama2::collector::Log::getDataSetItemLastDateTime(uint64_t id) const
{
  boost::format query("select MAX(data_timestamp) from terrama2.data_collection_log where dataset_item_id=%1%");

  query.bind_arg(1, id);

  std::shared_ptr< te::da::DataSet > dataset = std::shared_ptr< te::da::DataSet >(transactor_->query(query.str()));

  if(dataset)
  {
    size_t columnPos = te::da::GetPropertyPos(dataset.get(), "max");

    if(dataset->moveNext() && !dataset->isNull(columnPos))
    {
      return std::shared_ptr< te::dt::TimeInstantTZ >(dynamic_cast<te::dt::TimeInstantTZ*>(dataset->getDateTime(columnPos).release()));
    }
  }

  return nullptr;
}
