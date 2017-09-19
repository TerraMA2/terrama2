#include <terrama2/core/Shared.hpp>
#include <terrama2/impl/Utils.hpp>

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/GeoUtils.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/core/data-model/DataManager.hpp>

#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <terrama2/Config.hpp>

#include "UtilsGeotiff.hpp"

// QT
#include <QTimer>
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

    te::core::URI uri("");

    Service service(dataManager);
    serviceManager.setInstanceId(1);
    serviceManager.setLogger(logger);
    serviceManager.setLogConnectionInfo(uri);
    serviceManager.setInstanceId(1);

    service.setLogger(logger);
    service.start();


    // DataProvider information
    auto dataProvider = terrama2::examples::analysis::utilsgeotiff::dataProviderFile();
    dataManager->add(dataProvider);


    //DataSeries information
    auto outputDataSeries =  terrama2::examples::analysis::utilsgeotiff::outputDataSeries(dataProvider);
    dataManager->add(outputDataSeries);



    //DATASERIES AND DATASET folder TERRAMA2_DATA_DIR+"/geotiff/historical
    auto dataSeries = terrama2::examples::analysis::utilsgeotiff::dataSeries(dataProvider);
    dataManager->add(dataSeries);


    AnalysisDataSeries gridADS1;
    gridADS1.id = 1;
    gridADS1.dataSeriesId = dataSeries->id;
    gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Analysis
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
    analysis->id = 1;
    analysis->name = "Grid Sample";
    analysis->script = "return grid.history.interval.min(\"geotiff 1\", \"30d\", \"0d\")";
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::GRID_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = outputDataSeries->id;
    analysis->outputDataSetId = outputDataSeries->datasetList.front()->id;
    analysis->serviceInstanceId = 1;

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(gridADS1);
    analysis->analysisDataSeriesList = analysisDataSeriesList;

    analysis->schedule.frequency = 1;
    analysis->schedule.frequencyUnit = "min";

    std::shared_ptr<AnalysisOutputGrid> outputGrid = std::make_shared<AnalysisOutputGrid>();
    outputGrid->analysisId = 1;
    outputGrid->interpolationMethod = InterpolationMethod::BILINEAR;
    outputGrid->interestAreaType = InterestAreaType::SAME_FROM_DATASERIES;
    outputGrid->interestAreaDataSeriesId = 1;
    outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
    outputGrid->resolutionDataSeriesId = 1;
    outputGrid->interpolationDummy = 266;

    analysis->outputGridPtr = outputGrid;

    dataManager->add(analysis);



    service.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &service, &Service::stopService);
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);

    app.exec();

  }

  return 0;
}
