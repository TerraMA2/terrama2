
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/core/data-access/OccurrenceSeries.hpp>
#include <terrama2/impl/DataAccessorOccurrencePostGIS.hpp>
#include <terrama2/Config.hpp>

#include <iostream>

//QT
#include <QUrl>

//TerraLib
#include <terralib/geometry/WKTReader.h>

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  {
    QUrl uri;
    uri.setScheme("postgis");
    uri.setHost(QString::fromStdString(TERRAMA2_DATABASE_HOST));
    uri.setPort(std::stoi(TERRAMA2_DATABASE_PORT));
    uri.setUserName(QString::fromStdString(TERRAMA2_DATABASE_USERNAME));
    uri.setPassword(QString::fromStdString(TERRAMA2_DATABASE_PASSWORD));
    uri.setPath(QString::fromStdString("/"+TERRAMA2_DATABASE_DBNAME));

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
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-postgis");

    //DataSet information
    terrama2::core::DataSetOccurrence* dataSet = new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("table_name", "fires");
    dataSet->format.emplace("timestamp_property", "data_pas");
    dataSet->format.emplace("geometry_property", "geom");

    dataSeries->datasetList.emplace_back(dataSet);

    //accessing data
    terrama2::core::DataAccessorOccurrencePostGIS accessor(dataProviderPtr, dataSeriesPtr);
    //empty filter
    terrama2::core::Filter filter;
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("+00"));

    filter.lastValues = std::make_shared<size_t>(1);

    std::string boundingBoxWkt = "POLYGON((-51.11 -17.74, -41.11 -17.74, -41.11 -20.83, -51.11 -20.83, -51.11 -17.74))";
    te::gm::Geometry* geometry = te::gm::WKTReader::read(boundingBoxWkt.c_str());
    geometry->setSRID(4326);

    filter.region = std::shared_ptr<te::gm::Geometry>(geometry);

    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter, remover);

    assert(occurrenceSeries->occurrencesMap().size() == 1);

    auto teDataSet = (*occurrenceSeries->occurrencesMap().begin()).second.syncDataSet->dataset();


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
      else if(name == "geom")
      {
        types+= "Geometry\t";
        geomColumn = i;
      }
      else
        types+= "Int\t";
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
        else if(i == geomColumn)
        {
          std::cout << "<<Geometry>>";
        }
        else
        {
          std::cout << teDataSet->getInt32(i);
        }

        std::cout << "\t";
      }
      std::cout << std::endl;
    }

    std::cout << "\ndataset size: " << teDataSet->size() << std::endl;
  }



  return 0;
}
