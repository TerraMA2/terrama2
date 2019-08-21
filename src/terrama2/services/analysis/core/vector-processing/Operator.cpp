#include "Operator.hpp"

//
#include <terralib/dataaccess/datasource/DataSourceFactory.h>

terrama2::services::analysis::core::vp::Operator::Operator(terrama2::services::analysis::core::AnalysisPtr analysis,
                                                           terrama2::core::DataSeriesPtr monitoredDataSeries,
                                                           terrama2::core::DataSeriesPtr dynamicDataSeries,
                                                           std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                                                           te::core::URI outputDataProviderURI,
                                                           std::string outputTableName)
  : analysis_(analysis),
    monitoredDataSeries_(monitoredDataSeries),
    dynamicDataSeries_(dynamicDataSeries),
    additionalDataSeries_(additionalDataSeries),
    outputDataProviderURI_(outputDataProviderURI),
    outputTableName_(outputTableName)
{
  dataSource_ = te::da::DataSourceFactory::make("POSTGIS", outputDataProviderURI_);

  dataSource_->open();
}

terrama2::services::analysis::core::vp::Operator::~Operator()
{
  if(dataSource_->isOpened())
    dataSource_->close();
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
