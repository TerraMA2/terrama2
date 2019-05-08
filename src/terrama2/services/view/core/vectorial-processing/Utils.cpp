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
                                                                       te::da::DataSetType* monitoredDataSeriesType,
                                                                       te::da::DataSetType* dynamicDataSeriesType,
                                                                       const std::string& geometryName,
                                                                       te::da::DataSetType* additionalDataSeriesType)
{
  assert(monitoredDataSeriesType != nullptr);
  assert(dynamicDataSeriesType != nullptr);

  auto monitoredPrimaryKey = monitoredDataSeriesType->getPrimaryKey()->getProperties()[0]->getName();
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

  if (additionalDataSeriesType != nullptr)
  {
    // Add the columns of Additional Data Series
    prepareFromClause(additionalDataSeriesType, columnClause);

    auto additionalPrimaryKey = additionalDataSeriesType->getPrimaryKey()->getProperties()[0];
    auto additionalTableName = additionalDataSeriesType->getTitle();
    auto additionalTableWithoutSchema = terrama2::core::splitString(additionalTableName, '.')[1];
    // Add additional table name into SQL from clause
    fromClause += ", " + additionalTableName;
    // Set join condition of additional table name
    whereClause += "  AND " +
                   additionalTableName + "." + additionalPrimaryKey->getName() + "::VARCHAR = " +
                   tableName + "." + additionalTableWithoutSchema + "_pk::VARCHAR";
  }

  columnClause += ", " + tableName + "." + geometryName;

  std::string sql = "SELECT " + columnClause +
                    "  FROM " + fromClause +
                    " WHERE " + whereClause;

  std::cout << sql << std::endl;

  return sql;
}

std::unique_ptr<terrama2::services::view::core::View::Legend>
terrama2::services::view::core::vp::generateVectorProcessingLegend(const std::vector<std::string>& /*listOfIntersectionTables*/)
{
  std::unique_ptr<terrama2::services::view::core::View::Legend> legend(new terrama2::services::view::core::View::Legend);

  std::unique_ptr<te::se::Style> style(new te::se::FeatureTypeStyle());

  std::vector<View::Legend::Rule> legendRules = legend->rules;

  if(legend->operation == View::Legend::OperationType::VALUE)
  {
    if(legend->classify == View::Legend::ClassifyType::INTERVALS)
    {
      std::sort(legendRules.begin(), legendRules.end(), View::Legend::Rule::compareByNumericValue);
    }
  }

  if(legend->operation == View::Legend::OperationType::VALUE)
  {
    std::vector<std::unique_ptr<te::se::Rule> > rules;
    std::unique_ptr<te::se::Rule> ruleDefault;

    for(std::size_t i = 0; i < legendRules.size(); ++i)
    {
      auto legendRule = legendRules[i];
      std::unique_ptr<te::se::Symbolizer> symbolizer(getSymbolizer(te::gm::MultiPolygonType, legendRule.color, legendRule.opacity));

      std::unique_ptr<te::se::Rule> rule(new te::se::Rule);
      rule->push_back(symbolizer.release());
      rule->setName(new std::string(legendRule.title));

      if(legendRule.isDefault)
      {
        ruleDefault = std::move(rule);
        continue;
      }

      std::unique_ptr<te::fe::PropertyName> propertyName (new te::fe::PropertyName("category"));
      std::unique_ptr<te::fe::Literal> value (new te::fe::Literal(legendRule.value));

      // Defining OGC Style Filter
      std::unique_ptr<te::fe::Filter> filter(new te::fe::Filter);

      rule->setFilter(filter.release());

      rules.push_back(std::move(rule));
    }

    if(ruleDefault)
      style->push_back(ruleDefault.release());

    for(auto& rule : rules)
    {
      if(rule)
        style->push_back(rule.release());
    }
  }

  return legend;
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
