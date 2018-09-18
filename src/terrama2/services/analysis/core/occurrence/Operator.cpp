#include "Operator.hpp"

double
terrama2::services::analysis::core::occurrence::operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                             const std::string& dataSeriesName,
                                                             const std::string& dateFilterBegin,
                                                             const std::string& dateFilterEnd,
                                                             const std::string& attribute,
                                                             terrama2::services::analysis::core::StatisticOperation aggregationStatisticOperation,
                                                             const std::string& monitoredIdentifier,
                                                             const std::string& additionalIdentifier,
                                                             const std::string& restriction)
{

}

double
terrama2::services::analysis::core::occurrence::count(const std::string& dataSeriesName,
                                                      const std::string& dateFilter,
                                                      const std::string& monitoredIdentifier,
                                                      const std::string& additionalIdentifier,
                                                      const std::string& restriction)
{
  return operatorImpl(StatisticOperation::COUNT,
                      dataSeriesName,
                      dateFilter,
                      "0s",
                      "",
                      StatisticOperation::INVALID,
                      monitoredIdentifier,
                      additionalIdentifier,
                      restriction);
}
