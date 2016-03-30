
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>
#include <terrama2/impl/DataAccessorGeoTiff.hpp>
#include <terrama2/core/data-access/GridSeries.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

//DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file://";
  dataProvider->uri += TERRAMA2_DATA_DIR;
  dataProvider->uri += "/geotiff";

  dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider->dataProviderType = 0;
  dataProvider->active = true;

//DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->semantics.name = "GRID-geotiff";

  terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
  dataSet->active = true;
  dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");

  dataSeries->datasetList.emplace_back(dataSet);
  
  //empty filter
  terrama2::core::Filter filter;
//accessing data
  terrama2::core::DataAccessorGeoTiff accessor(dataProviderPtr, dataSeriesPtr);
  terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter);

  assert(gridSeries->gridList().size() == 1);

  // std::shared_ptr<te::mem::DataSet> teDataSet = gridSeries->gridList().at(0).second;


  terrama2::core::finalizeTerralib();

  return 0;
}
