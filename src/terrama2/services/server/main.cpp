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
#include "../../Exception.hpp"

int main(int argc, char* argv[])
{

  // check if the parameters was passed correctly
  if(argc < 2)
  {
    std::cerr << "Usage: terrama_server <project path>" << std::endl;

    return EXIT_FAILURE;
  }

  try
  {
    terrama2::core::initializeLogger("terrama2.log");
    TERRAMA2_LOG_INFO() << "Initializating TerraLib...";
    terrama2::core::initializeTerralib();

    TERRAMA2_LOG_INFO() << "Loading TerraMA2 Project...";
    QJsonDocument jdoc = terrama2::core::ReadJsonFile(argv[1]);
    QJsonObject project = jdoc.object();
    if(!terrama2::core::ApplicationController::getInstance().loadProject(project))
    {
      TERRAMA2_LOG_ERROR() << "Failure in TerraMA2 initialization: Project File is invalid or does not exist!";
      exit(TERRAMA2_PROJECT_LOAD_ERROR);
    }

    QCoreApplication app(argc, argv);

    terrama2::core::DataManager& dataManager = terrama2::core::DataManager::getInstance();
    dataManager.load();

    TERRAMA2_LOG_INFO() << "Starting services...";
    terrama2::core::ServiceManager serviceManager(&dataManager);
    QJsonArray services = project.value("services").toArray();
    serviceManager.addJsonServices(services);

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
