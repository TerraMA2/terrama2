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

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/DataStoragerFactory.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

#include <terrama2/impl/DataAccessorDcpInpe.hpp>
#include <terrama2/impl/DataAccessorDcpPostGIS.hpp>
#include <terrama2/impl/DataAccessorGeoTiff.hpp>
#include <terrama2/impl/DataAccessorOccurrenceMvf.hpp>
#include <terrama2/impl/DataAccessorOccurrencePostGis.hpp>
#include <terrama2/impl/DataAccessorStaticDataOGR.hpp>

#include <terrama2/impl/DataStoragerPostGis.hpp>

#include <terrama2/core/data-access/DataRetriever.hpp>

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
    terrama2::core::initializeTerraMA();

    terrama2::core::DataAccessorFactory::getInstance().add("DCP-inpe", terrama2::core::DataAccessorDcpInpe::make);
    terrama2::core::DataAccessorFactory::getInstance().add("DCP-postgis", terrama2::core::DataAccessorDcpPostGIS::make);
    terrama2::core::DataAccessorFactory::getInstance().add("GRID-geotiff", terrama2::core::DataAccessorGeoTiff::make);
    terrama2::core::DataAccessorFactory::getInstance().add("OCCURRENCE-mvf", terrama2::core::DataAccessorOccurrenceMvf::make);
    terrama2::core::DataAccessorFactory::getInstance().add("OCCURRENCE-postgis", terrama2::core::DataAccessorOccurrencePostGis::make);
    terrama2::core::DataAccessorFactory::getInstance().add("STATIC_DATA-ogr", terrama2::core::DataAccessorStaticDataOGR::make);

    terrama2::core::DataStoragerFactory::getInstance().add("POSTGIS", terrama2::core::DataStoragerPostGis::make);

    QCoreApplication app(argc, argv);
    terrama2::core::TcpManager tcpManager;
    auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();
    tcpManager.listen(dataManager, QHostAddress::Any, 30000);

    terrama2::services::collector::core::Service service(dataManager);
    QObject::connect(&tcpManager, &terrama2::core::TcpManager::startProcess, &service, &terrama2::services::collector::core::Service::addToQueue);
    QObject::connect(&tcpManager, &terrama2::core::TcpManager::stopSignal, &service, &terrama2::services::collector::core::Service::stop);
    QObject::connect(&tcpManager, &terrama2::core::TcpManager::stopSignal, &app, &QCoreApplication::quit);
    service.start();

    app.exec();

    terrama2::core::finalizeTerraMA();
  }
  catch(...)
  {
    // TODO: o que fazer com uncaught exception
    std::cout << "\n\nException...\n" << std::endl;
  }

  return 0;
}
