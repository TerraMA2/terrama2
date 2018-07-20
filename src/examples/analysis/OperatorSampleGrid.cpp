#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Shared.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include <extra/data/Geotiff.hpp>

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


    auto dataProvider = terrama2::geotiff::dataProviderFileGrid();
    dataManager->add(dataProvider);


    auto outputDataSeriesGrid = terrama2::geotiff::dataSeriesResultAnalysisGrid(dataProvider,terrama2::geotiff::nameoutputgrid::output_grid);
    dataManager->add(outputDataSeriesGrid);



    auto dataSeries = terrama2::geotiff::dataSeriesL5219076(dataProvider);
    dataManager->add(dataSeries);


    AnalysisDataSeries gridADS1;
    gridADS1.id = 1;
    gridADS1.dataSeriesId = dataSeries->id;
    gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;



    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    analysis->id = 1;
    analysis->name = "Grid Sample";
    analysis->script = R"x(return grid.sample("geotiff 1"))x";
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::GRID_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = outputDataSeriesGrid->id;
    analysis->outputDataSetId = outputDataSeriesGrid->datasetList.front()->id;
    analysis->serviceInstanceId = 1;



    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(gridADS1);
    analysis->analysisDataSeriesList = analysisDataSeriesList;



    std::shared_ptr<AnalysisOutputGrid> outputGrid = std::make_shared<AnalysisOutputGrid>();

    outputGrid->analysisId = 1;
    outputGrid->interpolationMethod = InterpolationMethod::BILINEAR;
    outputGrid->interestAreaType = InterestAreaType::SAME_FROM_DATASERIES;
    outputGrid->interestAreaDataSeriesId = dataSeries->id;
    outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
    outputGrid->resolutionDataSeriesId = dataSeries->id;
    outputGrid->interpolationDummy = 266;

    analysis->outputGridPtr = outputGrid;

    dataManager->add(analysis);


    service.addToQueue(analysis, terrama2::core::TimeUtils::nowUTC());


    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &service, &Service::stopService);
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);

    app.exec();

  }



  return 0;
}
