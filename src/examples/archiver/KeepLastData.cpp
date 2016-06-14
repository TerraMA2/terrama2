
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
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

int main(int argc, char* argv[])
{
  terrama2::core::initializeTerraMA();

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
    filter.lastValue = true;
    terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);
    std::cout << "\nLast data timestamp: " << accessor.lastDateTime()->toString() << std::endl;

    assert(dcpSeries->getDcpSeries().size() == 1);

    auto datasetSeries = (*dcpSeries->getDcpSeries().begin()).second;
    std::shared_ptr<te::da::DataSet> teDataSet = datasetSeries.syncDataSet->dataset();



    std::string tableName = dataSet->format["table_name"];

    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("POSTGIS"));

    std::map<std::string, std::string> connInfo {{"PG_HOST", uri.host().toStdString()},
      {"PG_PORT", std::to_string(uri.port())},
      {"PG_USER", uri.userName().toStdString()},
      {"PG_PASSWORD", uri.password().toStdString()},
      {"PG_DB_NAME", uri.path().section("/", 1, 1).toStdString()},
      {"PG_CONNECT_TIMEOUT", "4"},
      {"PG_CLIENT_ENCODING", "UTF-8"}
    };

    datasource->setConnectionInfo(connInfo);

    // RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(datasource);

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

    auto primaryKey = datasetSeries.teDataSetType->getPrimaryKey();
    assert(primaryKey);
    assert(!teDataSet->isEmpty());

    auto pkName = primaryKey->getName();
    std::string sql("DELETE FROM " +tableName+" WHERE "+pkName+" NOT IN (");

    auto pos = datasetSeries.teDataSetType->getPropertyPosition(pkName);

    teDataSet->moveBeforeFirst();
    while (teDataSet->moveNext())
    {
      sql+=teDataSet->getInt32(pos)+",";
    }
    sql.pop_back();
    sql+=")";

    transactor->execute(sql);


    std::cout << "dataset size: " << teDataSet->size() << std::endl;
  }

  terrama2::core::finalizeTerraMA();

  return 0;
}
