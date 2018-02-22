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


#include <examples/data/DCPSerramarInpe.hpp>
#include <examples/data/ResultAnalysisPostGis.hpp>
#include <examples/data/StaticPostGis.hpp>


// QT
#include <QTimer>
#include <QCoreApplication>
#include <QUrl>

using namespace terrama2::services::analysis::core;

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  Q_UNUSED(terramaRaii);

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

  service.setLogger(logger);
  service.start();

  /*
   * DataProvider and dataSeries Static
  */

  auto dataProviderStatic = terrama2::staticpostgis::dataProviderStaticPostGis();
  dataManager->add(dataProviderStatic);

  auto dataSeries = terrama2::staticpostgis::dataSeriesEstados2010(dataProviderStatic);
  dataManager->add(dataSeries);

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeriesId = dataSeries->id;
  monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
  monitoredObjectADS.metadata["identifier"] = "fid";


  /*
   * DataProvider and dataSeries result
  */
  auto dataProviderResult = terrama2::resultanalysis::dataProviderResultAnalysis();
  dataManager->add(dataProviderResult);


  auto outputDataSeries = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProviderResult,
                                                                                    terrama2::resultanalysis::tablename::dcp_history_interval_result,
                                                                                    dataSeries);
  dataManager->add(outputDataSeries);


  std::string script = R"z(moBuffer = Buffer(BufferType.Out_union, 2., "km")
ids = dcp.zonal.influence.by_rule("Serra do Mar", moBuffer)
x = dcp.zonal.history.interval.sum("Serra do Mar", "Pluvio", "48h", "24h", ids)
add_value("history_sum",x)
x = dcp.zonal.history.interval.max("Serra do Mar", "Pluvio", "48h", "24h", ids)
add_value("history_max",x)
x = dcp.zonal.history.interval.min("Serra do Mar", "Pluvio", "48h", "24h", ids)
add_value("history_min",x)
x = dcp.zonal.history.interval.mean("Serra do Mar", "Pluvio", "48h", "24h", ids)
add_value("history_mean",x)
x = dcp.zonal.history.interval.median("Serra do Mar", "Pluvio", "48h", "24h", ids)
add_value("history_median",x)
x = dcp.zonal.history.interval.standard_deviation("Serra do Mar", "Pluvio", "48h", "24h", ids)
add_value("history_standard_deviation",x))z";


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



  /*
   * DataProvider and dataSeries Serramar
  */

  auto dataProviderDCP = terrama2::serramar::dataProviderPostGisDCP();
  dataManager->add(dataProviderDCP);


  auto dcpSeries = terrama2::serramar::dataSeriesDcpSerramarPostGis(dataProviderDCP);
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

  service.addToQueue(analysis, terrama2::core::TimeUtils::stringToTimestamp("2008-07-21T10:00:00-03:00", terrama2::core::TimeUtils::webgui_timefacet));



  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(100000);
  app.exec();


  return 0;
}
