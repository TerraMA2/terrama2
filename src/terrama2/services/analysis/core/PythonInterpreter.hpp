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

#include <string>

#include "Context.hpp"

#include <terralib/dataaccess/dataset/DataSet.h>

// Boost
#include "boost/date_time/local_time/local_time.hpp"

#include <Python.h>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        PyObject* countPoints(PyObject* self, PyObject* args);
        PyObject* sumHistoryPCD(PyObject* self, PyObject* args);
        PyObject* result(PyObject* self, PyObject* args);

        std::string createMonitoredObjectFunction(const std::string& script);

        void init();

        void runMonitoredObjAnalysis(PyThreadState* state, uint64_t analysisId, std::vector<uint64_t> indexes);

        void finalize();
      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_PYTHONINTERPRETER_HPP__
