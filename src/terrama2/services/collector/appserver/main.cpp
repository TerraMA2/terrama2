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
  \file terrama2/Exception.hpp

  \brief Base exception classes in TerraMA2.

  \author Gilberto Ribeiro de Queiroz
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

// Qt
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char* argv[])
{
  try
  {
    if(argc != 2)
    {
      //TODO: help message
      std::cout << "\n<< Help usage message >>\n" << std::endl;
      return SERVICE_PARAMETERS_ERROR;
    }
    int listeningPort = std::stoi(argv[1]);

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
    serviceManager.setServiceType("Collector");
    serviceManager.setListeningPort(listeningPort);

    {
      QCoreApplication app(argc, argv);

      auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();

      terrama2::core::TcpManager tcpManager(dataManager);
      if(!tcpManager.listen(QHostAddress::Any, serviceManager.listeningPort()))
      {
        std::cerr << QObject::tr("\nUnable to listen to port: ").toStdString() << serviceManager.listeningPort() << "\n" << std::endl;
        return TCP_SERVER_ERROR;
      }

      QObject::connect(&serviceManager, &terrama2::core::ServiceManager::listeningPortUpdated, &tcpManager, &terrama2::core::TcpManager::updateListeningPort);

      terrama2::services::collector::core::Service service(dataManager);

      QObject::connect(&serviceManager, &terrama2::core::ServiceManager::numberOfThreadsUpdated, &service, &terrama2::services::collector::core::Service::updateNumberOfThreads);
      QObject::connect(&serviceManager, &terrama2::core::ServiceManager::logConnectionInfoUpdated, &service, &terrama2::services::collector::core::Service::updateLoggerConnectionInfo);

      QObject::connect(&tcpManager, &terrama2::core::TcpManager::startProcess, &service, &terrama2::services::collector::core::Service::addToQueue);
      QObject::connect(&tcpManager, &terrama2::core::TcpManager::stopSignal, &service, &terrama2::services::collector::core::Service::stop);
      QObject::connect(&tcpManager, &terrama2::core::TcpManager::stopSignal, &app, &QCoreApplication::quit);

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
