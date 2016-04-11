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

#include <Python.h>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        enum StatisticOperation
        {
          MEAN,
          SUM,
          MAX,
          MIN,
          STANDARD_DEVIATION,
          MEDIAN
        };

        PyObject* countPoints(PyObject* self, PyObject* args);

        PyObject* sumHistoryDCP(PyObject* self, PyObject* args);
        PyObject* meanHistoryDCP(PyObject* self, PyObject* args);
        PyObject* minHistoryDCP(PyObject* self, PyObject* args);
        PyObject* maxHistoryDCP(PyObject* self, PyObject* args);
        PyObject* medianHistoryDCP(PyObject* self, PyObject* args);
        PyObject* standardDeviationHistoryDCP(PyObject* self, PyObject* args);

        PyObject* operatorHistoryDCP(PyObject* args, StatisticOperation statisticOperation);

        PyObject* sumDCP(PyObject* self, PyObject* args);
        PyObject* meanDCP(PyObject* self, PyObject* args);
        PyObject* minDCP(PyObject* self, PyObject* args);
        PyObject* maxDCP(PyObject* self, PyObject* args);
        PyObject* standardDeviationDCP(PyObject* self, PyObject* args);
        PyObject* medianDCP(PyObject* self, PyObject* args);

        PyObject* operatorDCP(PyObject* args, StatisticOperation statisticOperation);


        PyObject* result(PyObject* self, PyObject* args);

        void initInterpreter();
        void finalizeInterpreter();

        void runScriptMonitoredObjectAnalysis(PyThreadState* state, uint64_t analysisId, std::vector<uint64_t> indexes);
        void runScriptDCPAnalysis(PyThreadState* state, uint64_t analysisId);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_PYTHONINTERPRETER_HPP__
