
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/impl/DataAccessorOccurrenceWfp.hpp>
#include <terrama2/core/data-access/OccurrenceSeries.hpp>

#include <iostream>

//TerraLib
#include <terralib/geometry/WKTReader.h>

int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics.code = "OCCURRENCE-wfp";

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "fires.csv");
    dataSet->format.emplace("timezone", "+00");
    dataSet->format.emplace("srid", "4326");
    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2015-08-26 15:18:40-00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2015-08-26 15:18:42-00", "%Y-%m-%d %H:%M:%S%ZP");
    std::string boundingBoxWkt = "POLYGON((-74. -13., -73. -13., -73. -14., -74. -14., -74. -13.))";
    te::gm::Geometry* geometry = te::gm::WKTReader::read(boundingBoxWkt.c_str());
    geometry->setSRID(4326);
    filter.region = std::shared_ptr<te::gm::Geometry>(geometry);

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);
    terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter);

    assert(occurrenceSeries->getOccurrences().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*occurrenceSeries->getOccurrences().begin()).second.syncDataSet->dataset();

    //Print column names and types (DateTime/Double)
    int dateColumn = -1;
    int geomColumn = -1;
    std::string names, types;
    for(int i = 0; i < teDataSet->getNumProperties(); ++i)
    {
      std::string name = teDataSet->getPropertyName(i);
      names+= name + "\t";
      if(name == "data_pas")
      {
        types+= "DataTime\t";
        dateColumn = i;
      }
      else if(name == "position")
      {
        types+= "Geometry\t";
        geomColumn = i;
      }
      else
        types+= "String\t";
    }

    std::cout << names << std::endl;
    std::cout << types << std::endl;

    //Print values
    teDataSet->moveBeforeFirst();
    while(teDataSet->moveNext())
    {
      for(int i = 0; i < teDataSet->getNumProperties(); ++i)
      {

        std::cout << teDataSet->getAsString(i) << "\t";
      }
      std::cout << std::endl;
    }

    std::cout << "\nDataSet size: " << teDataSet->size() << std::endl;
  }

  terrama2::core::finalizeTerralib();

  return 0;
}
