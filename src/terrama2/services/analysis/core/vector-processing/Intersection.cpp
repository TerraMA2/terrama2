#include "Intersection.hpp"
#include "../../../../core/utility/Utils.hpp"

#include "../Analysis.hpp"

#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

terrama2::services::analysis::core::vp::Intersection::Intersection(terrama2::services::analysis::core::AnalysisPtr analysis,
                                                                   terrama2::core::DataSeriesPtr monitoredDataSeries,
                                                                   terrama2::core::DataSeriesPtr dynamicDataSeries,
                                                                   std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                                                                   te::core::URI outputDataProviderURI, std::string outputTableName)
  : Operator::Operator(analysis, monitoredDataSeries, dynamicDataSeries,
                       additionalDataSeries, outputDataProviderURI, outputTableName)
{

}

terrama2::services::analysis::core::vp::Intersection::~Intersection()
{
}

void terrama2::services::analysis::core::vp::Intersection::execute()
{
  auto transactor = dataSource_->getTransactor();

  std::string staticLayerTableName = terrama2::core::getTableNameProperty(monitoredDataSeries_->datasetList[0]);
  std::string dynamicLayerTableName = terrama2::core::getTableNameProperty(dynamicDataSeries_->datasetList[0]);

  //Attributes handler
  std::string attributes = analysis_->metadata.find("outputlayer")->second;
  std::replace(attributes.begin(), attributes.end(), '{', ' ');
  std::replace(attributes.begin(), attributes.end(), '}', ' ');

  //Date filter handler
  if((analysis_->metadata.find("startDate") != analysis_->metadata.end()) &&
     (analysis_->metadata.find("endDate") != analysis_->metadata.end()))
  {
    std::string startDate = analysis_->metadata.find("startDate")->second;
    std::string endDate = analysis_->metadata.find("endDate")->second;

    setWhereCondition(startDate+ ";" +endDate);
  }

  std::string sql = "SELECT table_name, affected_rows::double precision FROM vectorial_processing_intersection("+ std::to_string(analysis_->id) +", '" +
                     outputTableName_ + "', '" + staticLayerTableName + "', '" + dynamicLayerTableName + "', '" + attributes + "', '" + whereCondition_ + "'" + ")";


  resultDataSet_ = transactor->query(sql);
}
