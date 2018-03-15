/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/services/analysis/core/occurrence/zonal/interval/Operator.hpp

  \brief Contains occurrence analysis operators.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_ZONAL_INTERVAL_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_ZONAL_INTERVAL_OPERATOR_HPP__

// TerraMA2
#include "../../../python/PythonInterpreter.hpp"
#include "../../../BufferMemory.hpp"
#include "../../../Analysis.hpp"

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
          namespace zonal
          {
            namespace interval
            {
              /*!
                \brief Calculates the count of occurrences in the monitored object.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param restriction SQL restriction.

                \return The number of occurrences in the monitored object.
              */
              TMANALYSISEXPORT int count(const std::string &dataSeriesName, const std::string &dateFilterBegin, const std::string &dateFilterEnd,
                        terrama2::services::analysis::core::Buffer buffer = Buffer(),
                        const std::string &restriction = "");

              /*!
                \brief Calculates the minimum value of the attribute of occurrences in the monitored object area.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param attribute Name of the attribute to be used in statistic operator.
                \param restriction SQL restriction.
                \return The minimum value of the attribute of occurrences in the monitored object area.
              */
              TMANALYSISEXPORT double min(const std::string &dataSeriesName, const std::string &attribute, const std::string &dateFilterBegin,
                         const std::string &dateFilterEnd, terrama2::services::analysis::core::Buffer buffer = Buffer(),
                         const std::string &restriction = "");

              /*!
                \brief Calculates the maximum value of the attribute of occurrences in the monitored object area.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param attribute Name of the attribute to be used in statistic operator.
                \param restriction SQL restriction.
                \return The maximum value of the attribute of occurrences in the monitored object area.
              */
              TMANALYSISEXPORT double max(const std::string &dataSeriesName, const std::string &attribute, const std::string &dateFilterBegin,
                         const std::string &dateFilterEnd, terrama2::services::analysis::core::Buffer buffer = Buffer(),
                         const std::string &restriction = "");

              /*!
                \brief Calculates the mean value of the attribute of occurrences in the monitored object area.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param attribute Name of the attribute to be used in statistic operator.
                \param restriction SQL restriction.
                \return The mean value of the attribute of occurrences in the monitored object area.
              */
              TMANALYSISEXPORT double
              mean(const std::string &dataSeriesName, const std::string &attribute, const std::string &dateFilterBegin,
                   const std::string &dateFilterEnd, terrama2::services::analysis::core::Buffer buffer = Buffer(),
                   const std::string &restriction = "");

              /*!
                \brief Calculates the median value of the attribute of occurrences in the monitored object area.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param attribute Name of the attribute to be used in statistic operator.
                \param restriction SQL restriction.
                \return The median value of the attribute of occurrences in the monitored object area.
              */
              TMANALYSISEXPORT double
              median(const std::string &dataSeriesName, const std::string &attribute, const std::string &dateFilterBegin, 
                     const std::string &dateFilterEnd, terrama2::services::analysis::core::Buffer buffer = Buffer(),
                     const std::string &restriction = "");

              /*!
                \brief Calculates the sum of values of the attribute of occurrences in the monitored object area.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param attribute Name of the attribute to be used in statistic operator.
                \param restriction SQL restriction.
                \return The sum of values of the attribute of occurrences in the monitored object area.
              */
              TMANALYSISEXPORT double sum(const std::string &dataSeriesName, const std::string &attribute, const std::string &dateFilterBegin,
                         const std::string &dateFilterEnd, terrama2::services::analysis::core::Buffer buffer = Buffer(),
                         const std::string &restriction = "");

              /*!
                \brief Calculates the starndard deviation of the attribute of occurrences in the monitored object area.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param attribute Name of the attribute to be used in statistic operator.
                \param restriction SQL restriction.
              */
              TMANALYSISEXPORT double standardDeviation(const std::string &dataSeriesName, const std::string &attribute,
                                       const std::string &dateFilterBegin, const std::string &dateFilterEnd,
                                       terrama2::services::analysis::core::Buffer buffer = Buffer(),
                                       const std::string &restriction = "");

              /*!
                \brief Calculates the variance of the attribute of occurrences in the monitored object area.

                \param dataSeriesName DataSeries name.
                \param buffer Buffer to be used in the monitored object.
                \param dateFilterBegin Begin time filter for the data.
                \param dateFilterEnd End time filter for the data.
                \param attribute Name of the attribute to be used in statistic operator.
                \param restriction SQL restriction.
              */
              TMANALYSISEXPORT double
              variance(const std::string &dataSeriesName, const std::string &attribute, const std::string &dateFilterBegin,
                       const std::string &dateFilterEnd, terrama2::services::analysis::core::Buffer buffer = Buffer(),
                       const std::string &restriction = "");

            } // end namespace interval
          }   // end namespace zonal
        }     // end namespace occurrence
      }       // end namespace core
  }           // end namespace analysis
  }           // end namespace services
}             // end namespace terrama2

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_ZONAL_INTERVAL_OPERATOR_HPP__
