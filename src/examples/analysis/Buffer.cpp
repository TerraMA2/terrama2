#include <terrama2/core/Shared.hpp>
#include <terrama2/impl/Utils.hpp>

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>

#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>

#include <terrama2/Config.hpp>

#include <examples/data/ResultAnalysisPostGis.hpp>
#include <examples/data/StaticPostGis.hpp>
#include <examples/data/OccurrenceWFP.hpp>

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
  Q_UNUSED(terramaRaii);

  terrama2::core::registerFactories();

  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

  {
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

    //DataProvider and DataSeries static postgis estados_2010

    auto dataProviderStatic = terrama2::staticpostgis::dataProviderStaticPostGis();
    dataManager->add(dataProviderStatic);

    auto dataSeriesEstados = terrama2::staticpostgis::dataSeriesEstados2010(dataProviderStatic);
    dataManager->add(dataSeriesEstados);

    AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeriesEstados->id;
    monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    monitoredObjectADS.metadata["identifier"] = "fid";

    // DataProvider information
    auto dataProviderResult = terrama2::resultanalysis::dataProviderResultAnalysis();
    dataManager->add(dataProviderResult);


    auto dataSeriesResult = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProviderResult, terrama2::resultanalysis::tablename::buffer_analysis_result,dataSeriesEstados);
    dataManager->add(dataSeriesResult);


    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
    analysis->id = 1;
    analysis->name = "Analysis";
    analysis->active = true;

    std::string script = R"z(moBuffer = Buffer()
x = occurrence.zonal.count("Occurrence", "2d", moBuffer)
add_value("no_buffer", x)

moBuffer = Buffer(BufferType.Out_union, 10., "km")
x = occurrence.zonal.count("Occurrence", "2d" , moBuffer)
add_value("out_union", x)

moBuffer = Buffer(BufferType.In, 10., "km")
x = occurrence.zonal.count("Occurrence",  "2d" , moBuffer)
add_value("buffer_in", x)

moBuffer = Buffer(BufferType.Out, 10, "km")
x = occurrence.zonal.count("Occurrence",  "2d", moBuffer)
add_value("out", x)

moBuffer = Buffer(BufferType.In_out, 10., "km", -10., "km")
x = occurrence.zonal.count("Occurrence", "2d", moBuffer)
add_value("in_out", x)

moBuffer = Buffer(BufferType.In_diff, 10., "km")
x = occurrence.zonal.count("Occurrence",  "2d", moBuffer)
add_value("in_diff", x)

moBuffer = Buffer(BufferType.Level, 20, "km", 5, "km")
x = occurrence.zonal.count("Occurrence",  "2d", moBuffer)
add_value("level", x))z";


    analysis->script = script;
    analysis->outputDataSeriesId = dataSeriesResult->id;
    analysis->outputDataSetId = dataSeriesResult->datasetList.front()->id;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->serviceInstanceId = 1;
    dataManager->add(analysis);







    //DataProvider and DataSeries information Occurrence

    auto dataProviderOcc = terrama2::occurrencewfp::dataProviderPostGisOccWFP();
    dataManager->add(dataProviderOcc);

    auto occurrenceDataSeries = terrama2::occurrencewfp::dataSeriesOccWFPPostGis(dataProviderOcc);
    dataManager->add(occurrenceDataSeries);

    AnalysisDataSeries occurrenceADS;
    occurrenceADS.id = 2;
    occurrenceADS.dataSeriesId = occurrenceDataSeries->id;
    occurrenceADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysisDataSeriesList.push_back(occurrenceADS);

    analysis->analysisDataSeriesList = analysisDataSeriesList;


    terrama2::core::ServiceManager::getInstance().setInstanceId(1);


    service.addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2016-04-30T22:30:00-03", terrama2::core::TimeUtils::webgui_timefacet));



    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    app.exec();
  }





  return 0;
}
