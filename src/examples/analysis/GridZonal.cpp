
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
#include <terrama2/core/data-model/DataSetGrid.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Shared.hpp>

#include <terrama2/impl/Utils.hpp>

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

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);
  serviceManager.setLogConnectionInfo(uri);

  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

  {
    QCoreApplication app(argc, argv);

    DataManagerPtr dataManager(new DataManager());

    // DataProvider information
    terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
    outputDataProvider->id = 3;
    outputDataProvider->name = "DataProvider postgis";
    outputDataProvider->uri = uri.uri();
    outputDataProvider->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
    outputDataProvider->dataProviderType = "POSTGIS";
    outputDataProvider->active = true;


    dataManager->add(outputDataProviderPtr);

    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

    // DataSeries information
    terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
    outputDataSeries->id = 3;
    outputDataSeries->name = "Analysis result";
    outputDataSeries->semantics = semanticsManager.getSemantics("ANALYSIS_MONITORED_OBJECT-postgis");
    outputDataSeries->dataProviderId = outputDataProviderPtr->id;


    // DataSet information
    terrama2::core::DataSet* outputDataSet = new terrama2::core::DataSet();
    outputDataSet->active = true;
    outputDataSet->id = 2;
    outputDataSet->dataSeriesId = outputDataSeries->id;
    outputDataSet->format.emplace("table_name", "zonal_analysis_result");

    outputDataSeries->datasetList.emplace_back(outputDataSet);


    dataManager->add(outputDataSeriesPtr);


    Analysis* analysis = new Analysis;
    AnalysisPtr analysisPtr(analysis);

    analysis->id = 1;
    analysis->name = "Analysis";
    analysis->active = true;

    std::string script = "x = grid.zonal.count(\"geotiff 1\")\n"
                         "add_value(\"min\", x)\n"
                         "return";


    analysis->script = script;
    analysis->outputDataSeriesId = 3;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->serviceInstanceId = 1;

    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProvider1Ptr(dataProvider);
    dataProvider->name = "Provider";
    dataProvider->uri += TERRAMA2_DATA_DIR + "/shapefile";
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;
    dataProvider->id = 1;

    dataManager->add(dataProvider1Ptr);


    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->dataProviderId = dataProvider->id;
    dataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");
    dataSeries->semantics.dataSeriesType = terrama2::core::DataSeriesType::GEOMETRIC_OBJECT;
    dataSeries->name = "Monitored Object";
    dataSeries->id = 1;
    dataSeries->dataProviderId = 1;

    //DataSet information
    terrama2::core::DataSet* dataSet = new terrama2::core::DataSet;
    terrama2::core::DataSetPtr dataSetPtr(dataSet);
    dataSet->active = true;
    dataSet->format.emplace("mask", "estados_2010.shp");
    dataSet->format.emplace("srid", "4326");
    dataSet->id = 1;
    dataSet->dataSeriesId = 1;

    dataSeries->datasetList.push_back(dataSetPtr);
    dataManager->add(dataSeriesPtr);

    AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeriesPtr->id;
    monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    monitoredObjectADS.metadata["identifier"] = "nome";


    // DataProvider information
    terrama2::core::DataProvider* dataProvider2 = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProvider2Ptr(dataProvider2);
    dataProvider2->uri = "file://"+TERRAMA2_DATA_DIR+"/geotiff";

    dataProvider2->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider2->dataProviderType = "FILE";
    dataProvider2->active = true;
    dataProvider2->id = 2;
    dataProvider2->name = "Local Geotiff";

    dataManager->add(dataProvider2Ptr);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data Series 2
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    terrama2::core::DataSeries* dataSeries2 = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeries2Ptr(dataSeries2);
    dataSeries2->semantics = semanticsManager.getSemantics("GRID-static_geotiff");
    dataSeries2->name = "geotiff 1";
    dataSeries2->id = 2;
    dataSeries2->dataProviderId = 2;

    terrama2::core::DataSetGrid* dataSet1 = new terrama2::core::DataSetGrid();
    dataSet1->active = true;
    dataSet1->format.emplace("mask", "Spot_Vegetacao_Jul2001_SP.tif");
    dataSet1->id = 2;

    dataSeries2->datasetList.emplace_back(dataSet1);

    AnalysisDataSeries gridADS1;
    gridADS1.id = 2;
    gridADS1.dataSeriesId = dataSeries2Ptr->id;
    gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    dataManager->add(dataSeries2Ptr);

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysisDataSeriesList.push_back(gridADS1);

    analysis->analysisDataSeriesList = analysisDataSeriesList;


    AnalysisOutputGrid* outputGrid = new AnalysisOutputGrid();
    AnalysisOutputGridPtr outputGridPtr(outputGrid);

    outputGrid->analysisId = 1;
    outputGrid->interpolationMethod = InterpolationMethod::BILINEAR;
    outputGrid->interestAreaType = InterestAreaType::SAME_FROM_DATASERIES;
    outputGrid->interestAreaDataSeriesId = 1;
    outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
    outputGrid->resolutionDataSeriesId = 1;
    outputGrid->interpolationDummy = -1;

    analysis->outputGridPtr = outputGridPtr;

    dataManager->add(analysisPtr);

    terrama2::core::ServiceManager::getInstance().setInstanceId(1);

    Service service(dataManager);
    auto logger = std::make_shared<AnalysisLogger>();
    logger->setConnectionInfo(uri);

    service.setLogger(logger);
    service.start();
    service.addToQueue(analysisPtr->id, terrama2::core::TimeUtils::nowUTC());


    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    app.exec();
  }





  return 0;
}
