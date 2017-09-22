#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/Logger.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Shared.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <terrama2/impl/Utils.hpp>

#include "UtilsDCPSerrmarInpe.hpp"
#include "UtilsPostGis.hpp"


// STL
#include <iostream>
#include <memory>

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

  auto outputDataSeries = terrama2::examples::analysis::utilspostgis::outputDataSeriesPostGis(dataProvider, terrama2::examples::analysis::utilspostgis::occurrence_analysis_result);
  dataManager->add(outputDataSeries);


  std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
  analysis->id = 1;
  analysis->name = "Analysis";
  analysis->active = true;

  std::string script = R"z(x = occurrence.zonal.count("Occurrence", "2d")
add_value("count", x))z";


  analysis->script = script;
  analysis->outputDataSeriesId = outputDataSeries->id;
  analysis->outputDataSetId = outputDataSeries->datasetList.front()->id;
  analysis->scriptLanguage = ScriptLanguage::PYTHON;
  analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
  analysis->serviceInstanceId = 1;


  auto dataSeries = terrama2::examples::analysis::utilspostgis::dataSeriesPostGis(dataProvider);
  dataManager->add(dataSeries);

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeriesId = dataSeries->id;
  monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
  monitoredObjectADS.metadata["identifier"] = "nome";


  //Data Occurrence
  auto occurrenceDataSeries = terrama2::examples::analysis::utilspostgis::occurrenceDataSeriesPostGis(dataProvider);
  dataManager->add(occurrenceDataSeries);

  AnalysisDataSeries occurrenceADS;
  occurrenceADS.id = 2;
  occurrenceADS.dataSeriesId = occurrenceDataSeries->id;
  occurrenceADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;
  occurrenceADS.alias = "occ";

  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysisDataSeriesList.push_back(occurrenceADS);

  analysis->analysisDataSeriesList = analysisDataSeriesList;

  analysis->schedule.id = 1;
  analysis->schedule.frequency = 30;
  analysis->schedule.frequencyUnit = "sec";

  dataManager->add(analysis);

  service.addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2016-04-30T20:15:00-03", terrama2::core::TimeUtils::webgui_timefacet));


  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(100000);
  app.exec();




  return 0;
}

