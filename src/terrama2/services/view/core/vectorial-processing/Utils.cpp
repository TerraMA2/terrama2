// TerraMA2
#include "Utils.hpp"

// TerraMA2 Core
#include "../../../../core/utility/Raii.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// STL
#include <cassert>

std::vector<std::string> getAnalysisTableNames(te::da::DataSourceTransactor* transactor, const std::string& tableName)
{
  assert(transactor != nullptr);

  auto resultDataSet = transactor->query("SELECT table_name FROM " + tableName);

  std::vector<std::string> analysisTableNameList;

  while(resultDataSet->moveNext())
    analysisTableNameList.push_back(resultDataSet->getString("table_name"));

  return analysisTableNameList;
}

std::string terrama2::services::view::core::vp::prepareSQLIntersection(const te::core::URI& connectionURI,
                                                                       const std::string& analysisTableNameResult,
                                                                       const std::string& /*monitoredTableName*/)
{
  auto dataSource = te::da::DataSourceFactory::make("POSTGIS", connectionURI);

  dataSource->open();

  auto transactor = dataSource->getTransactor();

  auto intersectionTableList = getAnalysisTableNames(transactor.get(), analysisTableNameResult);

  dataSource->close();

  std::string sql = "SELECT ";
  std::string columnClause = "";

  if (intersectionTableList.size() > 0)
  {
    const auto& intersectionTableName = intersectionTableList[0];

    columnClause += intersectionTableName + ".monitored_id, ";
    columnClause += intersectionTableName + ".additional_id, ";
    columnClause += intersectionTableName + ".intersection_geom, ";
    columnClause += "0 as category";

    sql += columnClause + " FROM " + intersectionTableName;
  }

  for(std::size_t i = 1; i != intersectionTableList.size(); ++i)
  {
    sql += " UNION SELECT ";
    const auto& intersectionTableName = intersectionTableList[i];

    columnClause = intersectionTableName + ".monitored_id, ";
    columnClause += intersectionTableName + ".additional_id, ";
    columnClause += intersectionTableName + ".intersection_geom, ";
    columnClause += std::to_string(i) + " as category";

    sql += columnClause + " FROM " + intersectionTableName;
  }

  return sql;
}
