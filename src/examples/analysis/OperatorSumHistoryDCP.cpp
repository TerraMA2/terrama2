
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/Context.hpp>
#include <terrama2/services/analysis/core/AnalysisExecutor.hpp>
#include <terrama2/services/analysis/core/PythonInterpreter.hpp>
#include <terrama2/services/analysis/Shared.hpp>
#include <terrama2/services/analysis/core/Service.hpp>

#include <terrama2/impl/Utils.hpp>

#include <iostream>

// QT
#include <QTimer>
#include <QCoreApplication>

using namespace terrama2::services::analysis::core;

int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  terrama2::core::registerDataAccessor();

  QCoreApplication app(argc, argv);


  DataManagerPtr dataManager(new DataManager());

  Context::getInstance().setDataManager(dataManager);

  std::string script = "x = dcp.history.sum(\"DCP-Angra\", \"pluvio\", 2, \"2h\")\n"
                       "add_value(\"history_sum\",x)\n"
                       "x = dcp.history.max(\"DCP-Angra\", \"pluvio\", 2, \"2h\")\n"
                       "add_value(\"history_max\",x)\n"
                       "x = dcp.history.min(\"DCP-Angra\", \"pluvio\", 2, \"2h\")\n"
                       "add_value(\"history_min\",x)\n"
                       "x = dcp.history.mean(\"DCP-Angra\", \"pluvio\", 2, \"2h\")\n"
                       "add_value(\"history_mean\",x)\n"
                       "x = dcp.history.median(\"DCP-Angra\", \"pluvio\", 2, \"2h\")\n"
                       "add_value(\"history_median\",x)\n"
                       "x = dcp.history.standardDeviation(\"DCP-Angra\", \"pluvio\", 2, \"2h\")\n"
                       "add_value(\"history_standardDeviation\",x)\n";

  Analysis analysis;
  analysis.id = 1;
  analysis.name = "Sum History DCP";
  analysis.script = script;
  analysis.scriptLanguage = PYTHON;
  analysis.type = MONITORED_OBJECT_TYPE;
  analysis.active = false;

  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
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
  dataProvider2->dataProviderType = "FILE";
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
  dcpSeries->semantics.name = "DCP-inpe";
  dcpSeries->semantics.dataSeriesType = terrama2::core::DataSeriesSemantics::DCP;
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
  dcpDataset->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-44.46540, -23.00506, 4674, nullptr));
  dcpSeries->datasetList.push_back(dcpDatasetPtr);

  AnalysisDataSeries dcpADS;
  dcpADS.id = 2;
  dcpADS.dataSeries = dcpSeriesPtr;
  dcpADS.type = ADDITIONAL_DATA_TYPE;
  dcpADS.metadata["INFLUENCE_TYPE"] = "RADIUS_CENTER";
  dcpADS.metadata["RADIUS"] = "50";

  dataManager->add(dcpSeriesPtr);

  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(dcpADS);
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
