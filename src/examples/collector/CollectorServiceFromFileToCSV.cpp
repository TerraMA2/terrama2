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
  \file src/examples/collector/CollectorServiceFromFileToCSV.cpp

  \brief Example of a collector from a file to a csv

  \author Jano Simas
 */

// TerraMA2
#include <terrama2/core/Shared.hpp>

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/DataStoragerFactory.hpp>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/analysis/core/Service.hpp>

#include <terrama2/services/collector/mock/MockCollectorLogger.hpp>

#include <terrama2/impl/Utils.hpp>

#include <terrama2/core/data-access/DataRetriever.hpp>

#include <extra/data/OccurrenceWFP.hpp>

// STL
#include <memory>
#include <iostream>

// Qt
#include <QCoreApplication>
#include <QTimer>
#include <QUrl>


int main(int argc, char* argv[])
{

    terrama2::core::TerraMA2Init terramaRaii("example", 0);
    Q_UNUSED(terramaRaii);

    terrama2::core::registerFactories();
    {
      QCoreApplication app(argc, argv);

      auto& serviceManager = terrama2::core::ServiceManager::getInstance();

      auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();

      auto loggerCopy = std::make_shared<terrama2::core::MockCollectorLogger>();

      EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));
      EXPECT_CALL(*loggerCopy, isValid()).WillRepeatedly(::testing::Return(true));

      auto logger = std::make_shared<terrama2::core::MockCollectorLogger>();

      EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
      EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
      EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
      EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));
      EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));

      terrama2::services::collector::core::Service service(dataManager);
      serviceManager.setInstanceId(1);
      serviceManager.setLogger(logger);
      serviceManager.setLogConnectionInfo(te::core::URI(""));

      service.setLogger(logger);
      service.start();


      auto dataProvider = terrama2::occurrencewfp::dataProviderFileOccWFP();
      dataManager->add(dataProvider);

      auto inputDataSeries = terrama2::occurrencewfp::dataSeriesOccWFP(dataProvider);
      dataManager->add(inputDataSeries);


      auto outputDataSeries = terrama2::occurrencewfp::dataSeriesOccWFPFile(dataProvider);
      dataManager->add(outputDataSeries);


      std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();

      collector->id = 777;
      collector->projectId = 0;
      collector->serviceInstanceId = 1;

      collector->inputDataSeries = inputDataSeries->id;
      collector->outputDataSeries = outputDataSeries->id;
      collector->inputOutputMap.emplace(inputDataSeries->id, outputDataSeries->id);

      dataManager->add(collector);

      service.addToQueue(collector->id, terrama2::core::TimeUtils::nowUTC());

      QTimer timer;
      QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
      timer.start(10000);
      app.exec();

    }

  return 0;
}
