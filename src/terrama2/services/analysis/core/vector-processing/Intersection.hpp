#ifndef __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__
#define __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__

// TerraMA2
#include "Operator.hpp"
#include "../Shared.hpp"

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace vp
        {
          class Intersection : public Operator
          {
            public:
              Intersection(terrama2::services::analysis::core::AnalysisPtr analysis,
                           terrama2::core::DataSeriesPtr monitoredDataSeries,
                           std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                           te::core::URI outputDataProviderURI);

              ~Intersection();

              virtual void execute() override;
          };
        }
      }
    }
  }
}

#endif // __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__
