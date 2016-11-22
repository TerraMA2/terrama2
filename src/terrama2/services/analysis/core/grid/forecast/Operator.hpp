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
  \file terrama2/services/analysis/core/grid/forecast/Operator.hpp

  \brief Contains grid forecast analysis operators.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_FORECAST_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_FORECAST_OPERATOR_HPP__

// TerraMA2
#include "../../python/PythonInterpreter.hpp"
#include "../../Analysis.hpp"

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
          namespace forecast
          {
            /*!
              \brief Implementation of grid forecast operator.

              In case of an error or no data available it will return NAN(Not A Number).

              \param statisticOperation The statistic operation chosen by the user.
              \param dataSeriesName DataSeries name.
              \param dateFilter Time filter for the data.

              \return A double value with the result.
            */
            double operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                const std::string& dataSeriesName, const std::string& dateFilterEnd, const size_t band);

            /*!
              \brief Calculates the sum of forecast grid data.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param dateFilter Time filter for the data.

              \return A double value with the result.
            */
            double sum(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band = 0);

            /*!
              \brief Calculates the mean of forecast grid data.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param dateFilter Time filter for the data.

              \return A double value with the result.
            */
            double mean(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band = 0);

            /*!
              \brief Calculates the min of forecast grid data.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param dateFilter Time filter for the data.

              \return A double value with the result.
            */
            double min(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band = 0);

            /*!
              \brief Calculates the max of forecast grid data.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param dcpId Identifier of DCP dataset.
              \param buffer Buffer to be used in the monitored object.
              \param dateFilter Time filter for the data.

              \return A double value with the result.
            */
            double max(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band = 0);

            /*!
              \brief Calculates the median of forecast grid data.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param dateFilter Time filter for the data.

              \return A double value with the result.
            */
            double median(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band = 0);

            /*!
              \brief Calculates the standard deviation of forecast grid data.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param dateFilter Time filter for the data.
              \return A double value with the result.
            */
            double standardDeviation(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band = 0);

            /*!
              \brief Calculates the variance of forecast grid data.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param dateFilter Time filter for the data.
              \return A double value with the result.
            */
            double variance(const std::string& dataSeriesName, const std::string& dateFilter, const size_t band = 0);

          } // end namespace forecast
        }   // end namespace grid
      }     // end namespace core
    }       // end namespace analysis
  }         // end namespace services
}           // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_FORECAST_OPERATOR_HPP__
