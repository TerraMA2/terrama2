
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpPostGIS.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <iostream>

//QT
#include <QUrl>

int main()
{
  terrama2::core::TerraMA2Init terramaRaii("unittest", 0);

  {
    QUrl uri;
    uri.setScheme("postgis");
    uri.setHost(QString::fromStdString(TERRAMA2_DATABASE_HOST));
    uri.setPort(std::stoi(TERRAMA2_DATABASE_PORT));
    uri.setUserName(QString::fromStdString(TERRAMA2_DATABASE_USERNAME));
    uri.setPassword(QString::fromStdString(TERRAMA2_DATABASE_PASSWORD));
    uri.setPath(QString::fromStdString("/"+TERRAMA2_DATABASE_DBNAME));

    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = uri.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "POSTGIS";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics = semanticsManager.getSemantics("DCP-postgis");

    //DataSet information
    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("table_name", "pcd");
    dataSet->format.emplace("timestamp_property", "date_time");

    dataSeries->datasetList.emplace_back(dataSet);

    //accessing data
    terrama2::core::DataAccessorDcpPostGIS accessor(dataProviderPtr, dataSeriesPtr);
    //empty filter
    terrama2::core::Filter filter;
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);
    std::cout << "\nLast data timestamp: " << accessor.lastDateTime()->toString() << std::endl;

    assert(dcpSeries->dcpSeriesMap().size() == 1);

    std::shared_ptr<te::da::DataSet> teDataSet = (*dcpSeries->dcpSeriesMap().begin()).second.syncDataSet->dataset();


    //Print column names and types (DateTime/Double)
    size_t dateColumn = std::numeric_limits<size_t>::max();
    std::string names, types;
    for(size_t i = 0; i < teDataSet->getNumProperties(); ++i)
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
    }
  }



  return 0;
}
