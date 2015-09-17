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
  \file terrama2/ws/collector/server/WebService.cpp

  \brief TerraMA2 server gSOAP interfaces implementation.

  \author Vinicius Campanha
 */

// TerraMA2
#include "soapWebService.h"
#include "Web.nsmap"


int WebService::ping(std::string &answer)
{
  answer = "TerraMA2 pong!";
  return SOAP_OK;
}


int WebService::restart(void)
{
  return SOAP_OK;
}


int WebService::shutdown(void)
{
  return SOAP_OK;
}


int WebService::addDataProvider(struct DataProvider)
{
  return SOAP_OK;
}


int WebService::addDataset(struct DataSet)
{
  return SOAP_OK;
}


int WebService::updateDataProvider(struct DataProvider)
{
  return SOAP_OK;
}


int WebService::updateDataSet(struct DataSet)
{
  return SOAP_OK;
}


int WebService::removeDataProvider(uint64_t id)
{
  return SOAP_OK;
}


int WebService::removeDataSet(uint64_t id)
{
  return SOAP_OK;
}


int WebService::findDataProvider(uint64_t id, struct DataProvider &r)
{
  return SOAP_OK;
}


int WebService::findDataSet(uint64_t id, struct DataSet &r)
{
  return SOAP_OK;
}


int WebService::listDataProvider(std::vector< struct DataProvider > &data_provider_list)
{
  return SOAP_OK;
}


int WebService::listDataSet(std::vector< struct DataSet > &data_set_list)
{
  return SOAP_OK;
}
