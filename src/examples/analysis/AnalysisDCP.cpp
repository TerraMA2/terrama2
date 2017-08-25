#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/GeoUtils.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>

#include<terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <terrama2/impl/Utils.hpp>
#include <terrama2/Config.hpp>

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

  {
    auto& serviceManager = terrama2::core::ServiceManager::getInstance();

    auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();

    // Starts the service and adds the analysis

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

    te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/pcd_angra");
    logger->setConnectionInfo(uri);

    serviceManager.setInstanceId(1);
    serviceManager.setLogger(logger);
    serviceManager.setLogConnectionInfo(uri);


    terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

    QCoreApplication app(argc, argv);




    // DataProvider information
    std::shared_ptr<terrama2::core::DataProvider> dataProvider = std::make_shared<terrama2::core::DataProvider>();
    dataProvider->id = 3;
    dataProvider->name = "DataProvider postgis";
    dataProvider->uri = uri.uri();
    dataProvider->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
    dataProvider->dataProviderType = "POSTGIS";
    dataProvider->active = true;

    dataManager->add(dataProvider);

    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

    // DataSeries information
    std::shared_ptr<terrama2::core::DataSeries> outputDataSeries = std::make_shared<terrama2::core::DataSeries>();
    outputDataSeries->id = 3;
    outputDataSeries->name = "Analysis result";
    outputDataSeries->semantics = semanticsManager.getSemantics("ANALYSIS_MONITORED_OBJECT-postgis");
    outputDataSeries->dataProviderId = dataProvider->id;


    // DataSet information
    terrama2::core::DataSet* outputDataSet = new terrama2::core::DataSet();
    outputDataSet->active = true;
    outputDataSet->id = 2;
    outputDataSet->dataSeriesId = outputDataSeries->id;
    outputDataSet->format.emplace("table_name", "dcp_result");

    outputDataSeries->datasetList.emplace_back(outputDataSet);


    dataManager->add(outputDataSeries);

    std::string script = "add_value(\"standard_deviation\", 10)\n";

    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    analysis->id = 1;
    analysis->name = "Min DCP";
    analysis->script = script;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::DCP_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = outputDataSeries->id;
    analysis->outputDataSetId = outputDataSet->id;
    analysis->serviceInstanceId = 1;

    analysis->metadata["INFLUENCE_TYPE"] = "1";
    analysis->metadata["INFLUENCE_RADIUS"] = "50";
    analysis->metadata["INFLUENCE_RADIUS_UNIT"] = "km";

    std::shared_ptr<terrama2::core::DataSeries> dataSeries = std::make_shared<terrama2::core::DataSeries>();
    dataSeries->dataProviderId = dataProvider->id;
    dataSeries->semantics = semanticsManager.getSemantics("DCP-postgis");
    dataSeries->name = "Monitored Object";
    dataSeries->id = 57;
    dataSeries->active = true;
    dataSeries->dataProviderId = dataProvider->id;

    //DataSet information
    std::shared_ptr<terrama2::core::DataSetDcp> dataSet = std::make_shared<terrama2::core::DataSetDcp>();
    dataSet->active = true;
    dataSet->format.emplace("timestamp_property", "datetime");
    dataSet->format.emplace("alias", "paraibuna");
    dataSet->format.emplace("table_name", "paraibuna");
    dataSet->format.emplace("timezone", "0");
    auto geom = terrama2::core::ewktToGeom("SRID=4618;POINT(-61 -23.408)");
    dataSet->position = std::dynamic_pointer_cast<te::gm::Point>(geom);
    dataSet->id = 1;
    dataSeries->datasetList.push_back(dataSet);

    //DataSet information
    std::shared_ptr<terrama2::core::DataSetDcp> dataSet2 = std::make_shared<terrama2::core::DataSetDcp>();
    dataSet2->active = true;
    dataSet2->format.emplace("timestamp_property", "datetime");
    dataSet2->format.emplace("alias", "picinguaba");
    dataSet2->format.emplace("table_name", "picinguaba");
    dataSet2->format.emplace("timezone", "0");
    auto geom2 = terrama2::core::ewktToGeom("SRID=4618;POINT(-44.85 -23.355)");
    dataSet2->position = std::dynamic_pointer_cast<te::gm::Point>(geom2);
    dataSet2->id = 2;
    dataSeries->datasetList.push_back(dataSet2);

    //DataSet information
    std::shared_ptr<terrama2::core::DataSetDcp> dataSet3 = std::make_shared<terrama2::core::DataSetDcp>();
    terrama2::core::DataSetPtr dataSet3Ptr(dataSet3);
    dataSet3->active = true;
    dataSet3->format.emplace("timestamp_property", "datetime");
    dataSet3->format.emplace("alias", "itanhaem");
    dataSet3->format.emplace("table_name", "itanhaem");
    dataSet3->format.emplace("timezone", "0");
    auto geom3 = terrama2::core::ewktToGeom("SRID=4618;POINT(-44.941 -23.074)");
    dataSet3->position = std::dynamic_pointer_cast<te::gm::Point>(geom3);
    dataSet3->id = 3;
    dataSeries->datasetList.push_back(dataSet3Ptr);

    dataManager->add(dataSeries);

    AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeries->id;
    monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_PCD_TYPE;
    monitoredObjectADS.metadata["identifier"] = "table_name";

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysis->analysisDataSeriesList = analysisDataSeriesList;

    dataManager->add(analysis);

    // Starts the service and adds the analysis
    Service service(dataManager);
    service.setLogger(logger);

    service.start();
    service.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);

    app.exec();
  }

  return 0;
}
