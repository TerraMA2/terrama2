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
#include "OperatorCache.hpp"
#include "BaseContext.hpp"
#include "GridContext.hpp"
#include "MonitoredObjectContext.hpp"
#include "Typedef.hpp"

// STL
#include <vector>
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
        /*!
          \brief Defines the statistic operation to be used in the analysis.
        */
        enum class StatisticOperation
        {
          INVALID = 0, //!< Invalid operation.
          MIN = 1, //!< Minimum value.
          MAX = 2, //!< Maximum value.
          SUM = 3, //!< Sum.
          MEAN = 4, //!< Mean.
          MEDIAN = 5, //!< Median.
          STANDARD_DEVIATION = 6, //!< Standard deviation.
          COUNT = 7 //!< Count.
        };

        /*!
          \brief Lock to the Python GIL.
        */
        struct GILLock
        {
          public:
            /*!
              \brief Constructor
            */
            GILLock()
            {
              state_ = PyGILState_Ensure();
            }

            /*!
              \brief Destructor
            */
            ~GILLock()
            {
              PyGILState_Release(state_);
            }

          private:
            PyGILState_STATE state_; //!< Python GIL state.
        };

        /*!
          \brief Add a value to the result table for the given attribute.
          \param attribute The name of the attribute.
          \param value The result value.
        */
        void addValue(const std::string& attribute, double value);


        /*!
          \brief Initialize Python interpreter.
          \note This must be called before creating any thread.
        */
        void initInterpreter();
        void populateNamespace();

        /*!
          \brief Finalize Python interpreter.
        */
        void finalizeInterpreter();

        /*!
          \brief Run Python script for a monitored object analysis.
          \param state Python thread state.
          \param analysisHashCode Analysis hash code.
          \param indexes Vector of geometries indexes.
        */
        void runMonitoredObjectScript(PyThreadState* state, MonitoredObjectContextPtr context, std::vector<uint64_t> indexes);


        /*!
          \brief Run Python script for a grid analysis.
          \param state Python thread state.
          \param analysisHashCode Analysis hash code.
          \param indexes Vector of row indexes to process.
        */
        void runScriptGridAnalysis(PyThreadState* state, terrama2::services::analysis::core::GridContextPtr context, std::vector<uint64_t> rows);

        /*!
          \brief Run Python script for a monitored object analysis.
          \param state Python thread state.
          \param analysisHashCode Analysis hash code.
        */
        void runScriptDCPAnalysis(PyThreadState* state, MonitoredObjectContextPtr context);

        /*!
          \brief Read analysis information from Python thread dict.
          \param cache Cache to store the information for the operator.
        */
        void readInfoFromDict(OperatorCache& cache);

        /*!
          \brief Returns the result of the operation for the given statistic.
          \param cache Cache with the calculated statistics.
          \param statisticOperation The statistic operation called by the script.
        */
        double getOperationResult(OperatorCache& cache, StatisticOperation statisticOperation);

        /*!
          \brief Returns the ContextDataSeries of the monitored object for the given analysis.
          \param analysisHashCode Analysis hashcode.
          \param dataManagerPtr Smart pointer to the data manager.
          \return The ContextDataSeries of the monitored object.
        */
        std::shared_ptr<ContextDataSeries> getMonitoredObjectContextDataSeries(MonitoredObjectContextPtr context,
                                                                               std::shared_ptr<DataManager>& dataManagerPtr);

        /*!
          \brief Returns the attribute value for the given position, it tries a lexical cast to double in case the attribute has a different type.
          \param syncDs Smart pointer to the dataset.
          \param attribute Attribute name.
          \param i The position.
          \param attributeType The attribute type.
          \return The attribute value for the given position
        */
        double getValue(terrama2::core::SynchronizedDataSetPtr syncDs, const std::string& attribute, uint64_t i, int attributeType);

        /*!
         \brief Calculates the statistics based on the given values.
         \param values The list of values.
         \param cache The OperatorCache to store the results.
        */
        void calculateStatistics(std::vector<double>& values, OperatorCache& cache);

        /*!
          \brief Extracts the error message from Python interpreter.
        */
        std::string extractException();

        /*!
          \brief Insert the analysis script in a function.
          \param analysisHashCode Analysis hashcode.
        */
        std::string prepareScript(terrama2::services::analysis::core::BaseContextPtr context);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_PYTHONINTERPRETER_HPP__
