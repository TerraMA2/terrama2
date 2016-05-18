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

  \brief Main file for analysis service.

  \author Paulo R. M. Oliveira
 */

// TerraMA2
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/PythonInterpreter.hpp>
#include <terrama2/core/network/TcpManager.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/Logger.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/ErrorCodes.hpp>

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

    terrama2::core::initializeTerraMA();

    terrama2::core::registerFactories();

    terrama2::services::analysis::core::initInterpreter();

    QCoreApplication app(argc, argv);
    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    serviceManager.setServiceType("Analysis");
    serviceManager.setListeningPort(listeningPort);

    auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();
    terrama2::core::TcpManager tcpManager(dataManager);
    tcpManager.listen(QHostAddress::Any, 30001);

    terrama2::services::analysis::core::Service service(dataManager);

    QObject::connect(&serviceManager, &terrama2::core::ServiceManager::numberOfThreadsUpdated, &service, &terrama2::services::analysis::core::Service::updateNumberOfThreads);
    QObject::connect(&serviceManager, &terrama2::core::ServiceManager::logConnectionInfoUpdated, &service, &terrama2::services::analysis::core::Service::updateLoggerConnectionInfo);

    QObject::connect(&tcpManager, &terrama2::core::TcpManager::startProcess, &service, &terrama2::services::analysis::core::Service::addToQueue);
    QObject::connect(&tcpManager, &terrama2::core::TcpManager::stopSignal, &service, &terrama2::services::analysis::core::Service::stop);
    QObject::connect(&tcpManager, &terrama2::core::TcpManager::stopSignal, &app, &QCoreApplication::quit);
    service.start();

    app.exec();

    terrama2::core::finalizeTerraMA();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unhandled exception.");
  }

  return 0;
}
