
#include "../core/shared.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/data-model/DataProvider.hpp"
#include "../core/data-model/DataSeries.hpp"
#include "../core/data-model/DataSetDcp.hpp"
#include "../impl/DataAccessorDcpInpe.hpp"

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  Analysis analysis;

  analysis.setId(1);

  std::string script = "x = countPoints(\"Ocorrencia\", 0.1, \"2h\", \"\")\nresult(x)";

  analysis.setScript(script);
  analysis.setScriptLanguage(Analysis::PYTHON);
  analysis.setType(Analysis::MONITORED_OBJECT_TYPE);

  terrama2::core::DataProvider dataProvider;
  dataProvider.name = "Provider";
  dataProvider.uri = "file:///Users/paulo/Workspace/data/shp";
  dataProvider.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider.dataProviderType = 0;
  dataProvider.active = true;

  terrama2::core::DataSeries dataSeries;
  dataSeries.semantics.name = "STATIC_DATA-ogr";

  dataSeries.datasetList.emplace_back(new terrama2::core::DataSet());
  //DataSet information
  std::shared_ptr<terrama2::core::DataSetDcp> dataSet = std::dynamic_pointer_cast<terrama2::core::DataSetDcp>(dataSeries.datasetList.at(0));
  dataSet->active = true;
  dataSet->format.emplace("mask", "UFEBRASIL.shp");
  dataSet->format.emplace("srid", "4019");

  terrama2::core::DataProvider dataProvider2;
  dataProvider2.name = "Provider";
  dataProvider2.uri = "file:///Users/paulo/Workspace/data/shp";
  dataProvider2.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider2.dataProviderType = 0;
  dataProvider2.active = true;


  //DataSeries information
  terrama2::core::DataSeries dcpSeries;
  dcpSeries.semantics.name = "OCCURRENCE-mvf";

  dcpSeries.datasetList.emplace_back(new terrama2::core::DataSetOccurrence());
  //DataSet information
  std::shared_ptr<terrama2::core::DataSetOccurrence> dcpDataset = std::dynamic_pointer_cast<terrama2::core::DataSetOccurrence>(dcpSeries.datasetList.at(0));
  dcpDataset->active = true;
  dcpDataset->format.emplace("mask", "exporta_20150826_2030.csv");
  dcpDataset->format.emplace("timezone", "+00");
  dcpDataset->format.emplace("srid", "4326");

  dataProvider2.dataSeriesList.push_back(dcpSeries);


  terrama2::core::finalizeTerralib();

  return 0;
}
