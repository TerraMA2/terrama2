
// TerraMA2

#include <terrama2/core/network/TcpManager.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/core/network/TcpSignals.hpp>
#include <terrama2/core/utility/JSonUtils.hpp>

#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/core/JSonUtils.hpp>

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

class MockDataManager : public terrama2::core::DataManager
{
  public:
    MockDataManager() {}

    virtual ~MockDataManager() = default;
    MockDataManager(const MockDataManager& other) = default;
    MockDataManager(MockDataManager&& other) = default;
    MockDataManager& operator=(const MockDataManager& other) = default;
    MockDataManager& operator=(MockDataManager&& other) = default;

    virtual void addFromJSON(const QJsonValue& jsonValue) override
    {
      QJsonDocument doc(jsonValue.toObject());
      std::cout << QString(doc.toJson()).toStdString() << std::endl;
    }

};

terrama2::core::DataProviderPtr buildInputProvider()
{
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

  QString uri = QString("file://%1/PCD_serrmar_INPE").arg(TERRAMA2_DATA_DIR);
  dataProvider->id = 1;
  dataProvider->projectId = 1;
  dataProvider->name = "Provider";
  dataProvider->description = "Testing provider";
  dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider->uri = uri.toStdString();
  dataProvider->active = true;

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr buildInputDataSeries()
{
  // DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->id = 1;
  dataSeries->name = "DataProvider queimadas local";
  dataSeries->semantics.name = "OCCURRENCE-mvf";
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

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr buildOutputDataSeries()
{
  // DataSeries information
  // DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
  outputDataSeries->id = 2;
  outputDataSeries->name = "DataProvider queimadas postgis";
  outputDataSeries->semantics.name = "OCCURRENCE-postgis";
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
  QCoreApplication app(argc, argv);

  QJsonArray array;
  array.push_back(terrama2::core::toJson(buildInputProvider()));
  array.push_back(terrama2::core::toJson(buildOutputProvider()));
  array.push_back(terrama2::core::toJson(buildInputDataSeries()));
  array.push_back(terrama2::core::toJson(buildOutputDataSeries()));
  array.push_back(terrama2::services::collector::core::toJson(buildCollector()));

  QJsonDocument doc(array);

  terrama2::core::TcpManager tcpManager;
  std::shared_ptr<terrama2::core::DataManager> dataManager = std::make_shared<MockDataManager>();
  tcpManager.listen(dataManager, QHostAddress::Any, 30000);


  QByteArray bytearray;
  QDataStream out(&bytearray, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_2);

  out << static_cast<uint32_t>(0);
  out << terrama2::core::TcpSignals::DATA_SIGNAL;
  out << doc.toJson();
  out.device()->seek(0);
  out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

  QTcpSocket socket;
  socket.connectToHost("localhost", 30000);
  socket.write(bytearray);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(5000);
  app.exec();

  return 0;
}
