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
  \file terrama2/core/DataManagerIntermediator.cpp

  \brief Helper function to convert from JSON and to JSON, TerraMA² data.

  \author Jano Simas
*/

#include "DataManagerIntermediator.hpp"
#include "DataManager.hpp"
#include "DataProvider.hpp"
#include "DataSet.hpp"
#include "Logger.hpp"
#include "TcpDispatcher.hpp"

//qT
#include <QTcpSocket>
#include <QJsonDocument>

//STL
#include <algorithm>

terrama2::core::DataManagerIntermediator::DataManagerIntermediator(const std::string& instanceName)
  : instanceName_(instanceName)
{
}

void terrama2::core::DataManagerIntermediator::fromJSON(const QJsonArray& jsonArray)
{
  std::for_each(jsonArray.constBegin(), jsonArray.constEnd(), &terrama2::core::DataManagerIntermediator::fromJSONValue);
}

void terrama2::core::DataManagerIntermediator::commit()
{
  TcpDispatcher::sendData(instanceName_, jsonPackage_);
  jsonPackage_ = QJsonArray();
}

void terrama2::core::DataManagerIntermediator::abort() noexcept
{
  jsonPackage_ = QJsonArray();
}

void terrama2::core::DataManagerIntermediator::processDataProvider(const QJsonObject& jsonObject)
{
  DataManager& dataManager = DataManager::getInstance();

  try
  {
    DataProvider dataProvider = DataProvider::FromJson(jsonObject);

    try
    {
      //if there is a DataProvider with this ID, update
      DataProvider localDataProvider = dataManager.findDataProvider(dataProvider.id()); Q_UNUSED(localDataProvider)
      dataManager.update(dataProvider);
    }
    catch (terrama2::InvalidArgumentException& /*e*/)
    {
      //if no DataProvider was found, create a new one
      dataManager.add(dataProvider);
    }
  }
  catch(terrama2::InvalidArgumentException& /*e*/)
  {
    //invalid DataProvider
    if(jsonObject.contains("id"))
    {
      uint64_t dataProviderId = jsonObject["id"].toInt();
      //if there is a DataProvider with this ID, remove
      DataProvider localDataProvider = dataManager.findDataProvider(dataProviderId); Q_UNUSED(localDataProvider)
      dataManager.removeDataProvider(dataProviderId);
    }
    else
    {
      //something wrong

      QString errMsg = QObject::tr("Invalid DataProvider received");
      TERRAMA2_LOG_ERROR() << errMsg;
    }
  }
}

void terrama2::core::DataManagerIntermediator::processDataSet(const QJsonObject& jsonObject)
{
  DataManager& dataManager = DataManager::getInstance();

  try
  {
    DataSet dataSet = DataSet::FromJson(jsonObject);

    try
    {
      //if there is a DataSet with this ID, update
      DataSet localDataSet = dataManager.findDataSet(dataSet.id()); Q_UNUSED(localDataSet)
      dataManager.update(dataSet);
    }
    catch (InvalidArgumentException& /*e*/)
    {
      //if no DataSet was found, create a new one
      dataManager.add(dataSet);
    }
  }
  catch(terrama2::InvalidArgumentException& /*e*/)
  {
    //invalid DataProvider
    if(jsonObject.contains("id"))
    {
      uint64_t dataSetId = jsonObject["id"].toInt();
      //if there is a DataSet with this ID, remove
      DataSet localDataSet = dataManager.findDataSet(dataSetId); Q_UNUSED(localDataSet)
      dataManager.removeDataSet(dataSetId);
    }
    else
    {
      //something wrong
      QString errMsg = QObject::tr("Invalid DataSet received");
      TERRAMA2_LOG_ERROR() << errMsg;
    }
  }
}

void terrama2::core::DataManagerIntermediator::fromJSONValue(const QJsonValue& jsonValue)
{
  try
  {
    QJsonObject object = jsonValue.toObject();
    QString coreClass = object["class"].toString();

    if(coreClass == "DataProvider")
      processDataProvider(object);
    else if(coreClass == "DataSet")
      processDataSet(object);
    else
    {
      // even known classes can be here, DataSetItem, Filter, etc
      // should not arrive here if not inside a DataSet or DataProvider

      TERRAMA2_LOG_ERROR() << QObject::tr("Unknown class received: %1").arg(coreClass);
      //TODO: throw here
    }
  }
  catch(terrama2::Exception& /*e*/)
  {
    //loggend on throw...
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknow error...");
  }
}
