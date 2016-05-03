#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/AnalysisExecutor.hpp>
#include <terrama2/services/analysis/core/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/Context.hpp>
#include <terrama2/services/analysis/Shared.hpp>

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
  terrama2::core::initializeTerralib();

  terrama2::core::registerFactories();

  QCoreApplication app(argc, argv);

  DataManagerPtr dataManager(new DataManager());
  Analysis analysis;

  analysis.id = 1;
  analysis.name = "Analysis";
  analysis.active = true;

  std::string script = "x = occurrence.count(\"Occurrence\", 0, Buffer.OBJECT_PLUS_EXTERN, \"48h\", \"\")\n"
                       "add_value(\"Count\", x)";


  analysis.script = script;
  analysis.scriptLanguage = PYTHON;
  analysis.type = MONITORED_OBJECT_TYPE;

  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  std::shared_ptr<const terrama2::core::DataProvider> dataProviderPtr(dataProvider);
  dataProvider->name = "Provider";
  dataProvider->uri = "file:///Users/paulo/Workspace/data/shp";
  dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;
  dataProvider->id = 1;

  dataManager->add(dataProviderPtr);


  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->dataProviderId = dataProvider->id;
  dataSeries->semantics.name = "STATIC_DATA-ogr";
  dataSeries->semantics.dataSeriesType = terrama2::core::DataSeriesSemantics::STATIC;
  dataSeries->name = "Monitored Object";
  dataSeries->id = 1;
  dataSeries->dataProviderId = 1;

  //DataSet information
  terrama2::core::DataSet* dataSet = new terrama2::core::DataSet;
  terrama2::core::DataSetPtr dataSetPtr(dataSet);
  dataSet->active = true;
  dataSet->format.emplace("mask", "UFEBRASIL.shp");
  dataSet->format.emplace("srid", "4019");
  dataSet->format.emplace("identifier", "NM_ESTADO");
  dataSet->id = 1;
  dataSet->dataSeriesId = 1;

  dataSeries->datasetList.push_back(dataSetPtr);
  dataManager->add(dataSeriesPtr);

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeriesId = dataSeriesPtr->id;
  monitoredObjectADS.type = DATASERIES_MONITORED_OBJECT_TYPE;


  QUrl uri;
  uri.setScheme("postgis");
  uri.setHost("localhost");
  uri.setPort(5432);
  uri.setUserName("postgres");
  uri.setPassword("postgres");
  uri.setPath("/basedeteste");


  //DataProvider information
  terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
  outputDataProvider->id = 2;
  outputDataProvider->name = "DataProvider queimadas postgis";
  outputDataProvider->uri = uri.url().toStdString();
  outputDataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  outputDataProvider->dataProviderType = "POSTGIS";
  outputDataProvider->active = true;

  dataManager->add(outputDataProviderPtr);

//DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr occurrenceDataSeriesPtr(outputDataSeries);
  outputDataSeries->id = 2;
  outputDataSeries->name = "Occurrence";
  outputDataSeries->semantics.name = "OCCURRENCE-postgis";
  outputDataSeries->dataProviderId = outputDataProviderPtr->id;


//DataSet information
  terrama2::core::DataSetOccurrence* occurrenceDataSet = new terrama2::core::DataSetOccurrence();
  occurrenceDataSet->active = true;
  occurrenceDataSet->id = 2;
  occurrenceDataSet->format.emplace("table_name", "queimadas");
  occurrenceDataSet->format.emplace("timestamp_property", "datetime");
  occurrenceDataSet->format.emplace("geometry_property", "geom");

  outputDataSeries->datasetList.emplace_back(occurrenceDataSet);

  dataManager->add(occurrenceDataSeriesPtr);

  AnalysisDataSeries occurrenceADS;
  occurrenceADS.id = 2;
  occurrenceADS.dataSeriesId = occurrenceDataSeriesPtr->id;
  occurrenceADS.type = ADDITIONAL_DATA_TYPE;

  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysisDataSeriesList.push_back(occurrenceADS);

  analysis.analysisDataSeriesList = analysisDataSeriesList;

  analysis.schedule.frequency = 1;
  analysis.schedule.frequencyUnit = "min";

  dataManager->add(analysis);

  Context::getInstance().setDataManager(dataManager);
  Service service(dataManager);
  service.start();
  service.addAnalysis(1);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();


  return 0;
}
