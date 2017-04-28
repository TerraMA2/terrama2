
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/impl/DataAccessorGDAL.hpp>
#include <terrama2/impl/DataStoragerTiff.hpp>
#include <terrama2/core/data-access/GridSeries.hpp>

#include <iostream>

#include <terralib/rp/Functions.h>


int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("unittest", 0);

  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/geotiff";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("GRID-gdal");

    terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
    dataSet->active = true;
    dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");
    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataAccessorGDAL accessor(dataProviderPtr, dataSeriesPtr);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter,remover);

    assert(gridSeries->gridMap().size() == 1);

    //DataProvider information
    terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
    outputDataProvider->uri = "file://";
    outputDataProvider->uri += TERRAMA2_DATA_DIR;
    terrama2::core::DataStoragerTiff dataStorager(outputDataProviderPtr);

    terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
    outputDataSeries->semantics = semanticsManager.getSemantics("GRID-gdal");

    terrama2::core::DataSetGrid* outputDataSet = new terrama2::core::DataSetGrid();
    terrama2::core::DataSetGridPtr outputDataSetPtr(outputDataSet);
    outputDataSet->active = true;
    outputDataSet->format.emplace("mask", "L5219076_076%YYYY%MM%DD_r3g2b1.tif");
    outputDataSeries->datasetList.push_back(outputDataSetPtr);

    auto seriesMap = gridSeries->getSeries();
    auto series = seriesMap.begin()->second;
    dataStorager.store(series, outputDataSetPtr);
  }



  return 0;
}
