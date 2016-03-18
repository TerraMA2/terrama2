
#include "../core/shared.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/data-model/DataProvider.hpp"
#include "../core/data-model/DataSeries.hpp"
#include "../core/data-model/DataSetOccurrence.hpp"
#include "../impl/DataAccessorOccurrenceMvf.hpp"
#include "../core/data-access/OccurrenceSeries.hpp"

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  terrama2::core::DataProvider dataProvider;
  dataProvider.uri = "file:///home/jsimas/MyDevel/dpi/terrama2-extra/test_data/fire_system";
  dataProvider.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider.dataProviderType = 0;
  dataProvider.active = true;

  terrama2::core::DataSeries dataSeries;
  dataSeries.semantic.name = "OCCURRENCE-mvf";

  dataSeries.datasetList.emplace_back(new terrama2::core::DataSetOccurrence());
  std::shared_ptr<terrama2::core::DataSetOccurrence> dataSet = std::dynamic_pointer_cast<terrama2::core::DataSetOccurrence>(dataSeries.datasetList.at(0));
  dataSet->active = true;
  dataSet->format.emplace("mask", "fires.csv");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("srid", "4326");

  dataProvider.dataSeriesList.push_back(dataSeries);

  // angra.txt
  terrama2::core::Filter filter;

  terrama2::core::DataAccessorOccurrenceMvf accessor(dataProvider, dataSeries);
  terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter);

  assert(occurrenceSeries->occurrenceList().size() == 1);

  std::shared_ptr<te::mem::DataSet> teDataSet = occurrenceSeries->occurrenceList().at(0).second;

//Print column names and types (DateTime/Double)
  int dateColumn = -1;
  int geomColumn = -1;
  std::string names, types;
  for(int i = 0; i < teDataSet->getNumProperties(); ++i)
  {
    std::string name = teDataSet->getPropertyName(i);
    names+= name + "\t";
    if(name == "DateTime")
    {
      types+= "DataTime\t";
      dateColumn = i;
    }
    else if(name == "geom")
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
      if(teDataSet->isNull(i))
      {
        std::cout << "NULL";
        continue;
      }

      if(i == dateColumn)
      {
        std::shared_ptr<te::dt::DateTime> dateTime(teDataSet->getDateTime(i));
        std::cout << dateTime->toString();
      }
      else if(i == geomColumn)
      {
        std::shared_ptr<te::gm::Geometry> geometry(teDataSet->getGeometry(i));
        assert(geometry.get());
        std::cout << "<<geometry>>";
      }
      else
      {
        std::cout << teDataSet->getAsString(i);
      }

      std::cout << "\t";
    }
    std::cout << std::endl;
  }

  terrama2::core::finalizeTerralib();

  return 0;
}
