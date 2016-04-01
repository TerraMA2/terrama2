
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/AnalysisExecutor.hpp>
#include <terrama2/services/analysis/core/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/Shared.hpp>
#include <terrama2/services/analysis/core/Service.hpp>

#include <iostream>

// QT
#include <QTimer>
#include <QCoreApplication>

using namespace terrama2::services::analysis::core;

int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  QCoreApplication app(argc, argv);

  Analysis analysis;

  DataManagerPtr dataManager(new DataManager());

  analysis.id = 1;

  std::string script = "x = sumHistoryPCD(\"PCD-Angra\", \"pluvio\", 2, \"10h\")\nresult(x)";

  analysis.script = script;
  analysis.scriptLanguage = PYTHON;
  analysis.type = MONITORED_OBJECT_TYPE;

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
  dataSet->format.emplace("mask", "afetados.shp");
  dataSet->format.emplace("srid", "4618");
  dataSet->format.emplace("identifier", "NOME");
  dataSet->id = 1;

  dataSeries->datasetList.push_back(dataSetPtr);
  dataManager->add(dataSeriesPtr);


  terrama2::core::DataProvider* dataProvider2 = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProvider2Ptr(dataProvider2);
  dataProvider2->name = "Provider";
  dataProvider2->uri = "file:///Users/paulo/Workspace/data";
  dataProvider2->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider2->dataProviderType = 0;
  dataProvider2->active = true;
  dataProvider2->id = 2;


  dataManager->add(dataProvider2Ptr);

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeries = dataSeriesPtr;
  monitoredObjectADS.type = DATASERIES_MONITORED_OBJECT_TYPE;


  //DataSeries information
  terrama2::core::DataSeries* dcpSeries = new terrama2::core::DataSeries;
  terrama2::core::DataSeriesPtr dcpSeriesPtr(dcpSeries);
  dcpSeries->dataProviderId = dataProvider2->id;
  dcpSeries->semantics.name = "PCD-inpe";
  dcpSeries->name = "PCD-Angra";
  dcpSeries->id = 2;
  dcpSeries->dataProviderId = 2;

  //DataSet information
  terrama2::core::DataSetDcp* dcpDataset = new terrama2::core::DataSetDcp;
  terrama2::core::DataSetDcpPtr dcpDatasetPtr(dcpDataset);
  dcpDataset->active = true;
  dcpDataset->format.emplace("mask", "angra.txt");
  dcpDataset->format.emplace("timezone", "-02:00");
  dcpDataset->dataSeriesId = 2;
  dcpDataset->id = 2;
  dcpDataset->position = new te::gm::Point(-44.46540, -23.00506, 4674, nullptr);
  dcpSeries->datasetList.push_back(dcpDatasetPtr);

  AnalysisDataSeries pcdADS;
  pcdADS.id = 2;
  pcdADS.dataSeries = dcpSeriesPtr;
  pcdADS.type = ADDITIONAL_DATA_TYPE;
  pcdADS.metadata["INFLUENCE_TYPE"] = "RADIUS_CENTER";
  pcdADS.metadata["RADIUS"] = "50";

  dataManager->add(dcpSeriesPtr);

  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(pcdADS);
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysis.analysisDataSeriesList = analysisDataSeriesList;

  dataManager->add(analysis);


  Service service(dataManager);
  service.start();
  service.addAnalysis(1);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();


  terrama2::core::finalizeTerralib();

  return 0;
}
