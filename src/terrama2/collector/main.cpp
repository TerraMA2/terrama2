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
  \file terrama2/collector/main.cpp

  \brief Main routine for TerraMA2 Collector Service.

  \author Jano Simas
 */

//STL
#include <cstdlib>

//QT
#include <QApplication>

#include "CollectorService.hpp"
#include "../core/TcpListener.hpp"
#include "../core/DataManager.hpp"
#include "../core/Logger.hpp"
#include "../core/Utils.hpp"
#include "../Exception.hpp"

int main(int argc, char* argv[])
{

  // check if the parameters was passed correctly
  if(argc < 2)
  {
    std::cerr << "Usage: collector_service <port number>" << std::endl;

    return EXIT_FAILURE;
  }

  int port = std::atoi(argv[1]);
  if(port < 1024 || port > 49151)
  {
    std::cerr << "Usage: collector_service <port number>" << std::endl;

    return EXIT_FAILURE;
  }


  try
  {
    terrama2::core::initializeLogger("terrama2.log");
    TERRAMA2_LOG_INFO() << "Initializating TerraLib...";
    terrama2::core::initializeTerralib();

    QApplication app(argc, argv);

    terrama2::core::DataManager::getInstance().load(true);

    TERRAMA2_LOG_INFO() << "Starting collector service...";
    terrama2::collector::CollectorService collectorService;
    collectorService.start();

    TERRAMA2_LOG_INFO() << "Listening to port: " + std::to_string(port);
    terrama2::core::TcpListener listener;
    QObject::connect(&listener, &terrama2::core::TcpListener::stopSignal, [&](){collectorService.stop(); app.exit();});
    listener.listen(QHostAddress::Any, port);

    app.exec();

    terrama2::core::DataManager::getInstance().unload();

    terrama2::core::finalizeTerralib();

    return EXIT_SUCCESS;
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_FATAL() << QObject::tr("Finishing program with error:\n") + *boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_FATAL() << QObject::tr("Finishing program with error:\n") + boost::diagnostic_information(e).c_str();
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_FATAL() << QObject::tr("Finishing program with error:\n") + e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_FATAL() << QObject::tr("Finishing program with unknown error.");
  }

  return EXIT_FAILURE;
}

