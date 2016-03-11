
#include "../core/shared.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/data-model/DataProvider.hpp"
#include "../core/data-model/DataSeries.hpp"
#include "../core/data-model/DataSetDcp.hpp"
#include "../implementation/DataAccessorDcpInpe.hpp"



int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  terrama2::core::DataProvider dataProvider;
  dataProvider.uri = "file:///home/jsimas/MyDevel/dpi/terrama2-extra/dados";
  dataProvider.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider.dataProviderType = 0;
  dataProvider.active = true;

  terrama2::core::DataSeries dataSeries;
  dataSeries.semantic = 0;

  dataSeries.datasetList.emplace_back(new terrama2::core::DataSetDcp());
  std::shared_ptr<terrama2::core::DataSetDcp> dataSet = std::dynamic_pointer_cast<terrama2::core::DataSetDcp>(dataSeries.datasetList.at(0));
  dataSet->active = true;
  dataSet->dateTimeColumnName = "N/A";
  dataSet->format.emplace("mask", "angra.txt");

  dataProvider.dataSeriesList.push_back(dataSeries);

  // angra.txt
  terrama2::core::Filter filter;

  terrama2::core::DataAccessorDcpInpe accessor(dataProvider, dataSeries);
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

  terrama2::core::DataProvider dataProvider2;
  terrama2::core::DataSeries dataSeries2;
  // terrama2::core::DcpStoragerPtr storager = Factory::getDcpStorager(dataProvider2, dataSeries2);
  // storager->store(dcpSeries);

  terrama2::core::finalizeTerralib();

  return 0;
}
