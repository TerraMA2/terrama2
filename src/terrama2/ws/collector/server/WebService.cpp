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

// STL
#include <memory>
#include <vector>

// TerraMA2
#include "../core/Utils.hpp"
#include "../../../core/DataManager.hpp"
#include "../../../core/Exception.hpp"

#include "soapWebService.h"
#include "Web.nsmap"


int WebService::ping(std::string &answer)
{
  answer = "TerraMA2 pong!";
  return SOAP_OK;
}


int WebService::addDataProvider(DataProvider struct_dataprovider, DataProvider &struct_dataproviderResult)
{
  try
  {
    auto dataProviderPtr = terrama2::ws::collector::core::Struct2DataProvider< DataProvider >(struct_dataprovider);

    terrama2::core::DataManager::getInstance().add(dataProviderPtr);

    struct_dataproviderResult = terrama2::ws::collector::core::DataProvider2Struct< DataProvider >(dataProviderPtr);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr <<  "Error at add DataProvider: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error at add DataProvider.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error at add DataProvider", "Unknow error.");
  }

  return SOAP_OK;
}


int WebService::addDataSet(DataSet struct_dataSet, DataSet &struct_datasetResult)
{
  try
  {
    auto dataSetPtr = terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataSet);

    terrama2::core::DataManager::getInstance().add(dataSetPtr);

    struct_datasetResult = terrama2::ws::collector::core::DataSet2Struct< DataSet >(dataSetPtr);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error at add DataSet: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error at add DataSet.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error at add DataSet", "Unknow error.");
  }

  return SOAP_OK;
}


int WebService::updateDataProvider(DataProvider struct_dataprovider, DataProvider &struct_dataproviderResult)
{
  try
  {
    auto dataProviderPtr = terrama2::ws::collector::core::Struct2DataProvider< DataProvider >(struct_dataprovider);

    terrama2::core::DataManager::getInstance().update(dataProviderPtr);

    struct_dataproviderResult = terrama2::ws::collector::core::DataProvider2Struct< DataProvider >(dataProviderPtr);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error to update DataProvider: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error to update DataProvider.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error to update DataProvider", "Unknow error.");
  }

  return SOAP_OK;
}


int WebService::updateDataSet(DataSet struct_dataset, DataSet &struct_datasetResult)
{
  try
  {
    auto dataSetPtr = terrama2::ws::collector::core::Struct2DataSet<DataSet>(struct_dataset);

    terrama2::core::DataManager::getInstance().update(dataSetPtr);

    struct_datasetResult = terrama2::ws::collector::core::DataSet2Struct< DataSet >(dataSetPtr);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error to update DataSet: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error to update DataSet.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error to update DataSet", "Unknow error.");
  }

  return SOAP_OK;
}


int WebService::removeDataProvider(uint64_t id)
{
  try
  {
    terrama2::core::DataManager::getInstance().removeDataProvider(id);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error to remove DataProvider:" << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error to remove DataProvider.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error to remove DataProvider", "Unknow error.");
  }

  return send_removeDataProvider_empty_response(SOAP_OK); // SOAP_OK: return HTTP 202 ACCEPTED
}


int WebService::removeDataSet(uint64_t id)
{
  try
  {
    terrama2::core::DataManager::getInstance().removeDataSet(id);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error to remove DataSet: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error to remove DataSet.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error to remove DataSet", "Unknow error.");
  }

  return send_removeDataSet_empty_response(SOAP_OK); // SOAP_OK: return HTTP 202 ACCEPTED
}


int WebService::findDataProvider(uint64_t id, DataProvider &struct_dataprovider)
{
  try
  {
    auto dataProvider = terrama2::core::DataManager::getInstance().findDataProvider(id);

    if (dataProvider.id() == 0)
    {
      std::cerr << "Error at find Data Provider: Data Provider don't exist!" << std::endl;
      return soap_receiverfault("Error at find Data Provider", "Data Provider don't exist!");
    }

    struct_dataprovider = terrama2::ws::collector::core::DataProvider2Struct<DataProvider>(dataProvider);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error at find Data Provider: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error at find Data Provider.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error at find Data Provider", "Unknow error.");
  }

  return SOAP_OK;
}


int WebService::findDataSet(uint64_t id,DataSet &struct_dataset)
{
  try
  {
    auto dataSet = terrama2::core::DataManager::getInstance().findDataSet(id);

    struct_dataset = terrama2::ws::collector::core::DataSet2Struct<DataSet>(dataSet);
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error at find DataSet: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;
    return soap_receiverfault("Error at find DataSet.", boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    return soap_receiverfault("Error at find DataSet", "Unknow error.");
  }

  return SOAP_OK;
}


int WebService::listDataProvider(std::vector< DataProvider > &data_provider_list)
{
  std::vector<terrama2::core::DataProvider> dataproviders = terrama2::core::DataManager::getInstance().providers();

  for(uint32_t i = 0; i < dataproviders.size() ; i++)
  {
    data_provider_list.push_back(terrama2::ws::collector::core::DataProvider2Struct<DataProvider>(dataproviders.at(i)));
  }

  return SOAP_OK;
}


int WebService::listDataSet(std::vector< DataSet > &data_set_list)
{
  std::vector<terrama2::core::DataSet> datasets = terrama2::core::DataManager::getInstance().dataSets();

  for(uint32_t i = 0; i < datasets.size() ; i++)
  {
    data_set_list.push_back(terrama2::ws::collector::core::DataSet2Struct<DataSet>(datasets.at(i)));
  }

  return SOAP_OK;
}
