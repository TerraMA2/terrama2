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
#include "UtilsPostGis.hpp"
#include "UtilsGeotiff.hpp"

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
    auto dataProvider = terrama2::examples::analysis::utilspostgis::dataProviderPostGis();
    dataManager->add(dataProvider);

    // DataSeries information
    auto outputDataSeries = terrama2::examples::analysis::utilspostgis::outputDataSeriesPostGis(dataProvider, terrama2::examples::analysis::utilspostgis::zonal_history_ratio_analysis_result);
    dataManager->add(outputDataSeries);

    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
    analysis->id = 1;
    analysis->name = "Analysis";
    analysis->active = true;
    std::string script = "x = grid.zonal.history.accum.max(\"geotiff 1\", \"30d\")\n"
                         "add_value(\"max\", x)\n";

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


    // DataProvider information
    auto dataProviderGeoTif = terrama2::examples::analysis::utilsgeotiff::dataProviderFile();
    dataManager->add(dataProviderGeoTif);

    auto dataSeriesStaticGdal = terrama2::examples::analysis::utilsgeotiff::dataSeriesStaticGdal(dataProviderGeoTif);
    dataManager->add(dataSeriesStaticGdal);

    AnalysisDataSeries gridADS1;
    gridADS1.id = 2;
    gridADS1.dataSeriesId = dataSeriesStaticGdal->id;
    gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysisDataSeriesList.push_back(gridADS1);

    analysis->analysisDataSeriesList = analysisDataSeriesList;

    analysis->schedule.frequency = 1;
    analysis->schedule.frequencyUnit = "min";

    dataManager->add(analysis);

    service.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());


    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    app.exec();
  }

  return 0;
}
