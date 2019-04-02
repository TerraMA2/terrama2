// TerraMA2
#include "Utils.hpp"

// TerraMA2 Core
#include "../Utils.hpp"
#include "../../../../core/utility/Raii.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// TerraLib Filter Enconding
#include <terralib/fe/Globals.h>
#include <terralib/fe/Literal.h>
#include <terralib/fe/PropertyName.h>

// TerraLib Styles
#include <terralib/se/FeatureTypeStyle.h>
#include <terralib/se/Rule.h>

std::string terrama2::services::view::core::vp::prepareSQLIntersection(const std::vector<std::string>& listOfIntersectionTables,
                                                                       const std::string& /*monitoredTableName*/)
{
  std::string sql = "SELECT ";
  std::string columnClause = "";

  if (listOfIntersectionTables.size() > 0)
  {
    const auto& intersectionTableName = listOfIntersectionTables[0];

    columnClause += intersectionTableName + ".monitored_id, ";
    columnClause += intersectionTableName + ".additional_id, ";
    columnClause += intersectionTableName + ".intersection_geom, ";
    columnClause += "0 as category";

    sql += columnClause + " FROM " + intersectionTableName;
  }

  for(std::size_t i = 1; i != listOfIntersectionTables.size(); ++i)
  {
    sql += " UNION SELECT ";
    const auto& intersectionTableName = listOfIntersectionTables[i];

    columnClause = intersectionTableName + ".monitored_id, ";
    columnClause += intersectionTableName + ".additional_id, ";
    columnClause += intersectionTableName + ".intersection_geom, ";
    columnClause += std::to_string(i) + " as category";

    sql += columnClause + " FROM " + intersectionTableName;
  }

  return sql;
}

std::unique_ptr<terrama2::services::view::core::View::Legend>
terrama2::services::view::core::vp::generateVectorProcessingLegend(const std::vector<std::string>& listOfIntersectionTables)
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

  return std::move(legend);
}

std::vector<std::string> terrama2::services::view::core::vp::getIntersectionTables(te::da::DataSourceTransactor* transactor,
                                                                                   const std::string& analysisTableNameResult)
{
  assert(transactor != nullptr);

  auto resultDataSet = transactor->query("SELECT table_name FROM " + analysisTableNameResult);

  std::vector<std::string> analysisTableNameList;

  while(resultDataSet->moveNext())
    analysisTableNameList.push_back(resultDataSet->getString("table_name"));

  return analysisTableNameList;
}
