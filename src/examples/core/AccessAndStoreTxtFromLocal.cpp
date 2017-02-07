
// TerraMA2
#include <terrama2/Config.hpp>
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/data-access/DcpSeries.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/FileRemover.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/DataStoragerFactory.hpp>
#include <terrama2/impl/DataStoragerPostGIS.hpp>
#include <terrama2/impl/DataAccessorTxtFile.hpp>
#include <terrama2/impl/Utils.hpp>

//QT
#include <QUrl>

// STL
#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  terrama2::core::registerFactories();

  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file://";
  dataProvider->uri+=TERRAMA2_DATA_DIR;

  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  dataSeries->semantics = semanticsManager.getSemantics("CSV-generic");


  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("folder", "/PCD_serrmar_INPE/");
  dataSet->format.emplace("mask", "testeCSV.txt");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("lines_skip", "2,4");



  dataSeries->datasetList.emplace_back(dataSet);

  //empty filter
  terrama2::core::Filter filter;

  //accessing data
  auto accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProviderPtr, dataSeriesPtr);
  auto remover = std::make_shared<terrama2::core::FileRemover>();
  auto uriMap = accessor->getFiles(filter, remover);

  auto dataMap = accessor->getSeries(uriMap, filter, remover);

  terrama2::core::DcpSeriesPtr dcpSeries = std::make_shared<terrama2::core::DcpSeries>();
  dcpSeries->addDcpSeries(dataMap);

  assert(dcpSeries->dcpSeriesMap().size() == 1);

  QUrl uri;
  uri.setScheme("postgis");
  uri.setHost("localhost");
  uri.setPort(5432);
  uri.setUserName("postgres");
  uri.setPassword("postgres");
  uri.setPath("/terrama2");

  //DataProvider information
  terrama2::core::DataProvider* dataProviderPostGIS = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPostGISPtr(dataProviderPostGIS);
  dataProviderPostGIS->uri = uri.url().toStdString();

  dataProviderPostGIS->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
  dataProviderPostGIS->dataProviderType = "POSTGIS";
  dataProviderPostGIS->active = true;

  //DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
  outputDataSeries->semantics = semanticsManager.getSemantics("DCP-postgis");

  terrama2::core::DataSetDcp* dataSetOutput = new terrama2::core::DataSetDcp();
  terrama2::core::DataSetPtr dataSetOutputPtr(dataSetOutput);
  dataSetOutput->active = true;
  dataSetOutput->format.emplace("table_name", "inpe");
  dataSetOutput->format.emplace("timestamp_column", "DateTime");

  auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(outputDataSeriesPtr->semantics.dataFormat, dataProviderPostGISPtr);

  for(auto& item : dataMap)
  {
    dataStorager->store( item.second, dataSetOutputPtr);
  }

  return 0;
}
