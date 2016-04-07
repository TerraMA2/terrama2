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
  dataProvider->uri = "file://";
  dataProvider->uri+=TERRAMA2_DATA_DIR;
  //dataProvider->uri+="/pcd_toa5/GRM";
  dataProvider->uri+="/pcd_toa5";
  dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider->dataProviderType = 0;
  dataProvider->active = true;

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->semantics.name = "DCP-toa5";

  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("mask", "GRM_slow_2014_01_02_1713.dat");
  dataSet->format.emplace("timezone", "+00");
  dataSet->format.emplace("folder", "/GRM");

  dataSeries->datasetList.emplace_back(dataSet);

  //empty filter
  terrama2::core::Filter filter;

  //accessing data
  terrama2::core::DataAccessorDcpToa5 accessor(dataProviderPtr, dataSeriesPtr);
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

  assert(dcpSeries->getDcpSeries().size() == 1);

  std::shared_ptr<te::mem::DataSet> teDataSet = (*dcpSeries->getDcpSeries().begin()).second.teDataSet;

//Print column names and types (DateTime/Int/String/Double)
  int dateColumnDateTime = -1;
  int dateColumnRecord = -1;
  int dateColumnStation = -1;

  std::string names, types;
  for(int i = 0; i < teDataSet->getNumProperties(); ++i)
  {
    bool isdouble = true;
    std::string name = teDataSet->getPropertyName(i);
    names+= name + "\t";

    if (name == "RECORD")
    {
      types+= "INT32\t";
      dateColumnRecord = i;
      isdouble = false;
    }

    if (name == "Estacao_ID")
    {
      types+= "String\t";
      dateColumnStation = i;
      isdouble = false;
    }

    if(name == "DateTime")
    {
      types+= "DataTime\t";
      dateColumnDateTime = i;
      isdouble = false;
    }

    if (isdouble)
    {
      types+= "Double\t";
      isdouble = true;
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
      bool isdouble = true;
      if(teDataSet->isNull(i))
      {
        isdouble = false;
        std::cout << "NULL";
      }

      if(i == dateColumnRecord)
      {
        int value_int = teDataSet->getInt32(i);
        std::cout << value_int;
        isdouble = false;
      }

      if(i == dateColumnStation)
      {
        std::string value_str = teDataSet->getString(i);
        std::cout << value_str;
        isdouble = false;
      }

      if(i == dateColumnDateTime)
      {
        std::shared_ptr<te::dt::DateTime> dateTime =  teDataSet->getDateTime(i);
        std::cout << dateTime->toString();
        isdouble = false;
      }

      if(isdouble)
      {
        double value =  teDataSet->getDouble(i);
        std::cout << value;
        isdouble = true;
      }

      std::cout << "\t";
    }
    std::cout << std::endl;
  }

  terrama2::core::finalizeTerralib();

  return 0;
}
