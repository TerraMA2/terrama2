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
#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Shared.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <terrama2/impl/Utils.hpp>


#include <examples/data/ResultAnalysisPostGis.hpp>
#include <examples/data/OccurrenceWFP.hpp>
#include <examples/data/StaticPostGis.hpp>

// STL
#include <iostream>
#include <memory>

// QT
#include <QTimer>
#include <QCoreApplication>
#include <QUrl>
#include "../../terrama2/services/analysis/core/AnalysisLogger.hpp"

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

  auto outputDataSeries =  terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProviderResult, terrama2::resultanalysis::tablename::reprocessing_result, dataSeries);
  dataManager->add(outputDataSeries);


  std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

  std::string script = R"(moBuffer = Buffer()
x = occurrence.zonal.count("Occurrence", "6h", moBuffer)
add_value("count", x))";

  analysis->id = 1;
  analysis->name = "Analysis";
  analysis->active = true;
  analysis->script = script;
  analysis->outputDataSeriesId = outputDataSeries->id;
  analysis->outputDataSetId = outputDataSeries->datasetList.front()->id;
  analysis->scriptLanguage = ScriptLanguage::PYTHON;
  analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
  analysis->serviceInstanceId = 1;



  /*
   * DataProvider and dataSeries Occurrence
*/
  auto dataProviderOcc = terrama2::occurrencewfp::dataProviderPostGisOccWFP();
  dataManager->add(dataProviderOcc);

  auto occurrenceDataSeries = terrama2::occurrencewfp::dataSeriesOccWFPPostGis(dataProviderOcc);
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


  auto reprocessingHistoricalData = new terrama2::core::ReprocessingHistoricalData();
  terrama2::core::ReprocessingHistoricalDataPtr reprocessingHistoricalDataPtr(reprocessingHistoricalData);


  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("-03"));


  std::string startDate = "2016-04-30 20:15:00";
  boost::posix_time::ptime startBoostDate(boost::posix_time::time_from_string(startDate));
  boost::local_time::local_date_time lstartDate(startBoostDate.date(), startBoostDate.time_of_day(), zone, true);
  reprocessingHistoricalData->startDate = std::make_shared<te::dt::TimeInstantTZ>(lstartDate);

  std::string endDate = "2016-05-01 08:00:00";
  boost::posix_time::ptime endBoostDate(boost::posix_time::time_from_string(endDate));
  boost::local_time::local_date_time lendDate(endBoostDate.date(), endBoostDate.time_of_day(), zone, true);
  reprocessingHistoricalData->endDate = std::make_shared<te::dt::TimeInstantTZ>(lendDate);

  analysis->schedule.reprocessingHistoricalData = reprocessingHistoricalDataPtr;

  dataManager->add(analysis);

  analysis->schedule.frequency = 15;
  analysis->schedule.frequencyUnit = "min";

  service.addToQueue(analysis, terrama2::core::TimeUtils::stringToTimestamp("2016-04-30T00:00:00-03", terrama2::core::TimeUtils::webgui_timefacet));



  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  app.exec();

  return 0;
}
