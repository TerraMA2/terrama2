#include "Intersection.hpp"
#include "../../../../core/utility/Utils.hpp"

#include "../Analysis.hpp"

#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

terrama2::services::analysis::core::vp::Intersection::Intersection(terrama2::services::analysis::core::AnalysisPtr analysis,
                                                                   terrama2::core::DataSeriesPtr monitoredDataSeries,
                                                                   terrama2::core::DataSeriesPtr dynamicDataSeries,
                                                                   std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                                                                   te::core::URI outputDataProviderURI)
  : Operator::Operator(analysis, monitoredDataSeries, dynamicDataSeries, additionalDataSeries, outputDataProviderURI)
{

}

terrama2::services::analysis::core::vp::Intersection::~Intersection()
{
}

void terrama2::services::analysis::core::vp::Intersection::execute()
{
  auto transactor = dataSource_->getTransactor();

  std::vector<std::string> tableNameList;
  for(auto dataSeries : additionalDataSeries_)
  {
    auto dataSet = dataSeries->datasetList[0];
    tableNameList.push_back(terrama2::core::getTableNameProperty(dataSet));
  }

  std::string queryTableNamesParameter;
  auto it = tableNameList.begin();
  if (it != tableNameList.end())
  {
    queryTableNamesParameter = "'" + *it + "'";
    ++it;
  }

  for(; it != tableNameList.end(); ++it)
    queryTableNamesParameter += ", '"+ *it +"'";

  auto monitoredTableName = terrama2::core::getTableNameProperty(monitoredDataSeries_->datasetList[0]);
  auto dynamicDataSeriesTableName = terrama2::core::getTableNameProperty(dynamicDataSeries_->datasetList[0]);

  std::string sql = "SELECT table_name, affected_rows::double precision FROM vectorial_processing_intersection("+ std::to_string(analysis_->id) +", '" +
                    monitoredTableName + "', '" + dynamicDataSeriesTableName + "', ";
  sql += "ARRAY[" + queryTableNamesParameter + "]::VARCHAR[], '" + whereCondition_ + "')";

  resultDataSet_ = transactor->query(sql);
}

