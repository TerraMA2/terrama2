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

#include <terrama2/services/alert/mock/MockAlertLogger.hpp>

#include <examples/data/ResultAnalysisPostGis.hpp>
#include <examples/data/StaticPostGis.hpp>
#include <iostream>

//QT
#include <QUrl>
#include <QtGui>
#include <QTimer>


terrama2::core::LegendPtr newLegend()
{
  auto legend = std::make_shared<terrama2::core::Risk>();
  legend->name = "Temperature levels";
  legend->id = 1;


  terrama2::core::RiskLevel level1;
  level1.level = 0;
  level1.value = 0;
  level1.name = "low";
  legend->riskLevels.push_back(level1);

  terrama2::core::RiskLevel level2;
  level2.level = 1;
  level2.value = 284;
  level2.name = "medium";
  legend->riskLevels.push_back(level2);

  terrama2::core::RiskLevel level3;
  level3.level = 2;
  level3.value = 275996;
  level3.name = "high";
  legend->riskLevels.push_back(level3);

  return legend;
}

terrama2::services::alert::core::AlertPtr newAlert(terrama2::core::DataSeriesPtr dataSeries)
{
  auto alert = std::make_shared<terrama2::services::alert::core::Alert>();

  alert->id = 1;
  alert->projectId = 1;
  alert->riskAttribute = "min";
  alert->dataSeriesId = dataSeries->id;
  alert->active = true;
  alert->name = "Example alert";
  alert->serviceInstanceId = 1;
  alert->description = "Example alert";
  alert->riskId = 1;


  terrama2::services::alert::core::AdditionalData additionalData;
  additionalData.dataSeriesId = dataSeries->id;
  additionalData.dataSetId = dataSeries->datasetList.front()->id;
  additionalData.referrerAttribute = "fid";
  additionalData.referredAttribute = "fid";
  additionalData.attributes.push_back("min");

  alert->additionalDataVector.push_back(additionalData);



  std::unordered_map<std::string, std::string> reportMetadata;

  reportMetadata[terrama2::services::alert::core::ReportTags::TITLE] = "NUMERIC RISK EXAMPLE REPORT";
  reportMetadata[terrama2::services::alert::core::ReportTags::ABSTRACT] = "NumericRisk example.";
  reportMetadata[terrama2::services::alert::core::ReportTags::AUTHOR] = "TerraMA2";
  reportMetadata[terrama2::services::alert::core::ReportTags::CONTACT] = "TerraMA2 developers.";
  reportMetadata[terrama2::services::alert::core::ReportTags::COPYRIGHT] = "copyright information...";
  reportMetadata[terrama2::services::alert::core::ReportTags::DESCRIPTION] = "Example generated report...";
  reportMetadata[terrama2::services::alert::core::ReportTags::DOCUMENT_URI] = "/" + TERRAMA2_DATA_DIR + "/NumericRisk.pdf";
  reportMetadata[terrama2::services::alert::core::ReportTags::TIMESTAMP_FORMAT] = "null";
  reportMetadata[terrama2::services::alert::core::ReportTags::LOGO_PATH] = "null";

  alert->reportMetadata = reportMetadata;

  terrama2::core::Filter filter;
  filter.lastValues = std::make_shared<size_t>(6);

  alert->filter = filter;

  terrama2::services::alert::core::Notification notification;
  notification.targets = {"bianca.maciel.c@gmail.com"};
  notification.includeReport = "PDF";
  notification.notifyOnChange = "false";
  notification.simplifiedReport = "false";

  alert->notifications = { notification };

  return alert;
}


int main(int argc, char* argv[])
{
  QGuiApplication a(argc, argv);


  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  Q_UNUSED(terramaRaii);

  terrama2::core::registerFactories();
  terrama2::services::alert::core::registerFactories();


  auto& serviceManager = terrama2::core::ServiceManager::getInstance();

  auto dataManager = std::make_shared<terrama2::services::alert::core::DataManager>();

  auto loggerCopy = std::make_shared<terrama2::core::MockAlertLogger>();

  EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*loggerCopy, isValid()).WillRepeatedly(::testing::Return(true));
  EXPECT_CALL(*loggerCopy, result(::testing::_, ::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());


  auto logger = std::make_shared<terrama2::core::MockAlertLogger>();

  EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));
  EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));
  EXPECT_CALL(*logger, result(::testing::_, ::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());


  terrama2::services::alert::core::Service service(dataManager);
  serviceManager.setInstanceId(1);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(te::core::URI(""));

  service.setLogger(logger);
  service.start();


  auto dataProvider = terrama2::resultanalysis::dataProviderResultAnalysis();
  dataManager->add(dataProvider);


  auto dataProviderStatic = terrama2::staticpostgis::dataProviderStaticPostGis();
  dataManager->add(dataProviderStatic);


  auto dataSeriesStatic = terrama2::staticpostgis::dataSeriesEstados2010(dataProviderStatic);
  dataManager->add(dataSeriesStatic);


  auto dataSeries = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProvider,
                                                                              terrama2::resultanalysis::tablename::zonal_analysis_result,
                                                                              dataSeriesStatic);
  dataManager->add(dataSeries);




  auto alert = newAlert(dataSeries);
  auto legend = newLegend();
  dataManager->add(alert);
  dataManager->add(legend);


  QJsonObject additionalIfo;
  additionalIfo.insert("email_server", QString("smtp://terrama2.testesalerta@gmail.com:terr@m@2v4@smtp.gmail.com:587"));

  service.updateAdditionalInfo(additionalIfo);


  service.addToQueue(alert->id, terrama2::core::TimeUtils::nowUTC());

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QGuiApplication::instance(), SLOT(quit()));
  timer.start(150000);
  a.exec();

  return 0;
}
