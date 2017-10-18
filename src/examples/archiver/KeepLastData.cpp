
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/Raii.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataAccessorDcpPostGIS.hpp>

#include <iostream>

//QT
#include <QUrl>

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  std::cout << "NOT WORKING" << std::endl;
  return 1;

  {
te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = uri.uri();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "POSTGIS";
    dataProvider->active = true;

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
    dataSeries->semantics = semanticsManager.getSemantics("DCP-postgis");

    //DataSet information
    terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
    dataSet->active = true;
    dataSet->format.emplace("table_name", "inpe");
    dataSet->format.emplace("timestamp_property", "datetime");

    dataSeries->datasetList.emplace_back(dataSet);

    //accessing data
    terrama2::core::DataAccessorDcpPostGIS accessor(dataProviderPtr, dataSeriesPtr);
    //empty filter
    terrama2::core::Filter filter;
    filter.lastValues = std::make_shared<size_t>(1);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter, remover);
    std::cout << "\nLast data timestamp: " << accessor.lastDateTime()->toString() << std::endl;

    assert(dcpSeries->dcpSeriesMap().size() == 1);

    auto datasetSeries = (*dcpSeries->dcpSeriesMap().begin()).second;
    std::shared_ptr<te::da::DataSet> teDataSet = datasetSeries.syncDataSet->dataset();



    std::string tableName = dataSet->format["table_name"];

    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("POSTGIS", uri));

    // RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(datasource);

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
    te::da::ScopedTransaction scopedTransaction(*transactor);

    auto primaryKey = datasetSeries.teDataSetType->getPrimaryKey();
    assert(primaryKey);
    assert(!teDataSet->isEmpty());

    auto pkName = primaryKey->getName();
    std::string sql("DELETE FROM " +tableName+" WHERE "+pkName+" NOT IN (");

    auto pos = datasetSeries.teDataSetType->getPropertyPosition(pkName);

    teDataSet->moveBeforeFirst();
    while (teDataSet->moveNext())
    {
      sql+=std::to_string(teDataSet->getInt32(pos))+",";
    }
    sql.pop_back();
    sql+=")";

    transactor->execute(sql);
    scopedTransaction.commit();


    std::cout << "dataset size: " << teDataSet->size() << std::endl;
  }



  return 0;
}
