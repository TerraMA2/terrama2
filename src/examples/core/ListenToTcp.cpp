
// TerraMA2

#include <terrama2/core/network/TcpManager.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/core/network/TcpSignals.hpp>
#include <terrama2/core/utility/JSonUtils.hpp>
#include <terrama2/core/utility/Utils.hpp>
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

class MockDataManager : public terrama2::core::DataManager
{
  public:
    MockDataManager() {}

    virtual ~MockDataManager() = default;
    MockDataManager(const MockDataManager& other) = default;
    MockDataManager(MockDataManager&& other) = default;
    MockDataManager& operator=(const MockDataManager& other) = default;
    MockDataManager& operator=(MockDataManager&& other) = default;

    virtual void addJSon(const QJsonObject& obj) override
    {
      QJsonDocument doc(obj);
      std::cout << QString(doc.toJson(QJsonDocument::Compact)).toStdString() << std::endl;
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
  dataSeries->semantics.name = "OCCURRENCE-wfp";
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

int main(int argc, char* argv[])
{

  terrama2::core::initializeTerraMA();
  QCoreApplication app(argc, argv);

  QJsonObject obj;
  QJsonArray providersArray;
  providersArray.push_back(terrama2::core::toJson(buildInputProvider()));
  obj.insert("DataProviders", providersArray);

  QJsonArray seriesArray;
  seriesArray.push_back(terrama2::core::toJson(buildInputDataSeries()));
  obj.insert("DataSeries", seriesArray);

  QJsonDocument doc(obj);

  std::shared_ptr<terrama2::core::DataManager> dataManager = std::make_shared<MockDataManager>();
  terrama2::core::TcpManager tcpManager(dataManager);
  tcpManager.listen(QHostAddress::Any, 30000);

  QByteArray bytearray;
  QDataStream out(&bytearray, QIODevice::WriteOnly);

  //Code commented for tests with TCP communication

  auto json = doc.toJson(QJsonDocument::Compact);
  out << static_cast<uint32_t>(0);
  out << static_cast<uint32_t>(terrama2::core::TcpSignals::ADD_DATA_SIGNAL);
  out << json;
  bytearray.remove(8, 4);//Remove QByteArray header
  out.device()->seek(0);
  out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

  QTcpSocket socket;
  socket.connectToHost("localhost", 30000);
  socket.write(bytearray);

  // QTimer timer;
  // QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  // timer.start(10000);
  app.exec();

  terrama2::core::finalizeTerraMA();

  return 0;
}
