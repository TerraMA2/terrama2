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

#include "../Analysis.hpp"
#include "../OperatorCache.hpp"
#include "../BaseContext.hpp"
#include "../GridContext.hpp"
#include "../MonitoredObjectContext.hpp"
#include "../Typedef.hpp"
#include "../utility/Utils.hpp"

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
        namespace python
        {
          /*!
            \brief Lock to the Python GIL.
          */
          struct GILLock
          {
            public:
              /*!
                \brief Constructor
              */
              GILLock(bool lock = true);

              /*!
                \brief Destructor
              */
              virtual ~GILLock();


              static std::mutex mutex_;

            protected:
              bool lock_;
          };

          struct OperatorLock : public GILLock
          {
            public:
              //! Constructor
              OperatorLock();

              //! Destructor
              virtual ~OperatorLock();


            private:
              PyThreadState* save_;

          };

          /*!
            \brief Add a value to the result table for the given attribute.
            \note The attribute name will be converted to lower case.
            \param attribute The name of the attribute.
            \param value The result value.
          */
          void addValue(const std::string& attribute, boost::python::object pyObjValue);

          void populateNamespace();

          /*!
            \brief Run Python script for a monitored object analysis.
            \param state Python thread state.
            \param analysisHashCode Analysis hash code.
            \param indexes Vector of geometries indexes.
          */
          void runMonitoredObjectScript(PyThreadState* state, MonitoredObjectContextPtr context, std::vector<uint32_t> indexes);


          /*!
            \brief Run Python script for a grid analysis.
            \param state Python thread state.
            \param analysisHashCode Analysis hash code.
            \param indexes Vector of row indexes to process.
          */
          void runScriptGridAnalysis(PyThreadState* state, terrama2::services::analysis::core::GridContextPtr context, std::vector<uint32_t> rows);

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
            \brief Extracts the error message from Python interpreter.
          */
          std::string extractException(terrama2::services::analysis::core::AnalysisPtr analysis = nullptr);

          /*!
            \brief Insert the analysis script in a function.
            \param analysis Pointer to the analysis object.
          */
          std::string prepareScript(AnalysisPtr analysis);

          /*!
            \brief Load Analysis scripts in Python Interpreter and apply statically validation
            \todo Since we are working with interpreted language, most of errors occurs in runtime.
                  In this way, we must apply other validations, such ZeroDivisionError, etc.
            \param analysis Pointer to the analysis object.
            \param validateResult Reference to the object containing the result of all validations.
          */
          void validateAnalysisScript(AnalysisPtr analysis, ValidateResult& validateResult);

          /*!
            \brief Returns the attrubute values as a json object.
            \param attribute Attribute name.
          */
          std::string getAttributeValueAsJson(const std::string &attribute);

          std::string getCurrentExecutionDate();

        } // end namespace python
      }   // end namespace core
    }     // end namespace analysis
  }       // end namespace services
}         // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_PYTHONINTERPRETER_HPP__
