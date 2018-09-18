#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_OPERATOR_HPP__


// TerraMA2
#include "../python/PythonInterpreter.hpp"
#include "../Analysis.hpp"

// STL
#include <string>


namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace occurrence
        {
          /*!
          \brief Calculates the all statistics of the attribute of occurrences in the monitored object area.

          \param statisticOperation The statistic operation called by the script.
          \param dataSeriesName DataSeries name.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilterBegin Begin time filter for the data.
          \param dateFilterEnd End time filter for the data.
          \param attribute Name of the attribute to be used in statistic operator.
          \param aggregationBuffer Buffer configuration to be used to aggregate occurrences in the same area.
          \param restriction SQL restriction.
          \return The result of the selected operation.
        */
          TMANALYSISEXPORT double operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                               const std::string & dataSeriesName,
                                               const std::string & dateFilterBegin,
                                               const std::string & dateFilterEnd,
                                               const std::string & attribute,
                                               terrama2::services::analysis::core::StatisticOperation aggregationStatisticOperation,
                                               const std::string& additionalIdentifier = "",
                                               const std::string& monitoredIdentifier = "",
                                               const std::string& restriction = "");

          /*!
            \brief Calculates the count of occurrences in the monitored object.

            \param dataSeriesName DataSeries name.
            \param buffer Buffer to be used in the monitored object.
            \param dateFilter Time filter for the data.
            \param restriction SQL restriction.

            \return The number of occurrences in the monitored object.
          */
          TMANALYSISEXPORT double count(const std::string& dataSeriesName,
                                        const std::string& dateFilter,
                                        const std::string& monitoredIdentifier = "",
                                        const std::string& additionalIdentifier = "",
                                        const std::string& restriction= "");
        }
      }
    }
  }
}

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_OPERATOR_HPP__
