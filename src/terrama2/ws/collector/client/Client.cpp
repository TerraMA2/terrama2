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
#include "../core/Utils.hpp"

terrama2::ws::collector::Client::Client(std::string url)
{
  wsClient_ = new WebProxy(url.c_str());
}


terrama2::ws::collector::Client::~Client()
{
  wsClient_->destroy();
  delete wsClient_;
}


int terrama2::ws::collector::Client::ping(std::string& answer)
{
  wsClient_->ping(answer);
  return 0;
}


int terrama2::ws::collector::Client::addDataProvider(terrama2::core::DataProvider dataProvider)
{
  wsClient_->addDataProvider(dataProvider);

  return 0;
}


int terrama2::ws::collector::Client::addDataset(DataSet struct_dataSet)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::updateDataProvider(DataProvider struct_dataProvider)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::updateDataSet(DataSet struct_dataSet)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::removeDataProvider(uint64_t id)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::removeDataSet(uint64_t id)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::findDataProvider(uint64_t id, DataProvider &struct_dataProvider)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::findDataSet(uint64_t id,DataSet &struct_dataSet)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::listDataProvider(std::vector< DataProvider > &dataProviderList)
{

  return SOAP_OK;
}


int terrama2::ws::collector::Client::listDataSet(std::vector< DataSet > &dataSetList)
{

  return SOAP_OK;
}
