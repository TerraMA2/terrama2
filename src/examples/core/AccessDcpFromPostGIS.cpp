
#include "../core/shared.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/data-model/DataProvider.hpp"
#include "../core/data-model/DataSeries.hpp"
#include "../core/data-model/DataSetDcp.hpp"
#include "../impl/DataAccessorDcpPostGIS.hpp"

#include <iostream>

//QT
#include <QUrl>

int main(int argc, char* argv[])
{
  terrama2::core::initializeTerralib();

  QUrl uri;
  uri.setScheme("postgis");
  uri.setHost("localhost");
  uri.setPort(5432);
  uri.setUserName("postgres");
  uri.setPassword("postgres");
  uri.setPath("/basedeteste");

//DataProvider information
  terrama2::core::DataProvider dataProvider;
  dataProvider.uri = uri.url().toStdString();
  dataProvider.intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
  dataProvider.dataProviderType = 0;
  dataProvider.active = true;

//DataSeries information
  terrama2::core::DataSeries dataSeries;
  dataSeries.semantics.name = "PCD-postgis";

  dataSeries.datasetList.emplace_back(new terrama2::core::DataSetDcp());
  //DataSet information
  std::shared_ptr<terrama2::core::DataSetDcp> dataSet = std::dynamic_pointer_cast<terrama2::core::DataSetDcp>(dataSeries.datasetList.at(0));
  dataSet->active = true;
  dataSet->format.emplace("table_name", "pcd");
  dataSet->format.emplace("timestamp_column", "date_time");

  dataProvider.dataSeriesList.push_back(dataSeries);

  //empty filter
  terrama2::core::Filter filter;

//accessing data
  terrama2::core::DataAccessorDcpPostGIS accessor(dataProvider, dataSeries);
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

  assert(dcpSeries->dcpList().size() == 1);

  std::shared_ptr<te::mem::DataSet> teDataSet = dcpSeries->dcpList().at(0).second;


//Print column names and types (DateTime/Double)
  int dateColumn = -1;
  std::string names, types;
  for(int i = 0; i < teDataSet->getNumProperties(); ++i)
  {
    std::string name = teDataSet->getPropertyName(i);
    names+= name + "\t";
    if(name == "date_time")
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
