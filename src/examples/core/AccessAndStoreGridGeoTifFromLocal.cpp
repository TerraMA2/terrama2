
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/impl/DataAccessorGeoTif.hpp>
#include <terrama2/impl/DataStoragerTif.hpp>
#include <terrama2/core/data-access/GridSeries.hpp>

#include <iostream>

#include <terralib/rp/Functions.h>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerraMA();

  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/geotiff";

    dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.code = "GRID-geotif";

    terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
    dataSet->active = true;
    dataSet->format.emplace("mask", "hhmmyyyyMMdd_r3g2b1.tif");
//    dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataAccessorGeoTif accessor(dataProviderPtr, dataSeriesPtr);
    terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter);

    assert(gridSeries->gridList().size() == 1);

    //DataProvider information
    terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
    outputDataProvider->uri = "file://";
    outputDataProvider->uri += TERRAMA2_DATA_DIR;
    terrama2::core::DataStoragerTif dataStorager(outputDataProviderPtr);

    terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
    outputDataSeries->semantics.code = "GRID-geotif";

    terrama2::core::DataSetGrid* outputDataSet = new terrama2::core::DataSetGrid();
    terrama2::core::DataSetGridPtr outputDataSetPtr(outputDataSet);
    outputDataSet->active = true;
    outputDataSet->format.emplace("mask", "dd-MM-yy_hhmmss.tif");
    outputDataSeries->datasetList.push_back(outputDataSetPtr);

    auto seriesMap = gridSeries->getSeries();
    auto series = seriesMap.begin()->second;
    dataStorager.store(series, outputDataSetPtr);
  }

  terrama2::core::finalizeTerraMA();

  return 0;
}
