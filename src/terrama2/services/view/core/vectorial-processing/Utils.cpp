// TerraMA2
#include "Utils.hpp"

// TerraMA2 Core
#include "../Utils.hpp"
#include "../../../../core/utility/Raii.hpp"
#include "../../../../core/utility/Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSet.h>
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
  for(const auto& property: dataSetType->getProperties())
    columnClause += ", " + property->getName();
}

std::string terrama2::services::view::core::vp::prepareSQLIntersection(te::da::DataSetType* outputDataSeriesType)
{
  assert(outputDataSeriesType != nullptr);

  std::string columnClause = "";

  prepareFromClause(outputDataSeriesType, columnClause);

  QString selectAttributes = QString::fromStdString(columnClause);

  selectAttributes = selectAttributes.mid(2, selectAttributes.length() - 1);

  std::string sql = "SELECT " + selectAttributes.toStdString() +
                    " FROM " + outputDataSeriesType->getTitle();

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

  std::unique_ptr<te::da::DataSet> resultDataSet;

  std::unique_ptr<te::da::DataSetType> resultDataSetType;

  try
  {
    resultDataSet = transactor->query("SELECT DISTINCT table_name FROM " + analysisTableNameResult);
    resultDataSet->moveFirst();

    resultDataSetType = transactor->getDataSetType(resultDataSet->getString("table_name"));

    return resultDataSetType;
  }
  catch(...)
  {
    resultDataSetType.reset(0);
  }

  return resultDataSetType;
}
