#ifndef __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__
#define __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__

// TerraMA2
#include "Operator.hpp"
#include "../Shared.hpp"

//STL
#include <string>

//QT
#include <QStringList>
#include <QString>

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
                           terrama2::core::DataSeriesPtr dynamicDataSeries,
                           std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                           te::core::URI outputDataProviderURI,
                           std::string outputTableName);

              ~Intersection() override;

              virtual void execute() override;
              virtual double getLastId() override;
              virtual std::string getDynamicTableName();
          };
        }
      }
    }
  }
}

#endif // __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__
