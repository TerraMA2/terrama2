
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpInpe.hpp>

#include <terrama2_config.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

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
  dataSeries->semantics.name = "DCP-inpe";


  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("mask", "30885.txt");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("folder", "");

  dataSeries->datasetList.emplace_back(dataSet);

  //empty filter
  terrama2::core::Filter filter;

  //accessing data
  terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

  assert(dcpSeries->getDcpSeries().size() == 1);

  std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->getDcpSeries().begin()).second.syncDataSet->dataset();

//Print column names and types (DateTime/Double)
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
    else
      types+= "Double\t";
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
      else
      {
        double value =  teDataSet->getDouble(i);
        std::cout << value;
      }

      std::cout << "\t";
    }
    std::cout << std::endl;
  }

  terrama2::core::finalizeTerralib();

  return 0;
}
