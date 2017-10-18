

// TerraMA2

#include <terrama2/core/network/TcpManager.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/core/network/TcpSignals.hpp>
#include <terrama2/core/utility/JSonUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/Exception.hpp>

#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/core/JSonUtils.hpp>
#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>

#include <terrama2/services/collector/mock/MockCollectorLogger.hpp>


#include <terrama2/impl/Utils.hpp>
#include <terrama2/Config.hpp>

//STL
#include <iostream>

// Qt
#include <QUrl>
#include <QTimer>
#include <QObject>
#include <QCoreApplication>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QDataStream>

#include <examples/data/OccurrenceWFP.hpp>

// Boost
#include <boost/exception/get_error_info.hpp>
#include <boost/exception/diagnostic_information.hpp>

int main(int argc, char* argv[])
{
  try
  {
    terrama2::core::TerraMA2Init terramaRaii("example", 0);
    Q_UNUSED(terramaRaii);

    terrama2::core::registerFactories();

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


    auto dataProviderFile = terrama2::occurrencewfp::dataProviderFileOccWFP();
    dataManager->add(dataProviderFile);

    auto inputDataSeries = terrama2::occurrencewfp::dataSeriesOccWFP(dataProviderFile);
    dataManager->add(inputDataSeries);


    auto dataProviderPostGis = terrama2::occurrencewfp::dataProviderPostGisOccWFP();
    dataManager->add(dataProviderPostGis);

    auto outputDataSeries = terrama2::occurrencewfp::dataSeriesOccWFPPostGis(dataProviderPostGis);
    dataManager->add(outputDataSeries);


    std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();
    collector->id = 777;
    collector->projectId = 0;
    collector->serviceInstanceId = 1;

    collector->inputDataSeries = inputDataSeries->id;
    collector->outputDataSeries = outputDataSeries->id;

    collector->inputOutputMap.emplace(inputDataSeries->id, outputDataSeries->id);

    dataManager->add(collector);

    QJsonObject obj;

    QJsonArray providersArray;
    providersArray.push_back(terrama2::occurrencewfp::dataProviderFileOccWFPJson());
    providersArray.push_back(terrama2::occurrencewfp::dataProviderPostGisOccWFPJson());
    obj.insert("DataProviders", providersArray);

    QJsonArray seriesArray;
    seriesArray.push_back(terrama2::occurrencewfp::dataSeriesOccWFPJson(dataProviderFile));
    seriesArray.push_back(terrama2::occurrencewfp::dataSeriesOccWFPPostGisJson(dataProviderPostGis));
    obj.insert("DataSeries", seriesArray);

    QJsonArray collectorsArray;
    collectorsArray.push_back(terrama2::services::collector::core::toJson(collector));
    obj.insert("Collectors", collectorsArray);

    QJsonDocument doc(obj);


    terrama2::core::TcpManager tcpManager(dataManager, std::weak_ptr<terrama2::core::ProcessLogger>());
    tcpManager.listen(QHostAddress::Any, 30000);


    QByteArray bytearray;
    QDataStream out(&bytearray, QIODevice::WriteOnly);

    out << static_cast<uint32_t>(0);
    out << static_cast<uint32_t>(terrama2::core::TcpSignal::ADD_DATA_SIGNAL);
    out << doc.toJson(QJsonDocument::Compact);
    bytearray.remove(8, 4);//Remove QByteArray header
    out.device()->seek(0);
    out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

    QTcpSocket socket;
    socket.connectToHost("localhost", 30000);
    socket.write(bytearray);
    socket.waitForBytesWritten();

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(30000);
    app.exec();

    service.stopService();
  }
  catch(const boost::exception& e)
  {
    std::cout << boost::diagnostic_information(e) << std::endl;
  }
  catch(const std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "\n\nException...\n" << std::endl;
  }



  return 0;
}
