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
  \file terrama2/integration_test/ws/collector/main.cpp

  \brief Main class for tests in the WebService class.

  \author Vinicius Campanha
*/

// Qt
#include <QObject>

// TerraMA2 Test
#include "TestClient.hpp"
#include "Utils.hpp"

int main(int argc, char **argv)
{

// Define SERVER if you are running a server in 1989 port
//#define SERVER server

#ifndef SERVER

  QObject *parent = new QObject();

  QString program = "./terrama2_mod_ws_collector_appserver";
  QStringList arguments;
  arguments.append("1989");
  arguments.append("../../codebase/src/unittest/ws/collector/data/project.json");


  QProcess *service = new QProcess(parent);
  service->setWorkingDirectory(QDir::currentPath());
  service->start(program, arguments);

  if(!service->waitForStarted(10000))
  {
    std::cerr << "Error to initialize Web Service: " << service->errorString().toStdString().c_str() << std::endl;
    return service->exitCode();
  }

  std::cerr << "Web Service started!" << std::endl;
#endif

  InitializeTerraMA2();

  TestClient testClient;
  int ret = QTest::qExec(&testClient, argc, argv);

  FinalizeTerraMA2();

#ifndef SERVER
  service->close();

  delete service;
  delete parent;
#endif

  return ret;
}
