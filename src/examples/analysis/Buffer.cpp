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
    outputDataSet->format.emplace("table_name", "buffer_analysis_result");

    outputDataSeries->datasetList.emplace_back(outputDataSet);


    dataManager->add(outputDataSeries);


    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
    analysis->id = 1;
    analysis->name = "Analysis";
    analysis->active = true;

    std::string script = "moBuffer = Buffer()\n"
                         "x = occurrence.count(\"Occurrence\", moBuffer, \"500d\", \"\")\n"
                         "add_value(\"no_buffer\", x)\n"

                         "moBuffer = Buffer(BufferType.Out_union, 10., \"km\")\n"
                         "x = occurrence.count(\"Occurrence\", moBuffer, \"500d\", \"\")\n"
                         "add_value(\"out_union\", x)\n"

                         "moBuffer = Buffer(BufferType.In, 10., \"km\")\n"
                         "x = occurrence.count(\"Occurrence\", moBuffer, \"500d\", \"\")\n"
                         "add_value(\"buffer_in\", x)\n"

                         "moBuffer = Buffer(BufferType.Out, 10, \"km\")\n"
                         "x = occurrence.count(\"Occurrence\", moBuffer, \"500d\", \"\")\n"
                         "add_value(\"out\", x)\n"

                         "moBuffer = Buffer(BufferType.In_out, 10., \"km\", -10., \"km\")\n"
                         "x = occurrence.count(\"Occurrence\", moBuffer, \"500d\", \"\")\n"
                         "add_value(\"in_out\", x)\n"

                         "moBuffer = Buffer(BufferType.In_diff, 10., \"km\")\n"
                         "x = occurrence.count(\"Occurrence\", moBuffer, \"500d\", \"\")\n"
                         "add_value(\"in_diff\", x)\n"

                         "moBuffer = Buffer(BufferType.Level, 20, \"km\", 5, \"km\")\n"
                         "x = occurrence.count(\"Occurrence\", moBuffer, \"500d\", \"\")\n"
                         "add_value(\"level\", x)\n";


    analysis->script = script;
    analysis->outputDataSeriesId = outputDataSeries->id;
    analysis->outputDataSetId = outputDataSet->id;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->serviceInstanceId = 1;
    dataManager->add(analysis);

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
    dataSet->dataSeriesId = 1;

    dataSeries->datasetList.push_back(dataSet);
    dataManager->add(dataSeries);

    AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeries->id;
    monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    monitoredObjectADS.metadata["identifier"] = "nome";


    //DataProvider information
    std::shared_ptr<terrama2::core::DataProvider> dataProvider2 = std::make_shared<terrama2::core::DataProvider>();
    dataProvider2->id = 2;
    dataProvider2->name = "DataProvider queimadas postgis";
    dataProvider2->uri = uri.uri();
    dataProvider2->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
    dataProvider2->dataProviderType = "POSTGIS";
    dataProvider2->active = true;

    dataManager->add(dataProvider2);

    //DataSeries information
    std::shared_ptr<terrama2::core::DataSeries> occurrenceDataSeries = std::make_shared<terrama2::core::DataSeries>();
    occurrenceDataSeries->id = 2;
    occurrenceDataSeries->name = "Occurrence";
    occurrenceDataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-postgis");
    occurrenceDataSeries->dataProviderId = dataProvider2->id;
    occurrenceDataSeries->active = true;


    //DataSet information
    terrama2::core::DataSetOccurrence* occurrenceDataSet = new terrama2::core::DataSetOccurrence();
    occurrenceDataSet->active = true;
    occurrenceDataSet->id = 2;
    occurrenceDataSet->format.emplace("table_name", "queimadas_test_table");
    occurrenceDataSet->format.emplace("timestamp_property", "data_pas");
    occurrenceDataSet->format.emplace("geometry_property", "geom");
    occurrenceDataSet->format.emplace("timezone", "UTC-03");

    occurrenceDataSeries->datasetList.emplace_back(occurrenceDataSet);

    dataManager->add(occurrenceDataSeries);

    AnalysisDataSeries occurrenceADS;
    occurrenceADS.id = 2;
    occurrenceADS.dataSeriesId = occurrenceDataSeries->id;
    occurrenceADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysisDataSeriesList.push_back(occurrenceADS);

    analysis->analysisDataSeriesList = analysisDataSeriesList;

    analysis->schedule.frequency = 1;
    analysis->schedule.frequencyUnit = "min";



    terrama2::core::ServiceManager::getInstance().setInstanceId(1);

    service.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());



    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    app.exec();
  }





  return 0;
}
