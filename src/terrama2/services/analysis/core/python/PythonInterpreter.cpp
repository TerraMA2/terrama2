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
  \file terrama2/services/analysis/core/PythonInterpreter.cpp

  \brief Manages the communication of Python and C.

  \author Paulo R. M. Oliveira
*/

#include "PythonInterpreter.hpp"

#include <boost/python.hpp>

#include <QTextStream>

#include "../Exception.hpp"
#include "../ContextManager.hpp"
#include "../GridContext.hpp"
#include "../MonitoredObjectContext.hpp"
#include "../BufferMemory.hpp"

#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/data-model/Filter.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/UnitsOfMeasureManager.h>

// STL
#include <math.h>

// Boost Python
#include <boost/python/call.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/python/stl_iterator.hpp>

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

using namespace boost::python;


std::string terrama2::services::analysis::core::python::extractException()
{
  using namespace boost::python;

  PyObject* exc,*val,*tb;
  PyErr_Fetch(&exc,&val,&tb);
  PyErr_NormalizeException(&exc,&val,&tb);
  handle<> hexc(exc),hval(allow_null(val)),htb(allow_null(tb));
  if(!hval)
  {
    return extract<std::string>(str(hexc));
  }
  else
  {
    object traceback(import("traceback"));
    object format_exception(traceback.attr("format_exception"));
    object formatted_list(format_exception(hexc,hval,htb));
    object formatted(str("").join(formatted_list));
    std::string errMsg = extract<std::string>(formatted);
    boost::replace_all(errMsg, "\"", "");
    boost::replace_all(errMsg, "\'", "");
    return errMsg;
  }
}

void terrama2::services::analysis::core::python::runMonitoredObjectScript(PyThreadState* state, MonitoredObjectContextPtr context, std::vector<uint32_t> indexes)
{

  // grab the global interpreter lock
  GILLock lock;

  if(!state)
  {
    QString errMsg = QObject::tr("Invalid thread state for python interpreter.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }

  // swap in my thread state
  PyThreadState_Swap(state);
  try
  {
    AnalysisPtr analysis = context->getAnalysis();

    std::string script = prepareScript(context);

    PyObject* pCompiledFn = Py_CompileString(script.c_str() , "" , Py_file_input) ;
    if(pCompiledFn == NULL)
    {
      QString errMsg = QObject::tr("Invalid script.");
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    Py_INCREF(pCompiledFn);

    // create a module
    PyObject* pModule = PyImport_ExecCodeModule((char*)"analysis" , pCompiledFn) ;
    if(pModule == NULL)
    {
      std::string errMsg = QObject::tr("Could not register the analysis function.").toStdString();
      throw PythonInterpreterException() << terrama2::ErrorDescription(errMsg.c_str());
    }

    Py_INCREF(pModule);

    boost::python::object analysisModule = boost::python::import("analysis");
    boost::python::object analysisFunction = analysisModule.attr("analysis");
    AnalysisHashCode analysisHashCode = analysis->hashCode(context->getStartTime());

    auto pValueAnalysis = PyInt_FromLong(analysisHashCode);
    auto isHashSet = PyDict_SetItemString(state->dict, "analysisHashCode", pValueAnalysis);

    for(uint32_t index : indexes)
    {
      auto pValueIndex = PyInt_FromLong(index);
      auto isindexSet = PyDict_SetItemString(state->dict, "index", pValueIndex);

      if(isHashSet == 0 && isindexSet == 0)
      {
        //TODO: read the return value
        analysisFunction();
      }
      else
      {
        std::string errMsg = QObject::tr("Could not set script parameters.").toStdString();
        throw PythonInterpreterException() << terrama2::ErrorDescription(errMsg.c_str());
      }

      Py_DECREF(pValueIndex);

    }

    Py_DECREF(pValueAnalysis);
    Py_DECREF(pModule);
    Py_DECREF(pCompiledFn);
  }
  catch(const error_already_set&)
  {
    std::string errMsg = extractException();
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
  }
}


void terrama2::services::analysis::core::python::runScriptGridAnalysis(PyThreadState* state, terrama2::services::analysis::core::GridContextPtr context, std::vector<uint32_t> rows)
{
  GILLock lock;

  if(!state)
  {
    QString errMsg = QObject::tr("Invalid thread state for python interpreter.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }

  // swap in my thread state
  auto previousState = PyThreadState_Swap(state);

  try
  {
    AnalysisPtr analysis = context->getAnalysis();

    auto outputRaster = context->getOutputRaster();
    if(!outputRaster)
    {
      QString errMsg(QObject::tr("Invalid output raster."));
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    int nCols = outputRaster->getNumberOfColumns();

    std::string script = prepareScript(context);
    PyObject* pCompiledFn = Py_CompileString(script.c_str() , "" , Py_file_input) ;
    if(pCompiledFn == NULL)
    {
      QString errMsg(QObject::tr("Invalid script."));
      throw PythonInterpreterException() << terrama2::ErrorDescription(errMsg);
    }

    // create a module
    PyObject* pModule = PyImport_ExecCodeModule((char*)"analysis" , pCompiledFn) ;
    if(pModule == NULL)
    {
      QString errMsg(QObject::tr("Could not register the analysis function."));
      throw PythonInterpreterException() << terrama2::ErrorDescription(errMsg);
    }

    boost::python::object analysisModule = boost::python::import("analysis");
    boost::python::object analysisFunction = analysisModule.attr("analysis");
    auto analysisHashCode = analysis->hashCode(context->getStartTime());

    auto pValueAnalysis = PyInt_FromLong(analysisHashCode);

    PyDict_SetItemString(state->dict, "analysisHashCode", pValueAnalysis);

    for(int row : rows)
    {
      auto pValueRow = PyInt_FromLong(row);
      PyDict_SetItemString(state->dict, "row", pValueRow);
      for(int col = 0; col < nCols; ++col)
      {
        auto pValueColumn = PyInt_FromLong(col);

        PyDict_SetItemString(state->dict, "column", pValueColumn);


        boost::python::object result = analysisFunction();
        double value = boost::python::extract<double>(result);
        if(std::isnan(value))
          outputRaster->setValue(col, row, analysis->outputGridPtr->interpolationDummy);
        else
          outputRaster->setValue(col, row, value);

        Py_DECREF(pValueColumn);
      }
      Py_DECREF(pValueRow);
    }

    Py_DECREF(pValueAnalysis);
    Py_DECREF(pModule);
    Py_DECREF(pCompiledFn);
  }
  catch(error_already_set)
  {
    std::string errMsg = extractException();
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
  }

  PyThreadState_Swap(previousState);
}

void terrama2::services::analysis::core::python::runScriptDCPAnalysis(PyThreadState* state, MonitoredObjectContextPtr context)
{
  AnalysisPtr analysis = context->getAnalysis();

  // grab the global interpreter lock
  GILLock lock;
  // swap in my thread state
  PyThreadState_Swap(state);
  PyThreadState_Clear(state);

  //TODO: is it needed?
  // Adds the analysis hashcode to the python state
  //  PyObject* analysisValue = PyInt_FromLong(analysisHashCode);
  //  PyObject* poDict = PyDict_New();
  //  PyDict_SetItemString(poDict, "analysisHashCode", analysisValue);
  //  state->dict = poDict;

  try
  {
    object main_module((handle<>(borrowed(PyImport_AddModule("__main__")))));
    object main_namespace = main_module.attr("__dict__");

    handle<> ignored((PyRun_String("from terrama2 import *", Py_file_input, main_namespace.ptr(), main_namespace.ptr())));
    ignored = handle<>((PyRun_String(analysis->script.c_str(), Py_file_input, main_namespace.ptr(), main_namespace.ptr())));

  }
  catch(error_already_set)
  {
    std::string errMsg = extractException();
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg);
  }
}

void terrama2::services::analysis::core::python::addValue(const std::string& attribute, double value)
{
  OperatorCache cache;

  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  std::string attrName = boost::to_lower_copy(attribute);

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return;
  }

  try
  {
    AnalysisPtr analysis = context->getAnalysis();
    if(analysis->type == AnalysisType::MONITORED_OBJECT_TYPE)
    {
      context->addAttribute(attrName);
      context->setAnalysisResult(cache.index, attrName, value);
    }
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return;
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    return;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }


}

BOOST_PYTHON_MODULE(terrama2)
{
  // specify that this module is actually a package
  object package = scope();
  package.attr("__path__") = "terrama2";


  def("add_value", terrama2::services::analysis::core::python::addValue);

  // Export BufferType enum to python
  enum_<terrama2::services::analysis::core::BufferType>("BufferType")
  .value("None", terrama2::services::analysis::core::NONE)
  .value("In", terrama2::services::analysis::core::IN)
  .value("Out", terrama2::services::analysis::core::OUT)
  .value("In_out", terrama2::services::analysis::core::IN_OUT)
  .value("Out_union", terrama2::services::analysis::core::OUT_UNION)
  .value("In_diff", terrama2::services::analysis::core::IN_DIFF)
  .value("Level", terrama2::services::analysis::core::LEVEL);

  // Export class Buffer enum to python
  class_<terrama2::services::analysis::core::Buffer>("Buffer", init<>())
  .def(init<terrama2::services::analysis::core::BufferType>())
  .def(init<terrama2::services::analysis::core::BufferType, double, std::string>())
  .def(init<terrama2::services::analysis::core::BufferType, double, std::string, double, std::string>())
  .def_readwrite("buffer_type", &terrama2::services::analysis::core::Buffer::bufferType)
  .def_readwrite("distance", &terrama2::services::analysis::core::Buffer::distance)
  .def_readwrite("distance2", &terrama2::services::analysis::core::Buffer::distance2)
  .def_readwrite("unit", &terrama2::services::analysis::core::Buffer::unit)
  .def_readwrite("unit2", &terrama2::services::analysis::core::Buffer::unit2);

  // Export class StatisticOperation enum to python
  enum_<terrama2::services::analysis::core::StatisticOperation>("Statistic")
  .value("min", terrama2::services::analysis::core::StatisticOperation::MIN)
  .value("max", terrama2::services::analysis::core::StatisticOperation::MAX)
  .value("sum", terrama2::services::analysis::core::StatisticOperation::SUM)
  .value("mean", terrama2::services::analysis::core::StatisticOperation::MEAN)
  .value("median", terrama2::services::analysis::core::StatisticOperation::MEDIAN)
  .value("standard_deviation", terrama2::services::analysis::core::StatisticOperation::STANDARD_DEVIATION)
  .value("count", terrama2::services::analysis::core::StatisticOperation::COUNT);
}

#if PY_MAJOR_VERSION >= 3
#   define INIT_MODULE PyInit_terrama2
extern "C" PyObject* INIT_MODULE();
#else
#   define INIT_MODULE initterrama2
extern "C" void INIT_MODULE();
#endif

void terrama2::services::analysis::core::python::populateNamespace()
{
  INIT_MODULE();
}

void terrama2::services::analysis::core::python::readInfoFromDict(OperatorCache& cache)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;

  // Analysis ID
  PyObject* analysisKey = PyString_FromString("analysisHashCode");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  if(analysisPy != NULL)
  {
    cache.analysisHashCode = PyInt_AsLong(analysisPy);
  }

  auto analysis = ContextManager::getInstance().getAnalysis(cache.analysisHashCode);
  if(!analysis)
  {
    QString errMsg(QObject::tr("Could not recover analysis configuration."));
    ContextManager::getInstance().addError(cache.analysisHashCode, errMsg.toStdString());
    return;
  }

  cache.analysisPtr = analysis;

  switch(analysis->type)
  {
    case AnalysisType::DCP_TYPE:
      break;
    case AnalysisType::MONITORED_OBJECT_TYPE:
    {
      // Geom index
      PyObject* geomKey = PyString_FromString("index");
      PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
      if(geomKey != NULL)
      {
        cache.index = PyInt_AsLong(geomIdPy);
      }
      Py_DECREF(geomKey);
      break;
    }
    case AnalysisType::GRID_TYPE:
    {
      // Output raster row
      PyObject* rowKey = PyString_FromString("row");
      PyObject* rowValue = PyDict_GetItem(pDict, rowKey);
      if(rowValue != NULL)
      {
        cache.row = PyInt_AsLong(rowValue);
      }

      // Ouput raster column
      PyObject* columnKey = PyString_FromString("column");
      PyObject* columnValue = PyDict_GetItem(pDict, columnKey);
      if(columnValue != NULL)
      {
        cache.column = PyInt_AsLong(columnValue);
      }

      Py_DECREF(rowKey);
      Py_DECREF(columnKey);
      break;
    }

  }

  Py_DECREF(analysisKey);


}


std::string terrama2::services::analysis::core::python::prepareScript(terrama2::services::analysis::core::BaseContextPtr context)
{
  AnalysisPtr analysis = context->getAnalysis();
  std::string formatedScript = analysis->script;

  // Adds indent after line break
  size_t pos = 0;
  std::string lineBreak = "\n";
  std::string formatedLineBreak = "\n    ";
  while((pos = formatedScript.find(lineBreak, pos)) != std::string::npos)
  {
    formatedScript.replace(pos, lineBreak.length(), formatedLineBreak);
    pos += formatedLineBreak.length();
  }

  // Adds indent to the first line
  formatedScript = "    "  + formatedScript;
  formatedScript = "from terrama2 import *\ndef analysis():\n" + formatedScript;

  return formatedScript;
}

std::mutex terrama2::services::analysis::core::python::GILLock::mutex_;

terrama2::services::analysis::core::python::GILLock::GILLock(bool lock) : lock_(lock)
{
  if(lock)
  {
    mutex_.lock();
    PyEval_AcquireLock();
  }
}

terrama2::services::analysis::core::python::GILLock::~GILLock()
{
  if(lock_)
  {
    PyEval_ReleaseLock();
    mutex_.unlock();
  }
}

terrama2::services::analysis::core::python::OperatorLock::OperatorLock()
  : GILLock(false)
{
  save_ =  PyEval_SaveThread();
  mutex_.unlock();
}

terrama2::services::analysis::core::python::OperatorLock::~OperatorLock()
{
  mutex_.lock();
  PyEval_RestoreThread(save_);
}


// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop
