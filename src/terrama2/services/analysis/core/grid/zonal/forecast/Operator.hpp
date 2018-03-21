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
  \file terrama2/services/analysis/core/grid/zonal/forecast/Operator.hpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_FORECAST_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_FORECAST_OPERATOR_HPP__

// TerraMA2
#include "../../../BufferMemory.hpp"

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
          namespace zonal
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
              TMANALYSISEXPORT double operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                  const std::string& dataSeriesName,
                                  const std::string& dateDiscardBefore,
                                  const std::string& dateDiscardAfter,
                                  terrama2::services::analysis::core::Buffer buffer = Buffer());

              TMANALYSISEXPORT int num(const std::string& dataSeriesName,
                      const std::string& dateDiscardAfter,
                      terrama2::services::analysis::core::Buffer buffer = Buffer());

              TMANALYSISEXPORT boost::python::list list( const std::string& dataSeriesName,
                                        const std::string& dateDiscardAfter,
                                        terrama2::services::analysis::core::Buffer buffer = Buffer());
              /*!
                \brief Calculates the sum of zonal grid data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double sum(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer = Buffer());

              /*!
                \brief Calculates the mean of zonal grid data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param dateFilter Time filter for the data.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double mean(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer = Buffer());

              /*!
                \brief Calculates the min of zonal grid data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.


                \return A double value with the result.
              */
              TMANALYSISEXPORT double min(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer = Buffer());

              /*!
                \brief Calculates the max of zonal grid data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dcpId Identifier of DCP dataset.
                \param buffer Buffer to be used in the monitored object.


                \return A double value with the result.
              */
              TMANALYSISEXPORT double max(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer = Buffer());

              /*!
                \brief Calculates the median of zonal grid data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.


                \return A double value with the result.
              */
              TMANALYSISEXPORT double median(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer = Buffer());

              /*!
                \brief Calculates the standard deviation of zonal grid data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double standardDeviation(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer = Buffer());

              /*!
                \brief Calculates the variance of zonal grid data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double variance(const std::string& dataSeriesName, const std::string& dateDiscardAfter, terrama2::services::analysis::core::Buffer buffer = Buffer());
            }
          } /* zonal */
        }   // end namespace grid
      }     // end namespace core
    }       // end namespace analysis
  }         // end namespace services
}           // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_FORECAST_OPERATOR_HPP__
