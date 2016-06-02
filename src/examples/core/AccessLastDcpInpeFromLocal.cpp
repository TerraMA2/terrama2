
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpInpe.hpp>
#include <terrama2/impl/DataStoragerPostGis.hpp>

#include <terrama2/Config.hpp>

#include <iostream>

//QT
#include <QUrl>

int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri+=TERRAMA2_DATA_DIR;
    dataProvider->uri+="/PCD_serrmar_INPE";

    dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.code = "DCP-inpe";


    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("mask", "30885.txt");
    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    filter.lastValue = true;

    //accessing data
    terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

    assert(dcpSeries->getDcpSeries().size() == 1);
    auto series = dcpSeries->getSeries().begin()->second;
    auto teDataSet = series.syncDataSet->dataset();

    std::cout << "dataset size: " << teDataSet->size() << std::endl;
  }

  terrama2::core::finalizeTerralib();

  return 0;
}
