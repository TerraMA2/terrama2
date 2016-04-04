#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpToa5.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file:///home/terrama2/Projeto/terrama2/codebase/data/pcd_toa5/toa5_origem/CPV";
  dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider->dataProviderType = 0;
  dataProvider->active = true;

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->semantics.name = "PCD-TOA5";

  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("mask", "CPV_slow_2014_01_02_1931.dat");
  dataSet->format.emplace("timezone", "+00");

  dataSeries->datasetList.emplace_back(dataSet);

  //empty filter
  terrama2::core::Filter filter;

  //accessing data
  terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

  assert(dcpSeries->getDcpSeries().size() == 1);

  std::shared_ptr<te::mem::DataSet> teDataSet = (*dcpSeries->getDcpSeries().begin()).second.teDataSet;

//Print column names and types (DateTime)
  int dateColumn = -1;
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
        std::shared_ptr<te::dt::DateTime> dateTime =  teDataSet->getDateTime(i);
        std::cout << dateTime->toString();
      }

      std::cout << "\t";
    }
    std::cout << std::endl;
  }

  terrama2::core::finalizeTerralib();

  return 0;
}
