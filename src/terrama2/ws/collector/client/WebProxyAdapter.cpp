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
  \file terrama2/ws/collector/client/WebProxyAdapter.hpp

  \brief Adapter for gSOAP WebProxy class.

  \author Paulo R. M. Oliveira
*/

#include "WebProxyAdapter.hpp"

terrama2::ws::collector::client::WebProxyAdapter::WebProxyAdapter()
: webProxy_(nullptr)
{
}

terrama2::ws::collector::client::WebProxyAdapter::WebProxyAdapter(const std::string server)
: server_(server)
{
  webProxy_ = new WebProxy(server_.c_str());
}

terrama2::ws::collector::client::WebProxyAdapter::~WebProxyAdapter()
{
  delete webProxy_;
}

void terrama2::ws::collector::client::WebProxyAdapter::destroy()
{
  return webProxy_->destroy();
}

int terrama2::ws::collector::client::WebProxyAdapter::ping(std::string &answer)
{
  return webProxy_->ping(answer);
}

int terrama2::ws::collector::client::WebProxyAdapter::addDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult)
{
  return webProxy_->addDataProvider(dataProvider, dataProviderResult);
}


int terrama2::ws::collector::client::WebProxyAdapter::addDataSet(struct DataSet dataset, struct DataSet& datasetResult)
{
  return webProxy_->addDataSet(dataset, datasetResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::updateDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult)
{
  return webProxy_->updateDataProvider(dataProvider, dataProviderResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::updateDataSet(struct DataSet dataset, struct DataSet& datasetResult)
{
  return webProxy_->updateDataSet(dataset, datasetResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::removeDataProvider(uint64_t id)
{
  return webProxy_->send_removeDataProvider(id);
}

int terrama2::ws::collector::client::WebProxyAdapter::removeDataSet(uint64_t id)
{
  return webProxy_->send_removeDataSet(id);
}

int terrama2::ws::collector::client::WebProxyAdapter::findDataProvider(uint64_t id, struct DataProvider& dataProviderResult)
{
  return webProxy_->findDataProvider(id, dataProviderResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::findDataSet(uint64_t id, struct DataSet& datasetResult)
{
  return webProxy_->findDataSet(id, datasetResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::listDataProvider(std::vector<struct DataProvider>& providers)
{
  return webProxy_->listDataProvider(providers);
}

int terrama2::ws::collector::client::WebProxyAdapter::listDataSet(std::vector<struct DataSet>& datasets)
{
  return webProxy_->listDataSet(datasets);
}

std::string terrama2::ws::collector::client::WebProxyAdapter::faultString()
{
  return webProxy_->soap_fault_string();
}

std::string terrama2::ws::collector::client::WebProxyAdapter::faultDetail()
{
  return webProxy_->soap_fault_detail();
}

int terrama2::ws::collector::client::WebProxyAdapter::recvRemoveDataProviderEmptyResponse()
{
  return webProxy_->recv_removeDataProvider_empty_response();
}

int terrama2::ws::collector::client::WebProxyAdapter::recvRemoveDatasetEmptyResponse()
{
  return webProxy_->recv_removeDataSet_empty_response();
}

int terrama2::ws::collector::client::WebProxyAdapter::shutdown()
{
  return webProxy_->send_shutdown();
}

int terrama2::ws::collector::client::WebProxyAdapter::recvShutdownEmptyResponse()
{
  return webProxy_->recv_shutdown_empty_response();
}
