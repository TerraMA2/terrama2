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
  \file terrama2/services/collector/main.cpp

  \brief Collector service main.

  \author Jano Simas
 */

// TerraMA2
#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/core/network/TcpManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/ErrorCodes.hpp>

#include <boost/exception/diagnostic_information.hpp>

// STL
#include <memory>
#include <iostream>
#include <utility>
#include <tuple>

// Qt
#include <QCoreApplication>
#include <QTimer>

bool checkServiceType(const std::string& serviceType)
{
  if(serviceType == "collector"
     || serviceType == "analysis")
    return true;

  return false;
}

std::tuple<std::shared_ptr<terrama2::core::TcpManager>, std::shared_ptr<terrama2::core::DataManager>, std::shared_ptr<terrama2::core::Service> >
createCollector(terrama2::core::ServiceManager& serviceManager)
{
  auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();

  auto tcpManager = std::make_shared<terrama2::core::TcpManager>(dataManager);
  if(!tcpManager->listen(QHostAddress::Any, serviceManager.listeningPort()))
  {
    std::cerr << QObject::tr("\nUnable to listen to port: ").toStdString() << serviceManager.listeningPort() << "\n" << std::endl;

    exit(TCP_SERVER_ERROR);
  }

  QObject::connect(&serviceManager, &terrama2::core::ServiceManager::listeningPortUpdated, tcpManager.get(), &terrama2::core::TcpManager::updateListeningPort);

  auto service = std::make_shared<terrama2::services::collector::core::Service>(dataManager);

  QObject::connect(&serviceManager, &terrama2::core::ServiceManager::numberOfThreadsUpdated, service.get(), &terrama2::services::collector::core::Service::updateNumberOfThreads);
  QObject::connect(&serviceManager, &terrama2::core::ServiceManager::logConnectionInfoUpdated, service.get(), &terrama2::services::collector::core::Service::updateLoggerConnectionInfo);

  QObject::connect(tcpManager.get(), &terrama2::core::TcpManager::startProcess, service.get(), &terrama2::services::collector::core::Service::addToQueue);
  QObject::connect(tcpManager.get(), &terrama2::core::TcpManager::stopSignal, service.get(), &terrama2::services::collector::core::Service::stop);
  QObject::connect(tcpManager.get(), &terrama2::core::TcpManager::stopSignal, QCoreApplication::instance(), &QCoreApplication::quit);

  return std::make_tuple(tcpManager, dataManager, service);
}

std::tuple<std::shared_ptr<terrama2::core::TcpManager>, std::shared_ptr<terrama2::core::DataManager>, std::shared_ptr<terrama2::core::Service> >
createService( terrama2::core::ServiceManager& serviceManager, const std::string& serviceType)
{
  if(serviceType == "collector")
    return createCollector(serviceManager);

//FIXME: invalid service type return code
  exit(-1);
}

int main(int argc, char* argv[])
{
  try
  {
    if(argc != 3)
    {
      //TODO: help message
      std::cout << "\n<< Help usage message >>\n" << std::endl;
      return SERVICE_PARAMETERS_ERROR;
    }
    std::string serviceType(argv[1]);
    std::transform(serviceType.begin(), serviceType.end(), serviceType.begin(), ::tolower);

    if(!checkServiceType(serviceType))
      return -1;//FIXME: invalid service type return code

    int listeningPort = std::stoi(argv[2]);

    try
    {
      terrama2::core::initializeTerraMA();
      terrama2::core::registerFactories();
    }
    catch (...)
    {
      return TERRAMA2_INITIALIZATION_ERROR;
    }

    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    serviceManager.setServiceType(serviceType);
    serviceManager.setListeningPort(listeningPort);

    // service context
    // this is needed for calling the destructor of the service before finalizing terralib
    {
      QCoreApplication app(argc, argv);

      std::shared_ptr<terrama2::core::TcpManager> tcpManager;
       std::shared_ptr<terrama2::core::DataManager> dataManager;
      std::shared_ptr<terrama2::core::Service> service;
      std::tie(tcpManager, dataManager, service) = createService(serviceManager, serviceType);
      if(!service.get()
        || !dataManager.get()
        || !tcpManager.get())
        return -1;//FIXME: error creating service return code

      app.exec();
    }

    try
    {
      terrama2::core::finalizeTerraMA();

      //Service closed by load error
      if(!serviceManager.serviceLoaded())
        return SERVICE_LOAD_ERROR;
    }
    catch (...)
    {
      return TERRAMA2_FINALIZATION_ERROR;
    }
  }
  //TODO: should be using logger?
  catch(boost::exception& e)
  {
    std::cout << boost::diagnostic_information(e) << std::endl;
  }
  catch(std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  catch(...)
  {
    // TODO: o que fazer com uncaught exception
    std::cout << "\n\nException...\n" << std::endl;
  }

  return 0;
}
