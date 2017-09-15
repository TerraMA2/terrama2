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

#include "UtilsDCPSerrmarInpe.hpp"
#include "UtilsPostGis.hpp"

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


  Service service(dataManager);
  serviceManager.setInstanceId(1);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(te::core::URI(""));
  serviceManager.setInstanceId(1);

  service.setLogger(logger);
  service.start();



  auto dataProvider = terrama2::examples::analysis::utilspostgis::dataProviderPostGis();
  dataManager->add(dataProvider);


  auto outputDataSeries = terrama2::examples::analysis::utilspostgis::outputDataSeriesPostGis(dataProvider, terrama2::examples::analysis::utilspostgis::dcp_history_interval_result);
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
  analysis->outputDataSeriesId = outputDataSeries->id;
  analysis->outputDataSetId = outputDataSeries->datasetList.front()->id;
  analysis->active = true;
  analysis->serviceInstanceId = 1;


  analysis->metadata["INFLUENCE_TYPE"] = "1";
  analysis->metadata["INFLUENCE_RADIUS"] = "50";
  analysis->metadata["INFLUENCE_RADIUS_UNIT"] = "km";


  //dataSeries Static Data PostGis estados_2010
  auto dataSeries = terrama2::examples::analysis::utilspostgis::dataSeriesPostGis(dataProvider);
  dataManager->add(dataSeries);

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeriesId = dataSeries->id;
  monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
  monitoredObjectADS.metadata["identifier"] = "nome";


  //DataProvider File folder PCD_serrmar_INPE
  auto dataProviderFile = terrama2::examples::analysis::utilsdcpserrmarinpe::dataProviderFile();
  dataManager->add(dataProviderFile);

  auto dcpSeries = terrama2::examples::analysis::utilsdcpserrmarinpe::dataSeriesDCP69031(dataProviderFile);
  dataManager->add(dcpSeries);

  AnalysisDataSeries dcpADS;
  dcpADS.id = 2;
  dcpADS.dataSeriesId = dcpSeries->id;
  dcpADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(dcpADS);
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysis->analysisDataSeriesList = analysisDataSeriesList;

  dataManager->add(analysis);

  service.addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2008-02-25T03:00:00.00-02:00", terrama2::core::TimeUtils::webgui_timefacet));



  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(100000);
  app.exec();


  return 0;
}
