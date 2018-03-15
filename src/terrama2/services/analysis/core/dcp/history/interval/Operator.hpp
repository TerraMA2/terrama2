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
  \file terrama2/services/analysis/core/dcp/history/interval/Operator.hpp

  \brief Contains DCP history interval analysis operators.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_DCP_HISTORY_INTERVAL_OPERATOR_HPP__
#define __TERRAMA2_ANALYSIS_CORE_DCP_HISTORY_INTERVAL_OPERATOR_HPP__

#include "../../../utility/Utils.hpp"
#include "../../../Shared.hpp"

// STL
#include <string>

// Boost
#include <boost/python/list.hpp>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace dcp
        {
          namespace history
          {
            namespace interval
            {

              /*!
                \brief Calculates the sum of historic DCP data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.
                \param ids List of DCP identifiers.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double sum(const std::string& attribute,
                         const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                         boost::python::list ids = boost::python::list());

              /*!
                \brief Calculates the mean of historic DCP data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.
                \param ids List of DCP identifiers.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double mean(const std::string& attribute,
                          const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                          boost::python::list ids = boost::python::list());

              /*!
                \brief Calculates the min of historic DCP data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.
                \param ids List of DCP identifiers.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double min(const std::string& attribute,
                         const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                         boost::python::list ids = boost::python::list());

              /*!
                \brief Calculates the max of historic DCP data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.
                \param ids List of DCP identifiers.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double max(const std::string& attribute,
                         const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                         boost::python::list ids = boost::python::list());

              /*!
                \brief Calculates the median of historic DCP data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.
                \param ids List of DCP identifiers.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double median(const std::string& attribute,
                            const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                            boost::python::list ids = boost::python::list());

              /*!
                \brief Calculates the standard deviation of historic DCP data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.
                \param ids List of DCP identifiers.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double standardDeviation(const std::string& attribute,
                                       const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                                       boost::python::list ids = boost::python::list());

              /*!
                \brief Calculates the variance of historic DCP data.

                In case of an error or no data available it will return NAN(Not A Number).

                \param dataSeriesName DataSeries name.
                \param attribute Which DCP attribute will be used.
                \param dateFilterBegin Begin of the time interval.
                \param dateFilterEnd End of the time interval.
                \param ids List of DCP identifiers.

                \return A double value with the result.
              */
              TMANALYSISEXPORT double variance(const std::string& attribute,
                              const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                              boost::python::list ids = boost::python::list());

            } // end namespace interval
          }   // end namespace history
        }     // end namespace dcp
      }       // end namespace core
    }         // end namespace analysis
  }           // end namespace services
}             // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_DCP_HISTORY_INTERVAL_OPERATOR_HPP__
