#include "VectorProcessingContext.hpp"

terrama2::services::analysis::core::VectorProcessingContext::VectorProcessingContext(terrama2::services::analysis::core::DataManagerPtr dataManager,
                                                                                     terrama2::services::analysis::core::AnalysisPtr analysis,
                                                                                     std::shared_ptr<te::dt::TimeInstantTZ> startTime)
  : MonitoredObjectContext (dataManager, analysis, startTime),
    lastIndex_(0)
{

}

void terrama2::services::analysis::core::VectorProcessingContext::addAnalysisResultItem(const std::string& attribute, boost::any result)
{
  setAnalysisResult(++lastIndex_, attribute, result);
}
