#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/analysis/core/Shared.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <terrama2/impl/Utils.hpp>

#include <iostream>

// QT
#include <QTimer>
#include <QCoreApplication>
#include <QUrl>

using namespace terrama2::services::analysis::core;

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  terrama2::core::registerFactories();

  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

  QCoreApplication app(argc, argv);

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();

  auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();

  auto loggerCopy = std::make_shared<terrama2::core::MockAnalysisLogger>();

  EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*loggerCopy, isValid()).WillRepeatedly(::testing::Return(true));

  auto logger = std::make_shared<terrama2::core::MockAnalysisLogger>();

  EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));
  EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));

  te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);

  Service service(dataManager);
  serviceManager.setInstanceId(1);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(te::core::URI(""));
  serviceManager.setInstanceId(1);

  service.setLogger(logger);
  service.start();

  // DataProvider information
  std::shared_ptr<terrama2::core::DataProvider> outputDataProvider = std::make_shared<terrama2::core::DataProvider>();
  outputDataProvider->id = 3;
  outputDataProvider->name = "DataProvider postgis";
  outputDataProvider->uri = uri.uri();
  outputDataProvider->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
  outputDataProvider->dataProviderType = "POSTGIS";
  outputDataProvider->active = true;


  dataManager->add(outputDataProvider);


  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  // DataSeries information
  std::shared_ptr<terrama2::core::DataSeries> outputDataSeries = std::make_shared<terrama2::core::DataSeries>();
  outputDataSeries->id = 3;
  outputDataSeries->name = "Analysis result";
  outputDataSeries->semantics = semanticsManager.getSemantics("ANALYSIS_MONITORED_OBJECT-postgis");
  outputDataSeries->dataProviderId = outputDataProvider->id;


  // DataSet information
  terrama2::core::DataSet* outputDataSet = new terrama2::core::DataSet();
  outputDataSet->active = true;
  outputDataSet->id = 2;
  outputDataSet->dataSeriesId = outputDataSeries->id;
  outputDataSet->format.emplace("table_name", "dcp_history_interval_result");

  outputDataSeries->datasetList.emplace_back(outputDataSet);


  dataManager->add(outputDataSeries);

  std::string script = "moBuffer = Buffer(BufferType.Out_union, 2., \"km\")\n"
          "ids = dcp.zonal.influence.by_rule(\"Serra do Mar\", moBuffer)\n"
          "x = dcp.zonal.history.interval.sum(\"Serra do Mar\", \"Pluvio\", \"48h\", \"24h\", ids)\n"
          "add_value(\"history_sum\",x)\n"
          "x = dcp.zonal.history.interval.max(\"Serra do Mar\", \"Pluvio\", \"48h\", \"24h\", ids)\n"
          "add_value(\"history_max\",x)\n"
          "x = dcp.zonal.history.interval.min(\"Serra do Mar\", \"Pluvio\", \"48h\", \"24h\", ids)\n"
          "add_value(\"history_min\",x)\n"
          "x = dcp.zonal.history.interval.mean(\"Serra do Mar\", \"Pluvio\", \"48h\", \"24h\", ids)\n"
          "add_value(\"history_mean\",x)\n"
          "x = dcp.zonal.history.interval.median(\"Serra do Mar\", \"Pluvio\", \"48h\", \"24h\", ids)\n"
          "add_value(\"history_median\",x)\n"
          "x = dcp.zonal.history.interval.standard_deviation(\"Serra do Mar\", \"Pluvio\", \"48h\", \"24h\", ids)\n"
          "add_value(\"history_standard_deviation\",x)\n";


  std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
  analysis->id = 1;
  analysis->name = "History DCP";
  analysis->script = script;
  analysis->scriptLanguage = ScriptLanguage::PYTHON;
  analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
  analysis->outputDataSeriesId = 3;
  analysis->outputDataSetId = outputDataSet->id;
  analysis->active = true;
  analysis->serviceInstanceId = 1;


  analysis->metadata["INFLUENCE_TYPE"] = "1";
  analysis->metadata["INFLUENCE_RADIUS"] = "50";
  analysis->metadata["INFLUENCE_RADIUS_UNIT"] = "km";

  std::shared_ptr<terrama2::core::DataProvider> dataProvider = std::make_shared<terrama2::core::DataProvider>();
  dataProvider->name = "Provider";
  dataProvider->uri = uri.uri();
  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "POSTGIS";
  dataProvider->active = true;
  dataProvider->id = 1;


  dataManager->add(dataProvider);

  std::shared_ptr<terrama2::core::DataSeries> dataSeries = std::make_shared<terrama2::core::DataSeries>();
  dataSeries->dataProviderId = dataProvider->id;
  dataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-postgis");
  dataSeries->name = "Monitored Object";
  dataSeries->id = 1;
  dataSeries->dataProviderId = 1;
  dataSeries->active = true;

  //DataSet information
  std::shared_ptr<terrama2::core::DataSet> dataSet = std::make_shared<terrama2::core::DataSet>();
  dataSet->active = true;
  dataSet->format.emplace("table_name", "estados_2010");
  dataSet->id = 1;

  dataSeries->datasetList.push_back(dataSet);
  dataManager->add(dataSeries);


  std::shared_ptr<terrama2::core::DataProvider> dataProvider2 = std::make_shared<terrama2::core::DataProvider>();
  dataProvider2->name = "Provider";
  dataProvider2->uri += TERRAMA2_DATA_DIR;
  dataProvider2->uri += "/PCD_serrmar_INPE";
  dataProvider2->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider2->dataProviderType = "FILE";
  dataProvider2->active = true;
  dataProvider2->id = 2;


  dataManager->add(dataProvider2);

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeriesId = dataSeries->id;
  monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
  monitoredObjectADS.metadata["identifier"] = "nome";


  //DataSeries information
  std::shared_ptr<terrama2::core::DataSeries> dcpSeries = std::make_shared<terrama2::core::DataSeries>();
  dcpSeries->dataProviderId = dataProvider2->id;
  dcpSeries->semantics = semanticsManager.getSemantics("DCP-inpe");
  dcpSeries->name = "Serra do Mar";
  dcpSeries->id = 2;
  dcpSeries->dataProviderId = 2;
  dcpSeries->active = true;

  //DataSet information
  /*std::shared_ptr<terrama2::core::DataSetDcp> dcpDataset69031 = std::make_shared<terrama2::core::DataSetDcp>();
  dcpDataset69031->active = true;
  dcpDataset69031->format.emplace("mask", "69031.txt");
  dcpDataset69031->format.emplace("alias", "69031");
  dcpDataset69031->format.emplace("timezone", "-02:00");
  dcpDataset69031->dataSeriesId = 2;
  dcpDataset69031->id = 2;
  dcpDataset69031->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-44.46540, -23.00506, 4618, te::gm::PointType, nullptr));
  dcpSeries->datasetList.push_back(dcpDataset69031);*/

  std::shared_ptr<terrama2::core::DataSetDcp> dcpDataset69034 = std::make_shared<terrama2::core::DataSetDcp>();
  dcpDataset69034->active = true;
  dcpDataset69034->format.emplace("mask", "69034.txt");
  dcpDataset69034->format.emplace("alias", "dcp_69034");
  dcpDataset69034->format.emplace("timezone", "-02:00");
  dcpDataset69034->dataSeriesId = 2;
  dcpDataset69034->id = 2;
  dcpDataset69034->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-44.46540, -23.00506, 4618, te::gm::PointType, nullptr));
  dcpSeries->datasetList.push_back(dcpDataset69034);


  std::shared_ptr<terrama2::core::DataSetDcp> dcpDataset30885 = std::make_shared<terrama2::core::DataSetDcp>();
  dcpDataset30885->active = true;
  dcpDataset30885->format.emplace("mask", "30885.txt");
  dcpDataset30885->format.emplace("alias", "dcp_30885");
  dcpDataset30885->format.emplace("timezone", "-02:00");
  dcpDataset30885->dataSeriesId = 2;
  dcpDataset30885->id = 3;
  dcpDataset30885->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-46.121, -23.758, 4618, te::gm::PointType, nullptr));
  dcpSeries->datasetList.push_back(dcpDataset30885);




  AnalysisDataSeries dcpADS;
  dcpADS.id = 2;
  dcpADS.dataSeriesId = dcpSeries->id;
  dcpADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;

  dataManager->add(dcpSeries);


  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(dcpADS);
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysis->analysisDataSeriesList = analysisDataSeriesList;

  dataManager->add(analysis);

  service.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(100000);
  app.exec();




  return 0;
}
