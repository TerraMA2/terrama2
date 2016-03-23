
#include <terrama2/core/shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/AnalysisExecutor.hpp>
#include <terrama2/services/analysis/core/PythonInterpreter.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  terrama2::analysis::core::init();

  terrama2::analysis::core::Analysis analysis;

  analysis.setId(1);

  std::string script = "x = sumHistoryPCD(\"PCD-Angra\", \"pluvio\", 2, \"10h\")\nresult(x)";

  analysis.setScript(script);
  analysis.setScriptLanguage(terrama2::analysis::core::Analysis::PYTHON);
  analysis.setType(terrama2::analysis::core::Analysis::MONITORED_OBJECT_TYPE);

  terrama2::core::DataProvider dataProvider;
  dataProvider.name = "Provider";
  dataProvider.uri = "file:///Users/paulo/Workspace/data/shp";
  dataProvider.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider.dataProviderType = 0;
  dataProvider.active = true;
  dataProvider.id = 1;



  terrama2::core::DataManager::getInstance().add(dataProvider);

  terrama2::core::DataSeries dataSeries;
  dataSeries.dataProviderId = dataProvider.id;
  dataSeries.semantics.name = "STATIC_DATA-ogr";
  dataSeries.name = "Monitored Object";

  //DataSet information
  std::shared_ptr<terrama2::core::DataSet> dataSet(new terrama2::core::DataSet);
  dataSet->active = true;
  dataSet->format.emplace("mask", "afetados.shp");
  dataSet->format.emplace("srid", "4618");
  dataSet->format.emplace("identifier", "NOME");

  dataSeries.datasetList.push_back(dataSet);
  terrama2::core::DataManager::getInstance().add(dataSeries);

  analysis.setMonitoredObject(dataSeries);

  terrama2::core::DataProvider dataProvider2;
  dataProvider2.name = "Provider";
  dataProvider2.uri = "file:///Users/paulo/Workspace/data";
  dataProvider2.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider2.dataProviderType = 0;
  dataProvider2.active = true;
  dataProvider2.id = 2;


  terrama2::core::DataManager::getInstance().add(dataProvider2);

  //DataSeries information
  terrama2::core::DataSeries dcpSeries;
  dcpSeries.dataProviderId = dataProvider2.id;
  dcpSeries.semantics.name = "PCD-inpe";
  dcpSeries.name = "PCD-Angra";
  dcpSeries.id = 2;

  //DataSet information
  std::shared_ptr<terrama2::core::DataSetDcp> dcpDataset(new terrama2::core::DataSetDcp);
  dcpDataset->active = true;
  dcpDataset->format.emplace("mask", "angra.txt");
  dcpDataset->format.emplace("timezone", "-02:00");
  dcpDataset->dataSeriesId = 2;
  dcpDataset->id = 1;
  dcpDataset->position = new te::gm::Point(-44.46540, -23.00506, 4674, nullptr);


  dcpSeries.datasetList.push_back(dcpDataset);
  dataProvider2.dataSeriesList.push_back(dcpSeries);


  terrama2::core::DataManager::getInstance().add(dcpSeries);

  std::vector<terrama2::core::DataSeries> staticDataList;
  staticDataList.push_back(dcpSeries);
  analysis.setAdditionalDataList(staticDataList);

  terrama2::analysis::core::Analysis::Influence influence;
  influence.type = terrama2::analysis::core::Analysis::InfluenceType::RADIUS_CENTER;
  influence.radius = 50;
  analysis.setInfluence(dcpSeries.id, influence);

  terrama2::analysis::core::runAnalysis(analysis);

  terrama2::analysis::core::finalize();

  terrama2::core::finalizeTerralib();

  return 0;
}
