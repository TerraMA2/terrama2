/*!
  \file terrama2/Exception.hpp

  \brief

  \author Bianca Maciel
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
#include <terrama2/core/utility/GeoUtils.hpp>


#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/mock/MockCollectorLogger.hpp>


#include <terrama2/impl/Utils.hpp>

#include <terrama2/core/data-access/DataRetriever.hpp>

#include <examples/data/DCPSerramarInpe.hpp>

// STL
#include <memory>
#include <iostream>

// Qt
#include <QCoreApplication>
#include <QTimer>
#include <QUrl>


void addInput(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
    //////////////////////////////////////////////
    //     input
    // DataProvider information
    //////////////////////////////////////////////


    auto dataProvider = terrama2::serramar::dataProviderSerramarInpe();
    dataManager->add(dataProvider);


    // DataSeries information
    auto dataSeries = terrama2::serramar::dataSeriesDcpSerramar(dataProvider);
    dataManager->add(dataSeries);
}


void addOutput(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManager)
{
    ///////////////////////////////////////////////
    //     output
    ///////////////////////////////////////////////


    // DataProvider information
    auto dataProvider = terrama2::serramar::dataProviderPostGisDCP();
    dataManager->add(dataProvider);


    // DataSeries information
    auto outputDataSeries = terrama2::serramar::dataSeriesDcpSerramarPostGis(dataProvider);
    dataManager->add(outputDataSeries);

}

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




      addInput(dataManager);
      addOutput(dataManager);



      std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();
      collector->id = 777;
      collector->projectId = 0;
      collector->serviceInstanceId = 1;

      collector->inputDataSeries = 1;
      collector->outputDataSeries = 2;

      // picinguaba input_id: 1 output_id: 6
      collector->inputOutputMap.emplace(1, 6);


      // itanhaem input_id: 2 output_id: 4
      collector->inputOutputMap.emplace(2, 7);


      // ubatuba input_id: 3 output_id: 8
      collector->inputOutputMap.emplace(3, 8);


      // itutinga input_id: 4 output_id: 9
      collector->inputOutputMap.emplace(4, 9);


      // cunha input_id: 2 output_id: 10
      collector->inputOutputMap.emplace(5, 10);





      dataManager->add(collector);

      service.addToQueue(collector, terrama2::core::TimeUtils::nowUTC());

      QTimer timer;
      QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
      timer.start(10000);
      app.exec();

    }
    return 0;
}
