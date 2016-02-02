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
  \file terrama2/ws/collector/appserver/main.cpp

  \brief Main routine for TerraMA2 Collector Web Service.

  \author Vinicius Campanha
 */

// STL
#include <iostream>
#include <future>

// Qt
#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

// Terralib
#include "terralib/common/PlatformUtils.h"
#include "terralib/common.h"
#include "terralib/plugin.h"

// TerraMA2
#include "soapWebService.h"
#include "../Exception.hpp"
#include "../Exception.hpp"
#include "../core/Codes.hpp"
#include "../../../core/ApplicationController.hpp"
#include "../../../core/DataManager.hpp"
#include "../../../core/Utils.hpp"
#include "../../../core/Logger.hpp"
#include "../../../core/TcpListener.hpp"
#include "../../../collector/CollectorService.hpp"

const int TERRALIB_LOAD_ERROR = 101;
const int COLLECTOR_SERVICE_STAR_ERROR = 102;
const int TERRALIB_UNLOAD_ERROR = 103;
const int TERRAMA2_PROJECT_LOAD_ERROR = 104;


class QCloser
{
public:
  QCloser(){}
  ~QCloser()
  {
    QApplication::exit();
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

    //    server.soap_force_close_socket();
    //    server.destroy();
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
  // check if the parameters was passed correctly
  if(argc < 2)
  {
    std::cerr << "Usage: terrama2_mod_ws_collector_appserver <project_File>" << std::endl;

    return EXIT_FAILURE;
  }

  std::string logPath = "";
  int port = 0;

  QJsonDocument jdoc = terrama2::core::ReadJsonFile(argv[1]);
  QJsonObject project = jdoc.object();

  try
  {
    if(project.contains("collector_web_service"))
    {
      QJsonObject collectionConfig = project["collector_web_service"].toObject();
      port = collectionConfig["port"].toInt();

      if( port < 1024 || port > 49151)
      {
        std::cerr << "Inform a valid port (between 1024 and 49151) into the project file in order to run the collector application server." << std::endl;
        return EXIT_FAILURE;
      }

      logPath = collectionConfig["log_file"].toString().toStdString();
      if (logPath.empty())
      {
        std::cerr << "Invalid log file destination in project file.";
        return EXIT_FAILURE;
      }
    }
    else
    {
      std::cerr << "Inform a valid port (between 1024 and 49151) into the project file in order to run the collector application server." << std::endl;
      return EXIT_FAILURE;
    }
  }
  catch(terrama2::Exception &e)
  {
    std::cerr << "Error at reading port from project: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str() << std::endl;;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cerr << "Unknown error at reading port from project!" << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    terrama2::core::initializeLogger(logPath);
    TERRAMA2_LOG_INFO() << "Initializating TerraLib...";
    terrama2::core::initializeTerralib();

    TERRAMA2_LOG_INFO() << "Loading TerraMA2 Project...";
    if(!terrama2::core::ApplicationController::getInstance().loadProject(project))
    {
      TERRAMA2_LOG_ERROR() << "Failure in TerraMA2 initialization: Check if the Project File exists and has valid information!";
      exit(TERRAMA2_PROJECT_LOAD_ERROR);
    }

    terrama2::core::DataManager::getInstance().load();

    QApplication app(argc, argv);
    auto gSoapThreadHandle = std::async(std::launch::async, gSoapThread, port);

    if(!(gSoapThreadHandle.wait_for(std::chrono::seconds(5)) == std::future_status::ready))
    {
      terrama2::collector::CollectorService collectorService;
      collectorService.start();

      if(!(gSoapThreadHandle.wait_for(std::chrono::seconds(5)) == std::future_status::ready))
        app.exec();
    }

    terrama2::core::DataManager::getInstance().unload();

    terrama2::core::finalizeTerralib();

    return gSoapThreadHandle.get() ? EXIT_SUCCESS : EXIT_FAILURE;
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "2\t" << "gSoapServer " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    exit(COLLECTOR_SERVICE_STAR_ERROR);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << "2\t" << "gSoapServer " << e.what();
    exit(COLLECTOR_SERVICE_STAR_ERROR);
  }
  catch(...)
  {
    TERRAMA2_LOG_FATAL() << "2\t" << "gSoapServer unknown error";
    exit(COLLECTOR_SERVICE_STAR_ERROR);
  }

  return EXIT_FAILURE;
}
