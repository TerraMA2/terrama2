#include <terrama2/core/Shared.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/utility/Utils.hpp>

#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/GeoUtils.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/data-model/DataManager.hpp>

#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/Collector.hpp>

#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <terrama2/Config.hpp>

// QT
#include <QTimer>
#include <QString>
#include <QCoreApplication>

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
    outputDataSet->format.emplace("table_name", "zonal_analysis_result");

    outputDataSeries->datasetList.emplace_back(outputDataSet);


    dataManager->add(outputDataSeries);


    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
    analysis->id = 1;
    analysis->name = "Analysis";
    analysis->active = true;
    analysis->outputDataSetId = outputDataSet->id;
    analysis->outputDataSeriesId = 3;
    std::string script = "x = grid.zonal.count(\"geotiff 1\")\n"
                         "add_value(\"min\", x)\n"
                         "return";


    analysis->script = script;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->serviceInstanceId = 1;


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
    dataSeries->semantics.dataSeriesType = terrama2::core::DataSeriesType::GEOMETRIC_OBJECT;
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


    // DataProvider information
    std::shared_ptr<terrama2::core::DataProvider> dataProvider2 = std::make_shared<terrama2::core::DataProvider>();
    dataProvider2->uri = "file://"+TERRAMA2_DATA_DIR+"/geotiff";
    dataProvider2->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider2->dataProviderType = "FILE";
    dataProvider2->active = true;
    dataProvider2->id = 2;
    dataProvider2->name = "Local Geotiff";

    dataManager->add(dataProvider2);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data Series 2
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<terrama2::core::DataSeries> dataSeries2 = std::make_shared<terrama2::core::DataSeries>();
    dataSeries2->semantics = semanticsManager.getSemantics("GRID-static_gdal");
    dataSeries2->name = "geotiff 1";
    dataSeries2->id = 2;
    dataSeries2->dataProviderId = 2;
    dataSeries2->active = true;

    terrama2::core::DataSetGrid* dataSet1 = new terrama2::core::DataSetGrid();
    dataSet1->active = true;
    dataSet1->format.emplace("mask", "Spot_Vegetacao_Jul2001_SP.tif");
    dataSet1->id = 2;

    dataSeries2->datasetList.emplace_back(dataSet1);

    AnalysisDataSeries gridADS1;
    gridADS1.id = 2;
    gridADS1.dataSeriesId = dataSeries2->id;
    gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    dataManager->add(dataSeries2);

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysisDataSeriesList.push_back(gridADS1);

    analysis->analysisDataSeriesList = analysisDataSeriesList;


    std::shared_ptr<AnalysisOutputGrid> outputGrid = std::make_shared<AnalysisOutputGrid>();
    outputGrid->analysisId = 1;
    outputGrid->interpolationMethod = InterpolationMethod::BILINEAR;
    outputGrid->interestAreaType = InterestAreaType::SAME_FROM_DATASERIES;
    outputGrid->interestAreaDataSeriesId = 1;
    outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
    outputGrid->resolutionDataSeriesId = 1;
    outputGrid->interpolationDummy = -1;

    analysis->outputGridPtr = outputGrid;

    dataManager->add(analysis);

    terrama2::core::ServiceManager::getInstance().setInstanceId(1);



    service.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());


    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    app.exec();
  }





  return 0;
}
