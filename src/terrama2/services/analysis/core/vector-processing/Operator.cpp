#include "Operator.hpp"

//
#include <terralib/dataaccess/datasource/DataSourceFactory.h>

terrama2::services::analysis::core::vp::Operator::Operator(terrama2::services::analysis::core::AnalysisPtr analysis,
                                                           terrama2::core::DataSeriesPtr monitoredDataSeries,
                                                           std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                                                           te::core::URI outputDataProviderURI)
  : analysis_(analysis),
    monitoredDataSeries_(monitoredDataSeries),
    additionalDataSeries_(additionalDataSeries),
    outputDataProviderURI_(outputDataProviderURI)
{
  dataSource_ = te::da::DataSourceFactory::make("POSTGIS", outputDataProviderURI_);

  dataSource_->open();
}

terrama2::services::analysis::core::vp::Operator::~Operator()
{
}

void terrama2::services::analysis::core::vp::Operator::setWhereCondition(const std::string& where)
{
  whereCondition_ = where;
}

std::shared_ptr<te::da::DataSet>
terrama2::services::analysis::core::vp::Operator::getResultDataSet() const
{
  return resultDataSet_;
}
