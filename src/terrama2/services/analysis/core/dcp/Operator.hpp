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
  \file terrama2/services/analysis/core/dcp/Operator.hpp

  \brief Contains DCP analysis operators.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_DCP_OPERATOR_HPP__
#define __TERRAMA2_ANALYSIS_CORE_DCP_OPERATOR_HPP__

// TerraMA2
#include "../python/PythonInterpreter.hpp"
#include "../BufferMemory.hpp"
#include "../Analysis.hpp"
#include "../Shared.hpp"

// STL
#include <string>

// Boost
#include <boost/python.hpp>

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

          /*!
            \brief Implementation of operator for latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

            In case of an error or no data available it will return NAN(Not A Number).

            \param statisticOperation The statistic operation chosen by the user.
            \param dataSeriesName DataSeries name.
            \param attribute Which DCP attribute will be used.
            \param ids A set of identifiers of DataSet.

            \return A double value with the result.
          */
          double operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                              const std::string& attribute,
                              boost::python::list pcds, const std::string& dateFilterBegin = "",
                              const std::string& dateFilterEnd = "");

          double value(const std::string& attribute);

          /*!
            \brief It returns the number of DCPs that have influence over the current monitored object.

            In case of an error or no data available it will return NAN(Not A Number).

            \param dataSeriesName DataSeries name.
            \param buffer Buffer to be used in the monitored object.

            \return The number of DCP that have influence over the current monitored object.
          */
          int count(terrama2::services::analysis::core::Buffer buffer = terrama2::services::analysis::core::Buffer());

          /*!
            \brief Calculates the minimum value of the latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

            In case of an error or no data available it will return NAN(Not A Number).

            \param statisticOperation The statistic operation chosen by the user.
            \param dataSeriesName DataSeries name.
            \param attribute Which DCP attribute will be used.
            \param ids A set of identifiers of DataSet.

            \return A double with the minimum value.
          */
          double min(const std::string& attribute,
                     boost::python::list ids = boost::python::list());

          /*!
            \brief Calculates the maximum value of the latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

            In case of an error or no data available it will return NAN(Not A Number).

            \param dataSeriesName DataSeries name.
            \param attribute Which DCP attribute will be used.
            \param ids A set of identifiers of DataSet.

            \return A double with the maximum value.
          */
          double max(const std::string& attribute,
                     boost::python::list ids = boost::python::list());

          /*!
            \brief Calculates the mean of the latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

            In case of an error or no data available it will return NAN(Not A Number).

            \param dataSeriesName DataSeries name.
            \param attribute Which DCP attribute will be used.
            \param ids A set of identifiers of DataSet.

            \return A double with the mean.
          */
          double mean(const std::string& attribute,
                      boost::python::list ids = boost::python::list());

          /*!
            \brief Calculates the median value of the latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

            In case of an error or no data available it will return NAN(Not A Number).

            \param dataSeriesName DataSeries name.
            \param attribute Which DCP attribute will be used.
            \param ids A set of identifiers of DataSet.

            \return A double with the median.
          */
          double median(const std::string& attribute,
                        boost::python::list ids = boost::python::list());

          /*!
            \brief Calculates the sum of the latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

            In case of an error or no data available it will return NAN(Not A Number).

            \param dataSeriesName DataSeries name.
            \param attribute Which DCP attribute will be used.
            \param ids A set of identifiers of DataSet.

            \return A double with the sum.
           */
          double sum(const std::string& attribute,
                     boost::python::list ids = boost::python::list());

          /*!
            \brief Calculates the standard deviation of the latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

            In case of an error or no data available it will return NAN(Not A Number).

            \param dataSeriesName DataSeries name.
            \param attribute Which DCP attribute will be used.
            \param ids A set of identifiers of DataSet.

            \return A double with the standard deviation.
          */
          double standardDeviation(const std::string& attribute,
                                   boost::python::list ids = boost::python::list());

          /*!
            \brief Calculates the variance of the latest DCP series data.

            In case an empty set of identifiers is given, it will use the influence
            configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the variance between DCP values.
            */
            double variance(const std::string& attribute,
                            boost::python::list ids = boost::python::list());

            namespace influence
            {
              std::vector< std::string > byRule(const terrama2::services::analysis::core::Buffer& buffer);
              std::vector< std::string > all(bool isActive);

              namespace python
              {
                boost::python::list byRule(const terrama2::services::analysis::core::Buffer& buffer);
              }
            }

        } // end namespace dcp
      }   // end namespace core
    }     // end namespace analysis
  }       // end namespace services
}         // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_DCP_OPERATOR_HPP__
