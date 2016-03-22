
#include <terrama2/core/shared.hpp>
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
  terrama2::core::DataProvider dataProvider;
  dataProvider.uri = "file://"+TERRAMA2_DATA_DIR+"/geotiff";
  dataProvider.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider.dataProviderType = 0;
  dataProvider.active = true;

//DataSeries information
  terrama2::core::DataSeries dataSeries;
  dataSeries.semantics.name = "GRID-geotiff";

  dataSeries.datasetList.emplace_back(new terrama2::core::DataSetGrid());
  //DataSet information
  std::shared_ptr<terrama2::core::DataSetGrid> dataSet = std::dynamic_pointer_cast<terrama2::core::DataSetGrid>(dataSeries.datasetList.at(0));
  dataSet->active = true;
  dataSet->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");

  dataProvider.dataSeriesList.push_back(dataSeries);

  //empty filter
  terrama2::core::Filter filter;
//accessing data
  terrama2::core::DataAccessorGeoTiff accessor(dataProvider, dataSeries);
  terrama2::core::GridSeriesPtr gridSeries = accessor.getGridSeries(filter);

  assert(gridSeries->gridList().size() == 1);

  // std::shared_ptr<te::mem::DataSet> teDataSet = gridSeries->gridList().at(0).second;


  terrama2::core::finalizeTerralib();

  return 0;
}
