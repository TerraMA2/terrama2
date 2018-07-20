
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpInpe.hpp>

#include <terrama2/Config.hpp>

#include <iostream>


int main()
{
terrama2::core::TerraMA2Init terramaRaii("example", 0);

  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file://";
  dataProvider->uri+=TERRAMA2_DATA_DIR;
  dataProvider->uri+="/PCD_serrmar_INPE";

  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  dataSeries->semantics = semanticsManager.getSemantics("DCP-inpe");


  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("mask", "30885.txt");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("folder", "");

  dataSeries->datasetList.emplace_back(dataSet);

  //empty filter
  terrama2::core::Filter filter;

  auto remover = std::make_shared<terrama2::core::FileRemover>();
  //accessing data
  terrama2::core::DataAccessorDcpInpe accessor(dataProviderPtr, dataSeriesPtr);
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);

  assert(dcpSeries->dcpSeriesMap().size() == 1);

  std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->dcpSeriesMap().begin()).second.syncDataSet->dataset();

//Print column names and types (DateTime/Double)
  size_t dateColumn = -1;
  std::string names, types;
  for(size_t i = 0; i < teDataSet->getNumProperties(); ++i)
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
    for(size_t i = 0; i < teDataSet->getNumProperties(); ++i)
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

    std::cout << "\nDataSet size: " << teDataSet->size() << std::endl;
  }



  return 0;
}
