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

//STL
#include <memory>
#include <iostream>

//Qt
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();
  terrama2::services::collector::core::Service service(dataManager);
  service.start();
  service.addToQueue(1);
  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(2000);
  app.exec();

  service.stop();
  return 0;
}
