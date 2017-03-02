#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
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
    outputDataSet->format.emplace("table_name", "buffer_analysis_result");

    outputDataSeries->datasetList.emplace_back(outputDataSet);


    dataManager->add(outputDataSeriesPtr);


    Analysis* analysis = new Analysis;
    AnalysisPtr analysisPtr(analysis);

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
    analysis->outputDataSeriesId = 3;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->serviceInstanceId = 1;

    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    std::shared_ptr<const terrama2::core::DataProvider> dataProviderPtr(dataProvider);
    dataProvider->name = "Provider";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/shapefile";
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;
    dataProvider->id = 1;

    dataManager->add(dataProviderPtr);


    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->dataProviderId = dataProvider->id;
    dataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");
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


    //DataProvider information
    terrama2::core::DataProvider* dataProvider2 = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProvider2Ptr(dataProvider2);
    dataProvider2->id = 2;
    dataProvider2->name = "DataProvider queimadas postgis";
    dataProvider2->uri = uri.uri();
    dataProvider2->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
    dataProvider2->dataProviderType = "POSTGIS";
    dataProvider2->active = true;

    dataManager->add(dataProvider2Ptr);

    //DataSeries information
    terrama2::core::DataSeries* occurrenceDataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr occurrenceDataSeriesPtr(occurrenceDataSeries);
    occurrenceDataSeries->id = 2;
    occurrenceDataSeries->name = "Occurrence";

    occurrenceDataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-postgis");

    occurrenceDataSeries->dataProviderId = dataProvider2Ptr->id;


    //DataSet information
    terrama2::core::DataSetOccurrence* occurrenceDataSet = new terrama2::core::DataSetOccurrence();
    occurrenceDataSet->active = true;
    occurrenceDataSet->id = 2;
    occurrenceDataSet->format.emplace("table_name", "queimadas_test_table");
    occurrenceDataSet->format.emplace("timestamp_property", "data_pas");
    occurrenceDataSet->format.emplace("geometry_property", "geom");
    occurrenceDataSet->format.emplace("timezone", "UTC-03");

    occurrenceDataSeries->datasetList.emplace_back(occurrenceDataSet);

    dataManager->add(occurrenceDataSeriesPtr);

    AnalysisDataSeries occurrenceADS;
    occurrenceADS.id = 2;
    occurrenceADS.dataSeriesId = occurrenceDataSeriesPtr->id;
    occurrenceADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysisDataSeriesList.push_back(occurrenceADS);

    analysis->analysisDataSeriesList = analysisDataSeriesList;

    analysis->schedule.frequency = 1;
    analysis->schedule.frequencyUnit = "min";

    dataManager->add(analysisPtr);

    terrama2::core::ServiceManager::getInstance().setInstanceId(1);

    Service service(dataManager);
    auto logger = std::make_shared<AnalysisLogger>();
    logger->setConnectionInfo(uri);

    service.setLogger(logger);
    service.start();
    service.addProcessToSchedule(analysisPtr);


    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    app.exec();
  }





  return 0;
}
