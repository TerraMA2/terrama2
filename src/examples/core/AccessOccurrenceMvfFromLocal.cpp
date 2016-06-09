
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/impl/DataAccessorOccurrenceWfp.hpp>
#include <terrama2/core/data-access/OccurrenceSeries.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerraMA();

  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/fire_system";

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "exporta_yyyyMMdd_hhmm.csv");
    dataSet->format.emplace("srid", "4326");
    dataSeries->datasetList.emplace_back(dataSet);

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);
    terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter);

    std::cout << "Last data timestamp: " << accessor.lastDateTime()->toString() << std::endl;

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

  terrama2::core::finalizeTerraMA();

  return 0;
}
