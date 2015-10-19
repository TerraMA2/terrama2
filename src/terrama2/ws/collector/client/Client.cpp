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
#include "../core/Utils.hpp"
#include "Web.nsmap"

terrama2::ws::collector::Client::Client(const std::string url)
{
  server_ = url;

  wsClient_ = new WebProxy(server_.c_str());
}


terrama2::ws::collector::Client::~Client()
{
  wsClient_->destroy();

  delete wsClient_;
}


void terrama2::ws::collector::Client::ping(std::string &answer)
{
  if(wsClient_->ping(answer) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::PingError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }
}


void terrama2::ws::collector::Client::addDataProvider(terrama2::core::DataProvider& dataProvider)
{

  DataProvider struct_dataProvider = terrama2::ws::collector::core::DataProvider2Struct<DataProvider>(dataProvider);

  DataProvider struct_dataProviderResult;

  if(wsClient_->addDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::AddingDataProviderError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  dataProvider = terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProviderResult);

}


void terrama2::ws::collector::Client::addDataSet(terrama2::core::DataSet & dataSet)
{
  if(dataSet.name().empty())
    throw client::AddingDataSetError() << ErrorDescription(QObject::tr("Null parameter passed!"));

  DataSet struct_dataSet = terrama2::ws::collector::core::DataSet2Struct<DataSet>(dataSet);

  DataSet struct_dataSetResult;

  if(wsClient_->addDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::AddingDataSetError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  dataSet = terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSetResult);

}


void terrama2::ws::collector::Client::updateDataProvider(terrama2::core::DataProvider & dataProvider)
{
  if(dataProvider.name().empty())
    throw client::UpdateDataProviderError() << ErrorDescription(QObject::tr("Null parameter passed!"));

  DataProvider struct_dataProvider = terrama2::ws::collector::core::DataProvider2Struct<DataProvider>(dataProvider);

  DataProvider struct_dataProviderResult;

  if(wsClient_->updateDataProvider(struct_dataProvider, struct_dataProviderResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::UpdateDataProviderError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  dataProvider = terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProviderResult);

}


void terrama2::ws::collector::Client::updateDataSet(terrama2::core::DataSet & dataSet)
{
  if(dataSet.id() == 0)
    throw client::UpdateDataSetError() << ErrorDescription(QObject::tr("Invalid dataset passed!"));

  DataSet struct_dataSet = terrama2::ws::collector::core::DataSet2Struct<DataSet>(dataSet);

  DataSet struct_dataSetResult;

  if(wsClient_->updateDataSet(struct_dataSet, struct_dataSetResult) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::UpdateDataSetError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  dataSet = terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSetResult);

}


void terrama2::ws::collector::Client::removeDataProvider(uint64_t id)
{
  if(wsClient_->send_removeDataProvider(id) != SOAP_OK || wsClient_->recv_removeDataProvider_empty_response() != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::RemoveDataProviderError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }
}


void terrama2::ws::collector::Client::removeDataSet(uint64_t id)
{
  if(wsClient_->removeDataSet(id) != SOAP_OK || wsClient_->recv_removeDataSet_empty_response() != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::RemoveDataSetError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

}


terrama2::core::DataProvider terrama2::ws::collector::Client::findDataProvider(uint64_t id)
{
  DataProvider struct_dataProvider;

  if(wsClient_->findDataProvider(id, struct_dataProvider) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::FindDataProviderError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  return terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProvider);

}


terrama2::core::DataSet terrama2::ws::collector::Client::findDataSet(uint64_t id)
{
  DataSet struct_dataSet;

  if(wsClient_->findDataSet(id, struct_dataSet) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::FindDataSetError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  return terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSet);

}


void terrama2::ws::collector::Client::listDataProvider(std::vector< terrama2::core::DataProvider > & providers)
{
  if(providers.at(0).id() == 0)
    throw client::ListDataProviderError() << ErrorDescription(QObject::tr("Invalid data provider passed!"));

  std::vector< DataProvider > struct_dataProviderList;

  if(wsClient_->listDataProvider(struct_dataProviderList) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::ListDataProviderError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  for(uint32_t i = 0; i < struct_dataProviderList.size() ; i++)
  {
    providers.push_back(terrama2::ws::collector::core::Struct2DataProvider<DataProvider>(struct_dataProviderList.at(i)));
  }

}


void terrama2::ws::collector::Client::listDataSet(std::vector< terrama2::core::DataSet > & datasets)
{
  if(datasets.at(0).id() == 0)
    throw client::ListDataSetError() << ErrorDescription(QObject::tr("Invalid dataset pased!"));

  std::vector< DataSet > struct_dataSetPtrList;

  if(wsClient_->listDataSet(struct_dataSetPtrList) != SOAP_OK)
  {
    std::string errorMessage = std::string(wsClient_->soap_fault_string()) + ": " + std::string(wsClient_->soap_fault_detail());

    throw client::ListDataSetError() << ErrorDescription(QObject::tr(errorMessage.c_str()));
  }

  for(uint32_t i = 0; i < struct_dataSetPtrList.size() ; i++)
  {
    datasets.push_back(terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSetPtrList.at(i)));
  }

}
