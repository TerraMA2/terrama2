
#include <terrama2/core/shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>

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

  std::string script = "x = countPoints(\"Occurrence\", 0.1, \"2h\", \"\")\nresult(x)";

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
  dataSet->format.emplace("mask", "UFEBRASIL.shp");
  dataSet->format.emplace("srid", "4019");
  dataSet->format.emplace("identifier", "NM_ESTADO");

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
  terrama2::core::DataSeries occurrenceSeries;
  occurrenceSeries.dataProviderId = dataProvider2.id;
  occurrenceSeries.semantics.name = "OCCURRENCE-mvf";
  occurrenceSeries.name = "Occurrence";

  occurrenceSeries.datasetList.emplace_back(new terrama2::core::DataSetOccurrence());
  //DataSet information
  std::shared_ptr<terrama2::core::DataSetOccurrence> occurrenceDataset = std::dynamic_pointer_cast<terrama2::core::DataSetOccurrence>(occurrenceSeries.datasetList.at(0));
  occurrenceDataset->active = true;
  occurrenceDataset->format.emplace("mask", "exporta_20150826_2030.csv");
  occurrenceDataset->format.emplace("timezone", "+00");
  occurrenceDataset->format.emplace("srid", "4326");

  dataProvider2.dataSeriesList.push_back(occurrenceSeries);


  terrama2::core::DataManager::getInstance().add(occurrenceSeries);

  std::vector<terrama2::core::DataSeries> staticDataList;
  staticDataList.push_back(occurrenceSeries);
  analysis.setAdditionalDataList(staticDataList);


  terrama2::analysis::core::runAnalysis(analysis);

  terrama2::analysis::core::finalize();

  terrama2::core::finalizeTerralib();

  return 0;
}
