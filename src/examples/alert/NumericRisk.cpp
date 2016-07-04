#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/alert/core/Shared.hpp>
#include <terrama2/services/alert/core/DataManager.hpp>
#include <terrama2/services/alert/core/Alert.hpp>
#include <terrama2/services/alert/impl/ReportTxt.hpp>
#include <terrama2/services/alert/impl/Utils.hpp>
#include <terrama2/services/alert/core/RunAlert.hpp>


#include <iostream>

//QT
#include <QUrl>


terrama2::core::DataProviderPtr inputDataProvider()
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
  dataProvider->name = "PostGIS provider";
  dataProvider->uri = uri.url().toStdString();
  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "POSTGIS";
  dataProvider->active = true;
  dataProvider->id = 1;

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr inputDataSeries()
{
  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->name = "input DCP";
  dataSeries->semantics.code = "DCP-postgis";
  dataSeries->id = 1;
  dataSeries->dataProviderId = 1;

  //DataSet information
  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("table_name", "pcd");
  dataSet->format.emplace("timestamp_property", "date_time");
  dataSet->format.emplace("identifier", "id");

  dataSeries->datasetList.emplace_back(dataSet);

  return dataSeriesPtr;
}

terrama2::services::alert::core::AlertPtr newAlert()
{
  auto alert = new terrama2::services::alert::core::Alert();
  terrama2::services::alert::core::AlertPtr alertPtr(alert);

  alert->id = 1;
  alert->projectId = 1;
  alert->active = true;
  alert->name = "Example alert";

  terrama2::core::DataSeriesRisk risk;
  risk.id = 1;
  risk.dataSeriesId = 1;
  risk.name = "Wind velocity alert";
  risk.attribute = "urivelvento";

  terrama2::core::RiskLevel level1;
  level1.level = 1;
  level1.hasUpperBound = true;
  level1.upperBound = 2;
  level1.name = "low";
  risk.riskLevels.push_back(level1);

  terrama2::core::RiskLevel level2;
  level2.level = 2;
  level2.hasLowerBound = true;
  level2.lowerBound = 2;
  level2.hasUpperBound = true;
  level2.upperBound = 4;
  level2.name = "medium";
  risk.riskLevels.push_back(level2);

  terrama2::core::RiskLevel level3;
  level3.level = 3;
  level3.hasLowerBound = true;
  level3.lowerBound = 4;
  level3.name = "high";
  risk.riskLevels.push_back(level3);

  alert->risk = risk;

  std::unordered_map<std::string, std::string> reportMetadata;
  reportMetadata[terrama2::services::alert::core::ReportTags::TYPE] = "TXT";

  reportMetadata[terrama2::services::alert::core::ReportTags::TITLE] = "NUMERIC RISK EXAMPLE REPORT";
  reportMetadata[terrama2::services::alert::core::ReportTags::SUBTITLE] = "NumericRisk.cpp";
  reportMetadata[terrama2::services::alert::core::ReportTags::AUTHOR] = "Jano Simas";
  reportMetadata[terrama2::services::alert::core::ReportTags::CONTACT] = "jano.simas@funcate.org.br";
  reportMetadata[terrama2::services::alert::core::ReportTags::COPYRIGHT] = "copyright information...";
  reportMetadata[terrama2::services::alert::core::ReportTags::DESCRIPTION] = "Example generated report...";

  reportMetadata[terrama2::services::alert::core::ReportTags::DESTINATION_FOLDER] = TERRAMA2_DATA_DIR;
  reportMetadata[terrama2::services::alert::core::ReportTags::FILE_NAME] = "report.txt";

  alert->reportMetadata = reportMetadata;

  return alertPtr;
}


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerraMA();
  terrama2::core::registerFactories();
  terrama2::services::alert::core::registerFactories();

  {
    auto dataManager = std::make_shared<terrama2::services::alert::core::DataManager>();

    dataManager->add(inputDataProvider());
    dataManager->add(inputDataSeries());
    auto alert = newAlert();
    dataManager->add(alert);

    auto now = terrama2::core::TimeUtils::nowUTC();

    terrama2::services::alert::core::runAlert(std::make_pair(alert->id, now), nullptr, dataManager);
  }

  terrama2::core::finalizeTerraMA();

  return 0;
}
