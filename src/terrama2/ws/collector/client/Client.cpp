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
  \file terrama2/ws/collector/client/Client.cpp

  \brief Client of TerraMA2 Collector Web Service.

  \author Vinicius Campanha
 */

// TerraMA2
#include "Client.hpp"
#include "Exception.hpp"
#include "Web.nsmap"
#include "WebProxyAdapter.hpp"
#include "../core/Utils.hpp"

terrama2::ws::collector::client::Client::Client(WebProxyAdapter* webProxy)
: webProxy_(webProxy)
{
}


terrama2::ws::collector::client::Client::~Client()
{
  webProxy_->destroy();

  delete webProxy_;
}


void terrama2::ws::collector::client::Client::ping(std::string &answer)
{
  if(webProxy_->ping(answer) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::PingError() << ErrorDescription(errorMessage.c_str());
  }
}


void terrama2::ws::collector::client::Client::shutdown()
{
  if(webProxy_->shutdown() != SOAP_OK || webProxy_->recvShutdownEmptyResponse() != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::ShutdownError() << ErrorDescription(errorMessage.c_str());
  }
}


void terrama2::ws::collector::client::Client::addDataProvider(terrama2::core::DataProvider& dataProvider)
{

  DataProvider struct_dataProvider = terrama2::ws::collector::core::DataProvider2Struct<DataProvider>(dataProvider);

  DataProvider struct_dataProviderResult;

  if(webProxy_->addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::AddingDataProviderError() << ErrorDescription(errorMessage.c_str());
  }

  dataProvider = terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProviderResult);

}


void terrama2::ws::collector::client::Client::addDataSet(terrama2::core::DataSet & dataSet)
{
  if(dataSet.name().empty())
    throw client::AddingDataSetError() << ErrorDescription(QObject::tr("Null parameter passed!"));

  DataSet struct_dataSet = terrama2::ws::collector::core::DataSet2Struct<DataSet>(dataSet);

  DataSet struct_dataSetResult;

  if(webProxy_->addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::AddingDataSetError() << ErrorDescription(errorMessage.c_str());
  }

  dataSet = terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSetResult);

}


void terrama2::ws::collector::client::Client::updateDataProvider(terrama2::core::DataProvider & dataProvider)
{
  if(dataProvider.name().empty())
    throw client::UpdateDataProviderError() << ErrorDescription(QObject::tr("Null parameter passed!"));

  DataProvider struct_dataProvider = terrama2::ws::collector::core::DataProvider2Struct<DataProvider>(dataProvider);

  DataProvider struct_dataProviderResult;

  if(webProxy_->updateDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::UpdateDataProviderError() << ErrorDescription(errorMessage.c_str());
  }

  dataProvider = terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProviderResult);

}


void terrama2::ws::collector::client::Client::updateDataSet(terrama2::core::DataSet & dataSet)
{
  if(dataSet.id() == 0)
    throw client::UpdateDataSetError() << ErrorDescription(QObject::tr("Invalid dataset passed!"));

  DataSet struct_dataSet = terrama2::ws::collector::core::DataSet2Struct<DataSet>(dataSet);

  DataSet struct_dataSetResult;

  if(webProxy_->updateDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::UpdateDataSetError() << ErrorDescription(errorMessage.c_str());
  }

  dataSet = terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSetResult);

}


void terrama2::ws::collector::client::Client::removeDataProvider(uint64_t id)
{
  if(webProxy_->removeDataProvider(id) != SOAP_OK || webProxy_->recvRemoveDataProviderEmptyResponse() != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::RemoveDataProviderError() << ErrorDescription(errorMessage.c_str());
  }
}


void terrama2::ws::collector::client::Client::removeDataSet(uint64_t id)
{
  if(webProxy_->removeDataSet(id) != SOAP_OK || webProxy_->recvRemoveDatasetEmptyResponse() != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::RemoveDataSetError() << ErrorDescription(errorMessage.c_str());
  }

}


terrama2::core::DataProvider terrama2::ws::collector::client::Client::findDataProvider(uint64_t id)
{
  DataProvider struct_dataProvider;

  if(webProxy_->findDataProvider(id, struct_dataProvider) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::FindDataProviderError() << ErrorDescription(errorMessage.c_str());
  }

  return terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProvider);

}


terrama2::core::DataSet terrama2::ws::collector::client::Client::findDataSet(uint64_t id)
{
  DataSet struct_dataSet;

  if(webProxy_->findDataSet(id, struct_dataSet) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::FindDataSetError() << ErrorDescription(errorMessage.c_str());
  }

  return terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSet);

}


void terrama2::ws::collector::client::Client::listDataProvider(std::vector< terrama2::core::DataProvider > & providers)
{
  std::vector< DataProvider > struct_dataProviderList;

  if(webProxy_->listDataProvider(struct_dataProviderList) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::ListDataProviderError() << ErrorDescription(errorMessage.c_str());
  }

  for(uint32_t i = 0; i < struct_dataProviderList.size() ; i++)
  {
    providers.push_back(terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProviderList.at(i)));
  }

}


void terrama2::ws::collector::client::Client::listDataSet(std::vector< terrama2::core::DataSet > & datasets)
{
  std::vector< DataSet > struct_dataSetList;

  if(webProxy_->listDataSet(struct_dataSetList) != SOAP_OK)
  {
    std::string errorMessage = std::string(webProxy_->faultString()) + ": " + std::string(webProxy_->faultDetail());

    throw client::ListDataSetError() << ErrorDescription(errorMessage.c_str());
  }

  for(uint32_t i = 0; i < struct_dataSetList.size() ; i++)
  {
    datasets.push_back(terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSetList.at(i)));
  }

}
