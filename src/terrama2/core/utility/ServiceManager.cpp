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
  \file terrama2/core/ServiceManager.cpp

  \brief Manages the instance of remote services.

  \author Jano Simas
*/

#include "ServiceManager.hpp"
#include "../utility/Logger.hpp"
#include "../data-model/DataManager.hpp"

//QT
#include <QProcess>

//STD
#include <cassert>

terrama2::core::ServiceManager::ServiceManager(terrama2::core::DataManager* dataManager)
  : dataManager_(dataManager)
{
}

void terrama2::core::ServiceManager::addDataStruct(const ServiceData& serviceData, TcpDispatcherPtr dispatcher)
{
  DataManagerIntermediatorPtr dataManagerIntermediator = std::make_shared<DataManagerIntermediator>(dispatcher);
  ServiceDataStruct data { serviceData,
                           dispatcher,
                           dataManagerIntermediator };

  //service running
  //save remote service information
  serviceDataMap_.emplace(serviceData.name, data);

  QObject::connect(dataManager_,
                   &terrama2::core::DataManager::dataSeriesAdded,
                   [dataManagerIntermediator](const terrama2::core::DataSeries& dataseries){
                                                                                              *dataManagerIntermediator << dataseries;
                                                                                              dataManagerIntermediator->commit();
                                                                                            });

  QObject::connect(dataManager_,
                   &terrama2::core::DataManager::dataProviderAdded,
                   [dataManagerIntermediator](const terrama2::core::DataProvider& dataprovider){
                                                                                                 *dataManagerIntermediator << dataprovider;
                                                                                                 dataManagerIntermediator->commit();
                                                                                               });
}

void terrama2::core::ServiceManager::addService(const ServiceData& serviceData)
{
  TcpDispatcherPtr dispatcher  = std::make_shared<TcpDispatcher>(serviceData);
  if(dispatcher->pingService())
  {
    addDataStruct(serviceData, dispatcher);
    return;
  }

  QStringList commandArgs;
  //user and host
  commandArgs.append(QString::fromStdString(serviceData.user+"@"+serviceData.host));

  //TODO: remove LD_LIBRARY_PATH
  commandArgs.append("LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/jsimas/MyDevel/dpi/libs/lib:/home/jsimas/MyDevel/dpi/libs/terralib5/lib:/home/jsimas/MyDevel/dpi/libs/gdal2/lib:/home/jsimas/MyDevel/dpi/libs");

  switch (serviceData.type) {
    case ServiceData::COLLECTOR:
      TERRAMA2_LOG_INFO() << "Starting Collector Service at "+serviceData.host+":"+std::to_string(serviceData.servicePort);
      //prepare collector command
      commandArgs.append(QString::fromStdString(serviceData.pathToBinary)+"/collector_service");
      commandArgs.append(QString::number(serviceData.servicePort));
      break;
    default:
      //TODO: other services
      assert(0);
      break;
  }

  // ssh stands locked unless we redirect the output and move process to background
  commandArgs.append("> /dev/null");
  commandArgs.append("2> /dev/null");
  commandArgs.append("&");

  QProcess remoteService;
  remoteService.setProgram("ssh");
  remoteService.setArguments(commandArgs);
  remoteService.open();

  //waits for ssh to finish, the remote process will continue.
  if(remoteService.waitForFinished())
  {
    //check for error from ssh
    if(remoteService.error() != QProcess::UnknownError ||
       remoteService.exitStatus() != QProcess::NormalExit)
    {
      //TODO: erro!!!
    }

    //check if remote process is running and listening
    //TODO: ping

    //save remote service information
    addDataStruct(serviceData, dispatcher);
  }
  else
  {
    //TODO:throw
    //throw
  }
}

void terrama2::core::ServiceManager::addServices(const std::map<std::string, terrama2::core::ServiceData>& services)
{
  for(const auto& service : services)
    addService(service.second);
}

void terrama2::core::ServiceManager::addJsonServices(const QJsonArray& servicesArray)
{
  for(const QJsonValue& jsonValue : servicesArray)
  {
    QJsonObject object = jsonValue.toObject();

    ServiceData serviceData{
                              object.value("name").toString().toStdString(),
                              object.value("host").toString("localhost").toStdString(),
                              object.value("user").toString().toStdString(),
                              object.value("pathToBinary").toString().toStdString(),
                              static_cast<ServiceData::ServiceType>(object.value("serviceType").toInt()),
                              object.value("servicePort").toInt()
                            };

    addService(serviceData);
  }
}

void terrama2::core::ServiceManager::removeService(const std::string& instanceName)
{
  TcpDispatcherPtr localTcpDispatcher = tcpDispatcher(instanceName);
  localTcpDispatcher->stopService();

  if(!localTcpDispatcher->pingService())
    serviceDataMap_.erase(instanceName);
  else
  {
    //TODO: throw;
    throw;
  }
}

terrama2::core::DataManagerIntermediatorPtr terrama2::core::ServiceManager::intermediator(const std::string& instanceName) const
{
  return dataStruct(instanceName).dataManagerIntermediator;
}

terrama2::core::TcpDispatcherPtr terrama2::core::ServiceManager::tcpDispatcher(const std::string& instanceName) const
{
  return dataStruct(instanceName).tcpDispatcher;
}

terrama2::core::ServiceManager::ServiceDataStruct terrama2::core::ServiceManager::dataStruct(const std::string& instance) const
{
  std::map<std::string, ServiceDataStruct>::const_iterator it = serviceDataMap_.find(instance);
  if(it == serviceDataMap_.cend())
  {
    //TODO: throw
    throw;
  }
  else
  {
    return it->second;
  }
}
