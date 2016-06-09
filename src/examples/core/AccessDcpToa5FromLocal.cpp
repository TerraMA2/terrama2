#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpToa5.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerraMA();

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
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

  assert(dcpSeries->getDcpSeries().size() == 1);

  std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->getDcpSeries().begin()).second.syncDataSet->dataset();

//Print column names and types (DateTime/Int/String/Double)
  int dateColumnDateTime = -1;
  int dateColumnRecord = -1;
  int dateColumnStation = -1;

  std::string names, types;
  for(int i = 0; i < teDataSet->getNumProperties(); ++i)
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
    for(int i = 0; i < teDataSet->getNumProperties(); ++i)
    {
      if(teDataSet->isNull(i))
      {
        std::cout << "NULL";
      }

      else if(i == dateColumnRecord)
      {
        int value_int = teDataSet->getInt32(i);
        std::cout << value_int;
      }

      else if(i == dateColumnStation)
      {
        std::string value_str = teDataSet->getString(i);
        std::cout << value_str;
      }

      else if(i == dateColumnDateTime)
      {
        std::shared_ptr<te::dt::DateTime> dateTime =  teDataSet->getDateTime(i);
        std::cout << dateTime->toString();
      }

      else
      {
        double value =  teDataSet->getDouble(i);
        std::cout.precision(4);
        std::cout << std::fixed << value;
      }

      std::cout << "\t";
    }
    std::cout << std::endl;
  }

  terrama2::core::finalizeTerraMA();

  return 0;
}
