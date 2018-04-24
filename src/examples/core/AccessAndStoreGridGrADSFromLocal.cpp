
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/impl/DataAccessorGrADS.hpp>
#include <terrama2/impl/DataStoragerTiff.hpp>
#include <terrama2/core/data-access/GridSeries.hpp>

#include <iostream>

#include <terralib/rp/Functions.h>


int main(int, char**)
{
    terrama2::core::TerraMA2Init terramaRaii("example", 0);

  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/grads";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("GRID-grads");

    terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
    dataSet->active = true;
    dataSet->format.emplace("ctl_filename", "Prec_BRAMS05km.ctl");
    dataSet->format.emplace("bytes_before", "4");
    dataSet->format.emplace("bytes_after", "4");
    dataSet->format.emplace("srid", "4326");
    dataSet->format.emplace("timezone", "+00");

    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;

    //accessing data
    terrama2::core::DataAccessorGrADS accessor(dataProviderPtr, dataSeriesPtr);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter, remover);

    //DataProvider information
    terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
    outputDataProvider->uri = "file://";
    outputDataProvider->uri += TERRAMA2_DATA_DIR;
    outputDataProvider->uri += "/grads";

    terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
    outputDataSeries->semantics = semanticsManager.getSemantics("GRID-gdal");

    terrama2::core::DataStoragerTiff dataStorager(outputDataSeriesPtr, outputDataProviderPtr);

    terrama2::core::DataSetGrid* outputDataSet = new terrama2::core::DataSetGrid();
    terrama2::core::DataSetGridPtr outputDataSetPtr(outputDataSet);
    outputDataSet->active = true;
    outputDataSet->format.emplace("mask", "Prec_BRAMS05km.%YYYY%MM%DD_%hh_mm.tif");
    outputDataSeries->datasetList.push_back(outputDataSetPtr);

    auto seriesMap = gridSeries->getSeries();
    auto series = seriesMap.begin()->second;
    dataStorager.store(series, outputDataSetPtr);
  }



  return 0;
}
