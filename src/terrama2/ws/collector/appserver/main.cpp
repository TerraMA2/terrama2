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
#include "../core/Codes.hpp"
#include "../Exception.hpp"
#include "../../../core/ApplicationController.hpp"
#include "../../../core/DataManager.hpp"
#include "../../../core/Utils.hpp"
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
    qDebug() << "Starting Webservice...";

    qDebug() << "Initializating TerraLib...";

    WebService server;

    if(soap_valid_socket(server.master) || soap_valid_socket(server.bind(NULL, port, 100)))
    {
      qDebug() << "Webservice Started, running on port " << port;

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

      qDebug() << "Webservice finished!";

      return false;
    }

    qDebug() << "Shutdown Webservice...";

    server.destroy();

  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    return false;
  }
  catch(std::exception& e)
  {
    qDebug() << e.what();
    return false;
  }

  qDebug() << "Webservice finished!";

  return true;
}

void initializeTerralib(char** argv)
{
  try
  {
    // VINICIUS: replace the initialize terralib and terrama2 for the method in terrama2:core (when implemented)
    // Initialize the Terralib support
    TerraLib::getInstance().initialize();

    te::plugin::PluginInfo* info;
    std::string plugins_path = te::common::FindInTerraLibPath("share/terralib/plugins");
    info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.pgis.teplg");
    te::plugin::PluginManager::getInstance().add(info);

    info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.gdal.teplg");
    te::plugin::PluginManager::getInstance().add(info);

    info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.ogr.teplg");
    te::plugin::PluginManager::getInstance().add(info);

    te::plugin::PluginManager::getInstance().loadAll();

    qDebug() << "Loading TerraMA2 Project...";

    if(!terrama2::core::ApplicationController::getInstance().loadProject(argv[1]))
    {
      qDebug() << "TerraMA2 Project File is invalid or don't exist!";
      exit(TERRAMA2_PROJECT_LOAD_ERROR);
    }

    terrama2::core::DataManager::getInstance().load();
  }
  catch(boost::exception& e)
  {
    qDebug() << "1\t" << "initializeTerralib: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    exit(TERRALIB_LOAD_ERROR);
  }
  catch(std::exception& e)
  {
    qDebug() << "1\t" << "initializeTerralib: " << e.what();
    exit(TERRALIB_LOAD_ERROR);
  }
  catch(...)
  {
    qDebug() << "1\t" << "initializeTerralib unkown error";
    exit(TERRALIB_LOAD_ERROR);
  }
}

void finalizeTerralib()
{
  try
  {
    // VINICIUS: replace the finalize terralib and terrama2 for the method in terrama2:core (when implemented)
    TerraLib::getInstance().finalize();

    terrama2::core::DataManager::getInstance().unload();

    terrama2::core::ApplicationController::getInstance().getDataSource()->close();
  }
  catch(boost::exception& e)
  {
    qDebug() << "3\t" << "finalizeTerralib: " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    exit(TERRALIB_UNLOAD_ERROR);
  }
  catch(std::exception& e)
  {
    qDebug() << "3\t" << "finalizeTerralib: " << e.what();
    exit(TERRALIB_UNLOAD_ERROR);
  }
  catch(...)
  {
    qDebug() << "3\t" << "finalizeTerralib";
    exit(TERRALIB_UNLOAD_ERROR);
  }
}

int main(int argc, char* argv[])
{
  // check if the parameters was passed correctly
  if(argc < 2)
  {
    std::cerr << "Usage: terrama2_mod_ws_collector_appserver <project_File>" << std::endl;

    return EXIT_FAILURE;
  }

  int port = 0;

  try
  {
    QJsonDocument jdoc = terrama2::core::ReadJsonFile(argv[1]);

    QJsonObject project = jdoc.object();

    if(project.contains("collection"))
    {
      QJsonObject collectionConfig = project["collection"].toObject();
      QJsonObject collectionWebserviceConfig = collectionConfig["webservice"].toObject();
      port = collectionWebserviceConfig["portNumber"].toString().toInt();

      if( port < 1024 || port > 49151)
      {
        std::cerr << "Inform a valid port (between 1024 and 49151) and into the project file in order to run the collector application server." << std::endl;
        return EXIT_FAILURE;
      }
    }
    else
    {
      std::cerr << "Inform a valid port (between 1024 and 49151) and into the project file in order to run the collector application server." << std::endl;
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
    std::cerr << "Unknow error at reading port from project!" << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    initializeTerralib(argv);

    QApplication app(argc, argv);
    auto gSoapThreadHandle = std::async(std::launch::async, gSoapThread, port);

    terrama2::collector::CollectorService collectorService;
    collectorService.start();

    app.exec();

    finalizeTerralib();

    return gSoapThreadHandle.get() ? EXIT_SUCCESS : EXIT_FAILURE;
  }
  catch(boost::exception& e)
  {
    qDebug() << "2\t" << "gSoapServer " << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    exit(COLLECTOR_SERVICE_STAR_ERROR);
  }
  catch(std::exception& e)
  {
    qDebug() << "2\t" << "gSoapServer " << e.what();
    exit(COLLECTOR_SERVICE_STAR_ERROR);
  }
  catch(...)
  {
    qDebug() << "2\t" << "gSoapServer unkown error";
    exit(COLLECTOR_SERVICE_STAR_ERROR);
  }

  return EXIT_FAILURE;
}
