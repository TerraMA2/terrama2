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
  \file terrama2/services/analysis/core/grid/history/interval/Operator.hpp

  \brief Contains grid analysis operators.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_HISTORY_INTERVAL_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_HISTORY_INTERVAL_OPERATOR_HPP__

// TerraMA2
#include "../../../python/PythonInterpreter.hpp"
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
        namespace grid
        {
          namespace history
          {
            namespace interval
            {
              /*!
                \brief Implementation of grid history operator for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param statisticOperation The statistic operation chosen by the user.
                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                  const std::string& dataSeriesName,
                                  const std::string& dateFilterBegin,
                                  const std::string& dateFilterEnd,
                                  const size_t band);

              /*!
                \brief Calculates the sum of historic grid data for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double sum(const std::string& dataSeriesName,
                         const std::string& dateFilterBegin,
                         const std::string& dateFilterEnd,
                         const size_t band = 0);

              /*!
                \brief Calculates the mean of historic grid data for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double mean(const std::string& dataSeriesName, const std::string& dateFilterBegin,
                          const std::string& dateFilterEnd,
                          const size_t band = 0);

              /*!
                \brief Calculates the min of historic grid data for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double min(const std::string& dataSeriesName, const std::string& dateFilterBegin,
                         const std::string& dateFilterEnd, const size_t band = 0);

              /*!
                \brief Calculates the max of historic grid data for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double max(const std::string& dataSeriesName, const std::string& dateFilterBegin,
                         const std::string& dateFilterEnd, const size_t band = 0);

              /*!
                \brief Calculates the median of historic grid data for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double median(const std::string& dataSeriesName, const std::string& dateFilterBegin,
                            const std::string& dateFilterEnd, const size_t band = 0);

              /*!
                \brief Calculates the standard deviation of historic grid data for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double standardDeviation(const std::string& dataSeriesName, const std::string& dateFilterBegin,
                                       const std::string& dateFilterEnd, const size_t band = 0);

              /*!
                \brief Calculates the variance of historic grid data for a given time interval.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double variance(const std::string& dataSeriesName, const std::string& dateFilterBegin,
                              const std::string& dateFilterEnd, const size_t band = 0);

            } // end namespace interval
          }   // end namespace forecast
        }     // end namespace grid
      }       // end namespace core
    }         // end namespace analysis
  }           // end namespace services
}             // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_HISTORY_INTERVAL_OPERATOR_HPP__
