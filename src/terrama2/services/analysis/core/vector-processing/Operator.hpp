#ifndef __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_OPERATOR_HPP__
#define __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_OPERATOR_HPP__

// TerraMA2
#include "../../../../core/Shared.hpp"
#include "../Shared.hpp"
// STL
#include <memory>
// TerraLib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/datasource/DataSource.h>

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
          class Operator
          {
            public:
              enum OperatorType
              {
                BUFFER       = 1,
                DIFFERENCE   = 2,
                DISSOLVE     = 3,
                INTERSECTION = 4,
                MERGE        = 5,
                UNION        = 6
              };

              Operator(terrama2::services::analysis::core::AnalysisPtr analysis,
                       terrama2::core::DataSeriesPtr monitoredDataSeries,
                       terrama2::core::DataSeriesPtr dynamicDataSeries,
                       std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                       te::core::URI outputDataProviderURI,
                       std::string outputTableName);

              virtual ~Operator();

              /*!
               * \brief Execute the database operation in background and store data sets into result data set.
               *
               * \note It may throw exception related with Database.
               */
              virtual void execute() = 0;

              virtual void setWhereCondition(const std::string& where);

              virtual std::shared_ptr<te::da::DataSet> getResultDataSet() const;

            protected:
              terrama2::services::analysis::core::AnalysisPtr analysis_;
              terrama2::core::DataSeriesPtr monitoredDataSeries_;
              terrama2::core::DataSeriesPtr dynamicDataSeries_;
              std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries_;
              std::shared_ptr<te::da::DataSet> resultDataSet_;
              std::unique_ptr<te::da::DataSource> dataSource_;
              te::core::URI outputDataProviderURI_;
              std::string outputTableName_;

              std::string whereCondition_;
          }; // end class
        } // end vp
      } // end core
    } // end analysis
  } // end services
} // end terrama2

#endif // __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_OPERATOR_HPP__
