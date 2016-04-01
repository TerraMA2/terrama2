
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/AnalysisExecutor.hpp>
#include <terrama2/services/analysis/core/PythonInterpreter.hpp>

// STL
#include <iostream>
#include <memory>

// QT
#include <QTimer>
#include <QCoreApplication>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();


  QCoreApplication app(argc, argv);

  std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManager(new terrama2::services::analysis::core::DataManager());
  terrama2::services::analysis::core::Analysis analysis;

  analysis.id = 1;
  analysis.active = true;

  std::string script = "x = countPoints(\"Occurrence\", 0.1, \"2h\", \"\")\nresult(x)";

  analysis.script = script;
  analysis.scriptLanguage = terrama2::services::analysis::core::PYTHON;
  analysis.type = terrama2::services::analysis::core::MONITORED_OBJECT_TYPE;

  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->name = "Provider";
  dataProvider->uri = "file:///Users/paulo/Workspace/data/shp";
  dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider->dataProviderType = 0;
  dataProvider->active = true;
  dataProvider->id = 1;



  dataManager->add(dataProviderPtr);


  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->dataProviderId = dataProvider->id;
  dataSeries->semantics.name = "STATIC_DATA-ogr";
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

  terrama2::services::analysis::core::AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeries = dataSeriesPtr;
  monitoredObjectADS.type = terrama2::services::analysis::core::DATASERIES_MONITORED_OBJECT_TYPE;


  terrama2::core::DataProvider* dataProvider2 = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProvider2Ptr(dataProvider2);
  dataProvider2->name = "Provider";
  dataProvider2->uri = "file:///Users/paulo/Workspace/data";
  dataProvider2->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider2->dataProviderType = 0;
  dataProvider2->active = true;
  dataProvider2->id = 2;


  dataManager->add(dataProvider2Ptr);

  //DataSeries information
  terrama2::core::DataSeries* occurrenceSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr occurrenceSeriesPtr(occurrenceSeries);
  occurrenceSeries->dataProviderId = dataProvider2->id;
  occurrenceSeries->semantics.name = "OCCURRENCE-mvf";
  occurrenceSeries->name = "Occurrence";
  occurrenceSeries->id = 2;
  occurrenceSeries->dataProviderId = 2;

  //DataSet information
  terrama2::core::DataSetOccurrence* occurrenceDataset = new terrama2::core::DataSetOccurrence();
  terrama2::core::DataSetOccurrencePtr occurrenceDatasetPtr(occurrenceDataset);
  occurrenceDataset->active = true;
  occurrenceDataset->format.emplace("mask", "exporta_20150826_2030.csv");
  occurrenceDataset->format.emplace("timezone", "+00");
  occurrenceDataset->format.emplace("srid", "4326");
  occurrenceDataset->dataSeriesId = 2;
  occurrenceDataset->id = 2;

  occurrenceSeries->datasetList.push_back(occurrenceDatasetPtr);

  dataManager->add(occurrenceSeriesPtr);

  terrama2::services::analysis::core::AnalysisDataSeries occurrenceADS;
  occurrenceADS.id = 2;
  occurrenceADS.dataSeries = occurrenceSeriesPtr;
  occurrenceADS.type = terrama2::services::analysis::core::ADDITIONAL_DATA_TYPE;

  std::vector<terrama2::services::analysis::core::AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysisDataSeriesList.push_back(occurrenceADS);

  analysis.analysisDataSeriesList = analysisDataSeriesList;

  analysis.schedule.frequency = 1;
  analysis.schedule.frequencyUnit = terrama2::core::MINUTE;


  terrama2::services::analysis::core::Service service(dataManager);
  service.start();
  service.addAnalysis(1);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();


  return 0;
}
