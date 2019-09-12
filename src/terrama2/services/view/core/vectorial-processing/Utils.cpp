// TerraMA2
#include "Utils.hpp"

// TerraMA2 Core
#include "../Utils.hpp"
#include "../../../../core/utility/Raii.hpp"
#include "../../../../core/utility/Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// TerraLib Filter Enconding
#include <terralib/fe/Filter.h>
#include <terralib/fe/Globals.h>
#include <terralib/fe/Literal.h>
#include <terralib/fe/PropertyName.h>

// TerraLib Styles
#include <terralib/se/FeatureTypeStyle.h>
#include <terralib/se/Rule.h>

void prepareFromClause(te::da::DataSetType* dataSetType, std::string& columnClause)
{
  const auto& tableName = dataSetType->getTitle();
  const auto tableNameWithoutSchema = terrama2::core::splitString(tableName, '.')[1];

  for(const auto& property: dataSetType->getProperties())
  {
    // Skips geometry columns
    if(property->getType() != te::dt::GEOMETRY_TYPE)
      columnClause += ", " + tableName + "." + property->getName() + " AS " + tableNameWithoutSchema + "_" + property->getName();
  }
}

std::string terrama2::services::view::core::vp::prepareSQLIntersection(const std::string& tableName,
                                                                       const std::string& monitoredPrimaryKey,
                                                                       te::da::DataSetType* monitoredDataSeriesType,
                                                                       te::da::DataSetType* dynamicDataSeriesType,
                                                                       const std::string& geometryName)
{
  assert(monitoredDataSeriesType != nullptr);
  assert(dynamicDataSeriesType != nullptr);

  auto dynamicPrimaryKey = dynamicDataSeriesType->getPrimaryKey()->getProperties()[0]->getName();

  auto monitoredTableName = monitoredDataSeriesType->getTitle();
  auto dynamicTableName = dynamicDataSeriesType->getTitle();

  std::string columnClause = "monitored_id, intersect_id AS dynamic_pk";
  prepareFromClause(monitoredDataSeriesType, columnClause);
  prepareFromClause(dynamicDataSeriesType, columnClause);


  std::string fromClause = tableName + ", " + monitoredTableName + ", " + dynamicTableName;
  std::string whereClause = monitoredTableName + "." + monitoredPrimaryKey + "::VARCHAR = " +
                            tableName + ".monitored_id::VARCHAR" +
                            "   AND " +
                            dynamicTableName + "." + dynamicPrimaryKey + "::VARCHAR = " +
                            tableName + ".intersect_id::VARCHAR";

  columnClause += ", " + tableName + "." + geometryName;

  std::string sql = "SELECT " + columnClause +
                    "  FROM " + fromClause +
                    " WHERE " + whereClause;

  std::cout << sql << std::endl;

  return sql;
}

std::unique_ptr<te::se::Style>
terrama2::services::view::core::vp::generateVectorProcessingStyle(const std::string& layerName,
                                                                  const std::string& geometryColumnName)
{
  std::unique_ptr<te::se::Style> style(new te::se::FeatureTypeStyle());
  std::unique_ptr<te::se::Symbolizer> symbolizer(getSymbolizer(te::gm::MultiPolygonType, "#AAAAAA", "1"));
  std::unique_ptr<te::se::Rule> rule(new te::se::Rule);

  std::unique_ptr<te::fe::PropertyName> propertyName (new te::fe::PropertyName(geometryColumnName));

  rule->push_back(symbolizer.release());

  return style;
}

std::unique_ptr<te::da::DataSetType>
terrama2::services::view::core::vp::getIntersectionTable(te::da::DataSourceTransactor* transactor,
                                                         const std::string& analysisTableNameResult)
{
  assert(transactor != nullptr);

  auto resultDataSet = transactor->query("SELECT DISTINCT table_name FROM " + analysisTableNameResult);
  resultDataSet->moveFirst();

  return transactor->getDataSetType(resultDataSet->getString("table_name"));
}
