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
  \file terrama2/service/server/main.cpp

  \brief Main routine for TerraMA2 Server.

  \author Jano Simas
 */

//STL
#include <cstdlib>
#include <future>

//QT
#include <QCoreApplication>
#include <QJsonDocument>

#include "../../core/ApplicationController.hpp"
#include "../../core/ServiceManager.hpp"
#include "../../core/TcpListener.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/ErrorCodes.hpp"
#include "../../core/Logger.hpp"
#include "../../core/Utils.hpp"
#include "../../core/Project.hpp"

#include "../../ws/collector/core/Codes.hpp"

#include "../../Exception.hpp"

#include "soapWebService.h"

class QCloser
{
public:
  QCloser(){}
  ~QCloser()
  {
    QCoreApplication::exit();
  }
};

bool gSoapThread(int port)
{
  try
  {
    //Finixes the QApplication when the gsoap server is over
    QCloser qCloser;
    TERRAMA2_LOG_INFO() << "Starting Webservice...";

    WebService server;

    if(soap_valid_socket(server.master) || soap_valid_socket(server.bind(NULL, port, 100)))
    {
      TERRAMA2_LOG_INFO() << "Webservice Started, running on port " << port;

      for (;;)
      {
        if (!soap_valid_socket(server.accept()))
          break;

        if(server.serve() == terrama2::ws::collector::core::EXIT_REQUESTED)
          break;

        server.destroy();
        //process signal and QT events
        QCoreApplication::processEvents();
      }
    }
    else
    {
      server.soap_stream_fault(std::cerr);

      server.destroy();

      TERRAMA2_LOG_INFO() << "Webservice finished!";

      return false;
    }

    TERRAMA2_LOG_INFO() << "Shutdown Webservice...";
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    return false;
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    return false;
  }

  TERRAMA2_LOG_INFO() << "Webservice finished!";

  return true;
}


int main(int argc, char* argv[])
{
  const std::string helpMessage = "\nUsage: terrama2_server [options] port\n"
                                  "\toptions:\n"
                                  "\t\t-h, --help\t\t\t\t\tThis message.\n"
                                  "\t\t-p, --project projectFilePath projectFilePath2 ...\tLoad the project file.\n";

  // check if the parameters was passed correctly
  if(argc < 2 || std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)
   {
     std::cout << helpMessage << std::endl ;

     return EXIT_SUCCESS;
   }

  //TODO: remove this
  //check if a project was provided
  if(std::strcmp(argv[1], "-p") != 0 && std::strcmp(argv[1], "--project") != 0)
  {
    std::cout << "\nAlpha version, a project must be provided." << std::endl;
    std::cout << helpMessage << std::endl ;

    return EXIT_FAILURE;
  }

  //check webservice port
  int port = std::atoi(argv[argc-1]);
  if( port < 1024 || port > 49151)
  {
    std::cerr << "Inform a valid port (between 1024 and 49151) into the project file in order to run the collector application server." << std::endl;
    return EXIT_FAILURE;
  }

  try
  {

    TERRAMA2_LOG_INFO() << "Initializating TerraLib...";
    terrama2::core::initializeTerralib();

    //****************************************
    // Load project
    terrama2::core::Project project(argv[2]);
    terrama2::core::initializeLogger(project.logFile());

    TERRAMA2_LOG_INFO() << "Loading TerraMA2 Project...";
    if(!terrama2::core::ApplicationController::getInstance().loadProject(project))
    {
      TERRAMA2_LOG_ERROR() << "Failure in TerraMA2 initialization: Project File is invalid or does not exist!";
      exit(TERRAMA2_PROJECT_LOAD_ERROR);
    }
    //****************************************

    QCoreApplication app(argc, argv);

    auto gSoapThreadHandle = std::async(std::launch::async, gSoapThread, port);

    if(!(gSoapThreadHandle.wait_for(std::chrono::seconds(5)) == std::future_status::ready))
    {
      //Load TerraMA2 data
      terrama2::core::DataManager& dataManager = terrama2::core::DataManager::getInstance();
      dataManager.load();

      //Start service manager
      TERRAMA2_LOG_INFO() << "Starting services...";
      terrama2::core::ServiceManager serviceManager(&dataManager);
      //Start remote services
      serviceManager.addServices(project.serviceList());

      app.exec();
    }

    //unload and finaliza aplication
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
