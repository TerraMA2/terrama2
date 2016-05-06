

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
#include <terrama2/Exception.hpp>

#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/core/JSonUtils.hpp>
#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>

#include <terrama2/impl/Utils.hpp>
#include <terrama2_config.hpp>

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

// Boost
#include <boost/exception/get_error_info.hpp>
#include <boost/exception/diagnostic_information.hpp>

terrama2::core::DataProviderPtr buildInputProvider()
{
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

  QString uri = QString("file://%1/fire_system").arg(TERRAMA2_DATA_DIR);
  dataProvider->id = 1;
  dataProvider->projectId = 1;
  dataProvider->name = "Provider";
  dataProvider->description = "Testing provider";
  dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider->uri = uri.toStdString();
  dataProvider->active = true;
  dataProvider->dataProviderType = "FILE";

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr buildInputDataSeries()
{
  // DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->id = 1;
  dataSeries->name = "DataProvider queimadas local";
  dataSeries->semantics.code = "OCCURRENCE-wfp";
  dataSeries->dataProviderId = 1;

  terrama2::core::DataSetOccurrence* dataSet = new terrama2::core::DataSetOccurrence();
  dataSeries->datasetList.emplace_back(dataSet);
  dataSet->id = 1;
  dataSet->active = true;
  dataSet->format.emplace("mask", "fires.csv");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("srid", "4326");

  return dataSeriesPtr;
}

terrama2::core::DataProviderPtr buildOutputProvider()
{
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

  QUrl postgisUri;
  postgisUri.setScheme("postgis");
  postgisUri.setHost("localhost");
  postgisUri.setPort(5432);
  postgisUri.setUserName("postgres");
  postgisUri.setPassword("postgres");
  postgisUri.setPath("/basedeteste");
  dataProvider->id = 2;
  dataProvider->projectId = 1;
  dataProvider->name = "Output provider";
  dataProvider->description = "Testing output provider";
  dataProvider->intent = terrama2::core::DataProvider::PROCESS_INTENT;
  dataProvider->uri = postgisUri.url().toStdString();
  dataProvider->active = true;
  dataProvider->dataProviderType = "POSTGIS";

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr buildOutputDataSeries()
{
  // DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
  outputDataSeries->id = 2;
  outputDataSeries->name = "DataProvider queimadas postgis";
  outputDataSeries->semantics.code = "OCCURRENCE-postgis";
  outputDataSeries->dataProviderId = 2;

  // DataSet information
  terrama2::core::DataSetOccurrence* outputDataSet = new terrama2::core::DataSetOccurrence();
  outputDataSet->active = true;
  outputDataSet->id = 2;
  outputDataSet->format.emplace("table_name", "queimadas");

  outputDataSeries->datasetList.emplace_back(outputDataSet);

  return outputDataSeriesPtr;
}

terrama2::services::collector::core::CollectorPtr buildCollector()
{
  terrama2::services::collector::core::Collector* collector = new terrama2::services::collector::core::Collector();
  terrama2::services::collector::core::CollectorPtr collectorPtr(collector);

  collector->id = 1;
  collector->projectId = 1;
  collector->serviceInstanceId = 1;
  collector->active = true;
  collector->inputDataSeries = 1;
  collector->outputDataSeries = 2;
  collector->inputOutputMap.emplace(1, 2);
  collector->schedule;
  collector->intersection;

  return collectorPtr;
}


int main(int argc, char* argv[])
{
  try
  {
    terrama2::core::initializeTerraMA();

    terrama2::core::registerFactories();

    QCoreApplication app(argc, argv);

    QJsonObject obj;

    QJsonArray providersArray;
    providersArray.push_back(terrama2::core::toJson(buildInputProvider()));
    providersArray.push_back(terrama2::core::toJson(buildOutputProvider()));
    obj.insert("DataProviders", providersArray);

    QJsonArray seriesArray;
    seriesArray.push_back(terrama2::core::toJson(buildInputDataSeries()));
    seriesArray.push_back(terrama2::core::toJson(buildOutputDataSeries()));
    obj.insert("DataSeries", seriesArray);

    QJsonArray collectorsArray;
    collectorsArray.push_back(terrama2::services::collector::core::toJson(buildCollector()));
    obj.insert("Collectors", collectorsArray);

    QJsonDocument doc(obj);

    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    std::map<std::string, std::string> connInfo { {"PG_HOST", "localhost"},
                                                  {"PG_PORT", "5432"},
                                                  {"PG_USER", "postgres"},
                                                  {"PG_PASSWORD", "postgres"},
                                                  {"PG_DB_NAME", "nodejs"},
                                                  {"PG_CONNECT_TIMEOUT", "4"},
                                                  {"PG_CLIENT_ENCODING", "UTF-8"}
                                                };
    serviceManager.setLogConnectionInfo(connInfo);

    auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();
    terrama2::core::TcpManager tcpManager(dataManager);
    tcpManager.listen(QHostAddress::Any, 30000);
    terrama2::services::collector::core::Service service(dataManager);
    service.start();

    QByteArray bytearray;
    QDataStream out(&bytearray, QIODevice::WriteOnly);

    out << static_cast<uint32_t>(0);
    out << terrama2::core::TcpSignals::ADD_DATA_SIGNAL;
    out << doc.toJson();
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

    service.stop();
  }
  catch(boost::exception& e)
  {
    std::cout << boost::diagnostic_information(e) << std::endl;
  }
  catch(std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  catch(...)
  {
    // TODO: o que fazer com uncaught exception
    std::cout << "\n\nException...\n" << std::endl;
  }

  terrama2::core::finalizeTerraMA();

  return 0;
}
