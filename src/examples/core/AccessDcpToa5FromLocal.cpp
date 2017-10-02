#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpToa5.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file://"+TERRAMA2_DATA_DIR+"/pcd_toa5";
  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "FILE";
  dataProvider->active = true;

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  dataSeries->semantics = semanticsManager.getSemantics("DCP-toa5");

  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("mask", "GRM_slow_2014_01_02_1713.dat");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("folder", "GRM");

  dataSeries->datasetList.emplace_back(dataSet);

  //empty filter
  terrama2::core::Filter filter;

  //accessing data
  terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);
  auto remover = std::make_shared<terrama2::core::FileRemover>();
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);

  assert(dcpSeries->dcpSeriesMap().size() == 1);

  std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->dcpSeriesMap().begin()).second.syncDataSet->dataset();

//Print column names and types (DateTime/Int/String/Double)
  size_t dateColumnDateTime = std::numeric_limits<size_t>::max();
  size_t dateColumnRecord = std::numeric_limits<size_t>::max();
  size_t dateColumnStation = std::numeric_limits<size_t>::max();

  std::string names, types;
  for(size_t i = 0; i < teDataSet->getNumProperties(); ++i)
  {
    std::string name = teDataSet->getPropertyName(i);
    names+= name + "\t";

    if (name == "RECORD")
    {
      types+= "INT32\t";
      dateColumnRecord = i;
    }

    else if (name == "Estacao_ID")
    {
      types+= "String\t";
      dateColumnStation = i;
    }

    else if(name == "DateTime")
    {
      types+= "DataTime\t";
      dateColumnDateTime = i;
    }

    else
    {
      types+= "Double\t";
    }
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
      }
      else
      {
        std::cout << teDataSet->getAsString(i);
      }

      std::cout << "\t";
    }
    std::cout << std::endl;
  }



  return 0;
}
