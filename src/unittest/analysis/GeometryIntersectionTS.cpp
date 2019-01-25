#include "GeometryIntersectionTS.hpp"

// TerraMA2
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

// STL
#include <memory>


std::shared_ptr<terrama2::services::analysis::core::Service> buildService(terrama2::services::analysis::core::DataManagerPtr dataManager)
{
  auto& serviceManager = terrama2::core::ServiceManager::getInstance();

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


  // Starts the service and adds the analysis
  te::core::URI uri("");
  auto service = std::make_shared<terrama2::services::analysis::core::Service>(dataManager);
  serviceManager.setInstanceId(1);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(uri);
  serviceManager.setInstanceId(1);

  service->setLogger(logger);

  return std::move(service);
}

std::shared_ptr<terrama2::core::DataSeries> buildOutputDataSeries(const DataProviderId providerId,
                                                                  terrama2::services::analysis::core::DataManagerPtr dataManager)
{
  // Retrieves Semantics Handlerve
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  // DataSeries information - Output
  std::shared_ptr<terrama2::core::DataSeries> outputDataSeries = std::make_shared<terrama2::core::DataSeries>();
  outputDataSeries->id = 31;
  outputDataSeries->name = "Analysis Output";
  outputDataSeries->semantics = semanticsManager.getSemantics("ANALYSIS_MONITORED_OBJECT-postgis");
  outputDataSeries->dataProviderId = providerId;
  outputDataSeries->active = true;

  // DataSet information
  auto outputDataSet = std::make_shared<terrama2::core::DataSet>();
  outputDataSet->active = true;
  outputDataSet->id = 30;
  outputDataSet->dataSeriesId = outputDataSeries->id;
  outputDataSet->format.emplace("table_name", "analise_car_imovel_deter");

  outputDataSeries->datasetList.emplace_back(outputDataSet);

  return std::move(outputDataSeries);
}

void GeometryIntersectionTS::AnalysisTS()
{
  terrama2::core::registerFactories();

  {
    auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();
    // Retrieves a Analysis Service and then start
    auto& serviceManager = terrama2::core::ServiceManager::getInstance();

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


    // Starts the service and adds the analysis
    te::core::URI uri("");
    terrama2::services::analysis::core::Service service(dataManager);
    serviceManager.setInstanceId(1);
    serviceManager.setLogger(logger);
    serviceManager.setLogConnectionInfo(uri);
    serviceManager.setInstanceId(1);

    service.setLogger(logger);
    service.start();
    // Retrieves Semantics Handlerve
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

    /**
      Input Server: Data Collected, Reference File, Terrain Model Files
      DataProvider information
     */
    auto dataProvider = std::make_shared<terrama2::core::DataProvider>();
    dataProvider->id = 1;
    dataProvider->name = "Input Server";
    dataProvider->uri = "psql://"+TERRAMA2_DATABASE_USERNAME+":" + TERRAMA2_DATABASE_PASSWORD  +
                                "@" + TERRAMA2_DATABASE_HOST + ":"+ TERRAMA2_DATABASE_PORT + "/" + TERRAMA2_DATABASE_DBNAME;
    dataProvider->dataProviderType = "POSTGIS";
    dataProvider->active = true;

    dataManager->add(dataProvider);

    ////////////////////////////////////
    //  Load DataSeries to DataManager
    ///////////////////////////////////

    //CAR DataSeries information
    std::shared_ptr<terrama2::core::DataSeries> carDataSeries = std::make_shared<terrama2::core::DataSeries>();
    carDataSeries->id = 1;
    carDataSeries->name = "CAR_AREA_IMOVEL";
    carDataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-postgis");
    carDataSeries->dataProviderId = dataProvider->id;
    carDataSeries->active = true;

    //CAR DataSet info
    auto carDataSet = std::make_shared<terrama2::core::DataSet>();
    carDataSet->id = 1;
    carDataSet->active = true;
    carDataSet->format.emplace("table_name", "car_area_imovel");
    carDataSet->dataSeriesId = carDataSeries->id;
    carDataSeries->datasetList.emplace_back(carDataSet);
    dataManager->add(carDataSeries);


    // Alertas MPMT DataSeries
    std::shared_ptr<terrama2::core::DataSeries> alertaMpmtDataSeries = std::make_shared<terrama2::core::DataSeries>();
    alertaMpmtDataSeries->id = 2;
    alertaMpmtDataSeries->name = "alertas_mpmt";
    alertaMpmtDataSeries->semantics = semanticsManager.getSemantics("GEOMETRIC_OBJECT-postgis");
    alertaMpmtDataSeries->dataProviderId = dataProvider->id;
    alertaMpmtDataSeries->active = true;

    //Alertas MPMT DataSet
    auto alertaMpmtDataSet = std::make_shared<terrama2::core::DataSet>();
    alertaMpmtDataSet->id = 2;
    alertaMpmtDataSet->dataSeriesId = alertaMpmtDataSeries->id;
    alertaMpmtDataSet->active = true;
    alertaMpmtDataSet->format.emplace("table_name", "alertas_mpmt");
    alertaMpmtDataSet->format.emplace("timestamp_property", "view_date");
    alertaMpmtDataSeries->datasetList.emplace_back(alertaMpmtDataSet);

    dataManager->add(alertaMpmtDataSeries);


    // App DataSeries
    std::shared_ptr<terrama2::core::DataSeries> appDataSeries = std::make_shared<terrama2::core::DataSeries>();
    appDataSeries->id = 3;
    appDataSeries->name = "APP";
    appDataSeries->semantics = semanticsManager.getSemantics("GEOMETRIC_OBJECT-postgis");
    appDataSeries->dataProviderId = dataProvider->id;
    appDataSeries->active = true;

    //Alertas MPMT DataSet
    auto appDataSet = std::make_shared<terrama2::core::DataSet>();
    appDataSet->id = 3;
    appDataSet->dataSeriesId = appDataSeries->id;
    appDataSet->active = true;
    appDataSet->format.emplace("table_name", "app");
    appDataSeries->datasetList.emplace_back(appDataSet);

    dataManager->add(appDataSeries);

    // Reserva Legal DataSeries
    std::shared_ptr<terrama2::core::DataSeries> reservaLegalDataSeries = std::make_shared<terrama2::core::DataSeries>();
    reservaLegalDataSeries->id = 4;
    reservaLegalDataSeries->name = "Reserva Legal - MT";
    reservaLegalDataSeries->semantics = semanticsManager.getSemantics("GEOMETRIC_OBJECT-postgis");
    reservaLegalDataSeries->dataProviderId = dataProvider->id;
    reservaLegalDataSeries->active = true;

    //Alertas MPMT DataSet
    auto reservaLegalDataSet = std::make_shared<terrama2::core::DataSet>();
    reservaLegalDataSet->id = 4;
    reservaLegalDataSet->dataSeriesId = reservaLegalDataSeries->id;
    reservaLegalDataSet->active = true;
    reservaLegalDataSet->format.emplace("table_name", "reserva_legal");
    reservaLegalDataSeries->datasetList.emplace_back(reservaLegalDataSet);

    dataManager->add(reservaLegalDataSeries);

    ///////////////////////////////
    // Output data
    //////////////////////////////

    // DataSeries information - Output
    auto outputDataSeries = buildOutputDataSeries(dataProvider->id, dataManager);
    dataManager->add(outputDataSeries);

    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
    analysis->id = 9;
    analysis->name = "An_FS_SINMAP";
    analysis->script = "";
    analysis->type = terrama2::services::analysis::core::AnalysisType::VECTORIAL_PROCESSING_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = outputDataSeries->id;
    analysis->outputDataSetId = outputDataSeries->datasetList[0]->id;
    analysis->serviceInstanceId = terrama2::core::ServiceManager::getInstance().instanceId();
    dataManager->add(analysis);
    analysis->schedule.frequency = 24;
    analysis->schedule.frequencyUnit = "hours";
//  Add analysis data dynamic and static
    std::vector<terrama2::services::analysis::core::AnalysisDataSeries> analysisDataSeriesList;

    // Prepare Analysis Data Series

    //ANALYSIS DATA SERIES = CAR
    terrama2::services::analysis::core::AnalysisDataSeries car;
    car.id = 1;
    car.alias = "car";
    car.dataSeriesId = carDataSeries->id;
    car.type = terrama2::services::analysis::core::AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;

    terrama2::services::analysis::core::AnalysisDataSeries alertasmpmt;
    alertasmpmt.id = 2;
    alertasmpmt.alias = "deter";
    alertasmpmt.dataSeriesId = alertaMpmtDataSeries->id;
    alertasmpmt.type = terrama2::services::analysis::core::AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;

    //ANALYSIS DATA SERIES = APP
    terrama2::services::analysis::core::AnalysisDataSeries app;
    app.id = 3;
    app.alias = "app";
    app.dataSeriesId = appDataSeries->id;
    app.type = terrama2::services::analysis::core::AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;

    //ANALYSIS DATA SERIES = Reserva Legal
    terrama2::services::analysis::core::AnalysisDataSeries reservaLegal;
    reservaLegal.id = 4;
    reservaLegal.alias = "reserva_legal";
    reservaLegal.dataSeriesId = reservaLegalDataSeries->id;
    reservaLegal.type = terrama2::services::analysis::core::AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;

    analysisDataSeriesList.push_back(car);
    analysisDataSeriesList.push_back(alertasmpmt);
    analysisDataSeriesList.push_back(app);
    analysisDataSeriesList.push_back(reservaLegal);

    analysis->analysisDataSeriesList = analysisDataSeriesList;

    service.addToQueue(analysis, terrama2::core::TimeUtils::stringToTimestamp("2019-01-02T16:00:00.000-02:00", terrama2::core::TimeUtils::webgui_timefacet));

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(15000);
    auto exitCode = QCoreApplication::exec();

    if (exitCode != 0)
    {
      std::string errMsg = "The analysis finished with exit code '" + std::to_string(exitCode) + "'";
      QFAIL(errMsg.c_str());
    }
  }
}
