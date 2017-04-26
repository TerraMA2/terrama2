#include "AlertLoggerMock.hpp"
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/alert/core/Shared.hpp>
#include <terrama2/services/alert/core/DataManager.hpp>
#include <terrama2/services/alert/core/Alert.hpp>
#include <terrama2/services/alert/core/Report.hpp>
#include <terrama2/services/alert/core/Service.hpp>
#include <terrama2/services/alert/core/AlertExecutor.hpp>
#include <terrama2/services/alert/impl/Utils.hpp>


#include <iostream>

//QT
#include <QUrl>
#include <QtGui>
#include <QTimer>

using ::testing::_;

using namespace terrama2::services::alert::core;


terrama2::core::DataProviderPtr inputDataProvider()
{
  QUrl uri;
  uri.setScheme("postgis");
  uri.setHost(QString::fromStdString(TERRAMA2_DATABASE_HOST));
  uri.setPort(std::stoi(TERRAMA2_DATABASE_PORT));
  uri.setUserName(QString::fromStdString(TERRAMA2_DATABASE_USERNAME));
  uri.setPassword(QString::fromStdString(TERRAMA2_DATABASE_PASSWORD));
  uri.setPath(QString::fromStdString("/"+TERRAMA2_DATABASE_DBNAME));

  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->name = "PostGIS provider";
  dataProvider->uri = uri.url().toStdString();
  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "POSTGIS";
  dataProvider->active = true;
  dataProvider->id = 1;

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr inputDataSeries()
{

  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->name = "Fire count";
  dataSeries->semantics = semanticsManager.getSemantics("ANALYSIS_MONITORED_OBJECT-postgis");
  dataSeries->id = 1;
  dataSeries->dataProviderId = 1;

  //DataSet information
  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("table_name", "analise_result");
  dataSet->format.emplace("timestamp_property", "execution_date");

  dataSeries->datasetList.emplace_back(dataSet);

  return dataSeriesPtr;
}

terrama2::core::DataProviderPtr additionalDataProvider()
{
  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->name = "Shapefiles provider";
  dataProvider->uri = "file://"+ TERRAMA2_DATA_DIR+"/shapefile/";
  dataProvider->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;
  dataProvider->id = 2;

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr additionalDataSeries()
{

  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->name = "States 2010";
  dataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");
  dataSeries->id = 2;
  dataSeries->dataProviderId = 2;

  //DataSet information
  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->id = 2;
  dataSet->active = true;
  dataSet->format.emplace("mask", "estados_2010.shp");
  dataSet->format.emplace("srid", "4326");

  dataSeries->datasetList.emplace_back(dataSet);

  return dataSeriesPtr;
}

terrama2::services::alert::core::AlertPtr newAlert()
{
  auto alert = new terrama2::services::alert::core::Alert();
  terrama2::services::alert::core::AlertPtr alertPtr(alert);

  alert->id = 1;
  alert->projectId = 1;
  alert->riskAttribute = "count";
  alert->dataSeriesId = 1;
  alert->active = true;
  alert->name = "Example alert";
  alert->serviceInstanceId = 1;

  terrama2::core::Risk risk;
  risk.name = "Fire occurrence count";

  terrama2::core::RiskLevel level1;
  level1.level = 0;
  level1.value = 0;
  level1.name = "low";
  risk.riskLevels.push_back(level1);

  terrama2::core::RiskLevel level2;
  level2.level = 1;
  level2.value = 10;
  level2.name = "medium";
  risk.riskLevels.push_back(level2);

  terrama2::core::RiskLevel level3;
  level3.level = 2;
  level3.value = 15;
  level3.name = "high";
  risk.riskLevels.push_back(level3);

  alert->risk = risk;

  terrama2::services::alert::core::AdditionalData additionalData;
  additionalData.dataSeriesId = 2;
  additionalData.dataSetId = 2;
  additionalData.referrerAttribute = "id";
  additionalData.referredAttribute = "id";
  additionalData.attributes.push_back("nome");

 alert->additionalDataVector.push_back(additionalData);

  std::unordered_map<std::string, std::string> reportMetadata;

  reportMetadata[terrama2::services::alert::core::ReportTags::TITLE] = "NUMERIC RISK EXAMPLE REPORT";
  reportMetadata[terrama2::services::alert::core::ReportTags::ABSTRACT] = "NumericRisk example.";
  reportMetadata[terrama2::services::alert::core::ReportTags::AUTHOR] = "TerraMA2";
  reportMetadata[terrama2::services::alert::core::ReportTags::CONTACT] = "TerraMA2 developers.";
  reportMetadata[terrama2::services::alert::core::ReportTags::COPYRIGHT] = "copyright information...";
  reportMetadata[terrama2::services::alert::core::ReportTags::DESCRIPTION] = "Example generated report...";
  reportMetadata["document_save_path"] = "/" + TERRAMA2_DATA_DIR + "/";

  alert->reportMetadata = reportMetadata;

  terrama2::core::Filter filter;
  filter.lastValues = std::make_shared<size_t>(6);

  alert->filter = filter;

  Notification recipient;
  recipient.targets = {"vmimeteste@gmail.com"};
  alert->notifications = { recipient };

  return alertPtr;
}


int main(int argc, char* argv[])
{
  QGuiApplication a(argc, argv);

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);

  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  terrama2::core::registerFactories();
  terrama2::services::alert::core::registerFactories();

  auto dataManager = std::make_shared<terrama2::services::alert::core::DataManager>();

  dataManager->add(inputDataProvider());
  dataManager->add(inputDataSeries());
  dataManager->add(additionalDataProvider());
  dataManager->add(additionalDataSeries());
  auto alert = newAlert();
  dataManager->add(alert);

  te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);

  auto logger = std::make_shared<AlertLogger>();

  logger->setConnectionInfo(uri);

  QJsonObject additionalIfo;
  additionalIfo.insert("email_server", QString("smtp://vmimeteste@gmail.com:a1a2a3a4@smtp.gmail.com:587"));

  terrama2::core::ServiceManager::getInstance().setInstanceId(1);

  terrama2::services::alert::core::Service service(dataManager);
  logger->setConnectionInfo(uri);

  service.setLogger(logger);
  service.updateAdditionalInfo(additionalIfo);
  service.start();
  service.addToQueue(alert->id, terrama2::core::TimeUtils::nowUTC());

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QGuiApplication::instance(), SLOT(quit()));
  timer.start(10000);
  a.exec();
  return 0;
}
