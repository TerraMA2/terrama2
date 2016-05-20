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
  \file terrama2/services/analysis/core/PythonInterpreter.hpp

  \brief Manages the communication of Python and C.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_PYTHONINTERPRETER_HPP__
#define __TERRAMA2_ANALYSIS_CORE_PYTHONINTERPRETER_HPP__

#include "Analysis.hpp"
#include "BufferMemory.hpp"

#include <boost/python.hpp>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \brief Defines the statistic operation to be used in the analysis.
        */
        enum StatisticOperation
        {
          MIN = 1, //!< Minimum value.
          MAX = 2, //!< Maximum value.
          SUM = 3, //!< Sum.
          MEAN = 4, //!< Mean.
          MEDIAN = 5, //!< Median.
          STANDARD_DEVIATION = 6, //!< Standard deviation.
          COUNT = 7 //!< Count.
        };

        /*!
          \brief Add a value to the result table for the given attribute.
          \param attribute The name of the attribute.
          \param value The result value.
        */
        void addValue(const std::string& attribute, double value);

        /*!
          \brief Implementation of operator for latest DCP series data.

          In case an empty set of identifiers is given, it will use the influence
          configured for the analysis to determine which DCP dataset will be used.

          In case of an error or no data available it will return NAN(Not A Number).

          \param statisticOperation The statistic operation chosen by the user.
          \param dataSeriesName DataSeries name.
          \param buffer Buffer to be used for a monitored object.
          \param bufferType The buffer type.
          \param ids A set of identifiers of DataSet.

          \return A double value with the result.
        */
        double dcpOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids = boost::python::list());

        /*!
          \brief It returns the number of DCPs that have influence over the current monitored object.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param radius The buffer radius.
          \param bufferType The buffer type.

          \return The number of DCP that have influence over the current monitored object.
        */
        int dcpCount(const std::string& dataSeriesName, Buffer buffer);

        /*!
          \brief It calculates the minimum value of the latest DCP series data.

          In case an empty set of identifiers is given, it will use the influence
          configured for the analysis to determine which DCP dataset will be used.

          In case of an error or no data available it will return NAN(Not A Number).

          \param statisticOperation The statistic operation chosen by the user.
          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param radius The buffer radius.
          \param bufferType The buffer type.
          \param ids A set of identifiers of DataSet.

          \return A double with the minimum value.
        */
        double dcpMin(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids = boost::python::list());

        /*!
          \brief It calculates the maximum value of the latest DCP series data.

          In case an empty set of identifiers is given, it will use the influence
          configured for the analysis to determine which DCP dataset will be used.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param buffer Buffer to be used in the monitored object.
          \param ids A set of identifiers of DataSet.

          \return A double with the maximum value.
        */
        double dcpMax(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids = boost::python::list());

        /*!
          \brief It calculates the mean of the latest DCP series data.

          In case an empty set of identifiers is given, it will use the influence
          configured for the analysis to determine which DCP dataset will be used.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param buffer Buffer to be used in the monitored object.
          \param ids A set of identifiers of DataSet.

          \return A double with the mean.
        */
        double dcpMean(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids = boost::python::list());

        /*!
          \brief It calculates the median value of the latest DCP series data.

          In case an empty set of identifiers is given, it will use the influence
          configured for the analysis to determine which DCP dataset will be used.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param buffer Buffer to be used in the monitored object.
          \param ids A set of identifiers of DataSet.

          \return A double with the median.
        */
        double dcpMedian(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids = boost::python::list());

        /*!
          \brief It calculates the sum of the latest DCP series data.

          In case an empty set of identifiers is given, it will use the influence
          configured for the analysis to determine which DCP dataset will be used.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param buffer Buffer to be used in the monitored object.
          \param ids A set of identifiers of DataSet.

          \return A double with the sum.
        */
        double dcpSum(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids = boost::python::list());

        /*!
          \brief It calculates the standard deviation of the latest DCP series data.

          In case an empty set of identifiers is given, it will use the influence
          configured for the analysis to determine which DCP dataset will be used.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param buffer Buffer to be used in the monitored object.
          \param ids A set of identifiers of DataSet.

          \return A double with the standard deviation.
        */
        double dcpStandardDeviation(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids = boost::python::list());

        /*!
          \brief Implementation of history operator for DCP series.

          In case of an error or no data available it will return NAN(Not A Number).

          \param statisticOperation The statistic operation chosen by the user.
          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param dcpId Identifier of DCP dataset.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.

          \return A double value with the result.
        */
        double dcpHistoryOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter);

        /*!
          \brief It calculates the sum of historic DCP data.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param dcpId Identifier of DCP dataset.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.

          \return A double value with the result.
        */
        double dcpHistorySum(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter);

        /*!
          \brief It calculates the mean of historic DCP data.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param dcpId Identifier of DCP dataset.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.

          \return A double value with the result.
        */
        double dcpHistoryMean(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter);

        /*!
          \brief It calculates the min of historic DCP data.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param dcpId Identifier of DCP dataset.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.

          \return A double value with the result.
        */
        double dcpHistoryMin(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter);

        /*!
          \brief It calculates the max of historic DCP data.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param dcpId Identifier of DCP dataset.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.

          \return A double value with the result.
        */
        double dcpHistoryMax(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter);

        /*!
          \brief It calculates the median of historic DCP data.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param dcpId Identifier of DCP dataset.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.

          \return A double value with the result.
        */
        double dcpHistoryMedian(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter);

        /*!
          \brief It calculates the standard deviation of historic DCP data.

          In case of an error or no data available it will return NAN(Not A Number).

          \param dataSeriesName DataSeries name.
          \param attribute Which DCP attribute will be used.
          \param dcpId Identifier of DCP dataset.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.

          \return A double value with the result.
        */
        double dcpHistoryStandardDeviation(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter);

        /*!
          \brief It calculates the count of occurrences in the monitored object.

          \param dataSeriesName DataSeries name.
          \param buffer Buffer to be used in the monitored object.
          \param dateFilter Time filter for the data.
          \param restriction SQL restriction.

          \return The number of occurrences in the monitored object.
        */
        int occurrenceCount(const std::string& dataSeriesName, Buffer buffer, std::string dateFilter, std::string restriction);

        /*!
          \brief Registers all DCP functions in the Python interpreter.
        */
        void registerDCPFunctions();

        /*!
          \brief Registers all occurrences functions in the Python interpreter.
        */
        void registerOccurrenceFunctions();

        /*!
          \brief Initialize Python interpreter.
        */
        void initInterpreter();

        /*!
          \brief Finalize Python interpreter.
        */
        void finalizeInterpreter();

        /*!
          \brief Run Python script for a monitored object analysis.
          \param state Python thread state.
          \param analysisId Analysis identifier.
          \param indexes Vector of geometries indexes.
        */
        void runScriptMonitoredObjectAnalysis(PyThreadState* state, uint64_t analysisId, std::vector<uint64_t> indexes);

        /*!
          \brief Run Python script for a monitored object analysis.
          \param state Python thread state.
          \param analysisId Analysis identifier.
        */
        void runScriptDCPAnalysis(PyThreadState* state, uint64_t analysisId);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_PYTHONINTERPRETER_HPP__
