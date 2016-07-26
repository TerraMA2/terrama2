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

#include "Exception.hpp"
#include "ContextManager.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "dcp/Operator.hpp"
#include "dcp/history/Operator.hpp"
#include "grid/Operator.hpp"
#include "grid/history/Operator.hpp"
#include "grid/history/interval/Operator.hpp"
#include "grid/forecast/Operator.hpp"
#include "grid/forecast/interval/Operator.hpp"
#include "occurrence/Operator.hpp"
#include "occurrence/aggregation/Operator.hpp"

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

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

using namespace boost::python;


std::string terrama2::services::analysis::core::extractException()
{
  using namespace boost::python;

  PyObject *exc,*val,*tb;
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

void terrama2::services::analysis::core::runMonitoredObjectScript(PyThreadState* state, MonitoredObjectContextPtr context, std::vector<uint64_t> indexes)
{
  AnalysisPtr analysis = context->getAnalysis();

  std::string script = prepareScript(context);

  // grab the global interpreter lock
  GILLock gilLock;
  assert(state);
  // swap in my thread state
  auto previousState = PyThreadState_Swap(state);

  PyObject* pCompiledFn = Py_CompileString( script.c_str() , "" , Py_file_input ) ;
  assert( pCompiledFn != NULL ) ;

  Py_INCREF(pCompiledFn);

  // create a module
  PyObject* pModule = PyImport_ExecCodeModule( (char*)"analysis" , pCompiledFn ) ;
  assert( pModule != NULL ) ;

  Py_INCREF(pModule);

  try
  {
    boost::python::object analysisModule = boost::python::import("analysis");
    boost::python::object analysisFunction = analysisModule.attr("analysis");
    AnalysisHashCode analysisHashCode = analysis->hashCode2(context->getStartTime());

    auto pValueAnalysis = PyInt_FromLong(analysisHashCode);

    for(uint64_t index : indexes)
    {
      auto pValueIndex = PyInt_FromLong(index);

      PyObject* poDict = PyDict_New();

      auto isHashSet = PyDict_SetItemString(poDict, "analysisHashCode", pValueAnalysis);
      auto isindexSet = PyDict_SetItemString(poDict, "index", pValueIndex);
      if(isHashSet == 0 && isindexSet == 0)
      {
        state->dict = poDict;
        //TODO: read the return value
        analysisFunction(analysisHashCode, index);
      }
      else
      {
        //TODO: PAULO: how to deal with this error?
        abort();
      }
    }
  }
  catch(const error_already_set&)
  {
    std::string errMsg = extractException();
    context->addError(errMsg);
  }


  state = PyThreadState_Swap(previousState);
}


void terrama2::services::analysis::core::runScriptGridAnalysis(PyThreadState* state, terrama2::services::analysis::core::GridContextPtr context, std::vector<uint64_t> rows)
{
  AnalysisPtr analysis = context->getAnalysis();

  auto outputRaster = context->getOutputRaster();
  if(!outputRaster)
  {
    QString errMsg(QObject::tr("Invalid output raster."));
    context->addError(errMsg.toStdString());
    return;
  }

  // grab the global interpreter lock
  GILLock gilLock;
  // swap in my thread state
  auto previousState = PyThreadState_Swap(state);

  int nCols = outputRaster->getNumberOfColumns();

  std::string script = prepareScript(context);
  PyObject* pCompiledFn = Py_CompileString( script.c_str() , "" , Py_file_input ) ;
  assert( pCompiledFn != NULL ) ;

  Py_INCREF(pCompiledFn);

  // create a module
  PyObject* pModule = PyImport_ExecCodeModule( (char*)"analysis" , pCompiledFn ) ;
  assert( pModule != NULL ) ;

  Py_INCREF(pModule);

  try
  {
    boost::python::object analysisModule = boost::python::import("analysis");
    boost::python::object analysisFunction = analysisModule.attr("analysis");
    auto analysisHashCode = analysis->hashCode2(context->getStartTime());

    auto pValueAnalysis = PyInt_FromLong(analysisHashCode);

    for(int row : rows)
    {
      for(int col = 0; col < nCols; ++col)
      {
        auto pValueRow = PyInt_FromLong(row);
        auto pValueColumn = PyInt_FromLong(col);

        PyObject* poDict = PyDict_New();

        PyDict_SetItemString(poDict, "analysisHashCode", pValueAnalysis);
        PyDict_SetItemString(poDict, "row", pValueRow);
        PyDict_SetItemString(poDict, "column", pValueColumn);
        state->dict = poDict;


        boost::python::object result = analysisFunction(analysisHashCode, row, col);
        double value = boost::python::extract<double>(result);
        if(std::isnan(value))
          outputRaster->setValue(col, row, analysis->outputGridPtr->interpolationDummy);
        else
          outputRaster->setValue(col, row, value);
      }
    }
  }
  catch(error_already_set)
  {
    std::string errMsg = extractException();
    context->addError(errMsg);
  }


  state = PyThreadState_Swap(previousState);
}

void terrama2::services::analysis::core::runScriptDCPAnalysis(PyThreadState* state, MonitoredObjectContextPtr context)
{
  AnalysisPtr analysis = context->getAnalysis();

  // grab the global interpreter lock
  PyEval_AcquireLock();
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
    context->addError(errMsg);
  }

  // release our hold on the global interpreter
  PyEval_ReleaseLock();

}

void terrama2::services::analysis::core::addValue(const std::string& attribute, double value)
{
  OperatorCache cache;
 readInfoFromDict(cache);
 auto context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);

  auto dataManagerPtr = context->getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    context->addError(errMsg.toStdString());
    return;
  }

  AnalysisPtr analysis = context->getAnalysis();
  if(analysis->type == AnalysisType::MONITORED_OBJECT_TYPE)
  {
    std::shared_ptr<ContextDataSeries> moDsContext;
    terrama2::core::DataSetPtr datasetMO;

    // Reads the object monitored
    bool found = false;
    auto analysisDataSeriesList = analysis->analysisDataSeriesList;
    for(auto analysisDataSeries : analysisDataSeriesList)
    {
      if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
      {
        found = true;
        auto dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
        assert(dataSeries->datasetList.size() == 1);
        datasetMO = dataSeries->datasetList[0];

        if(!context->exists(datasetMO->id))
        {
          QString errMsg(QObject::tr("Could not recover monitored object dataset."));
          context->addError(errMsg.toStdString());
          return;
        }

        moDsContext = context->getContextDataset(datasetMO->id);

        if(moDsContext->identifier.empty())
        {
          QString errMsg(QObject::tr("Monitored object identifier is empty."));
          context->addError(errMsg.toStdString());
          return;
        }
        //TODO PAULO: verificar se o identificador existes
        // Stores the result in the context
        std::string geomId = moDsContext->series.syncDataSet->getString(cache.index, moDsContext->identifier);
        assert(!geomId.empty());

        context->addAttribute(attribute);
        context->setAnalysisResult(geomId, attribute, value);
      }
    }

    if(!found)
    {
      QString errMsg(QObject::tr("Could not find a monitored data series in this analysis."));
      context->addError(errMsg.toStdString());
      return;
    }
  }
}


double terrama2::services::analysis::core::getOperationResult(OperatorCache& cache, StatisticOperation statisticOperation)
{
  switch(statisticOperation)
  {
    case StatisticOperation::SUM:
      return cache.sum;
    case StatisticOperation::MEAN:
      return cache.mean;
    case StatisticOperation::MIN:
      return cache.min;
    case StatisticOperation::MAX:
      return cache.max;
    case StatisticOperation::STANDARD_DEVIATION:
      return cache.standardDeviation;
    case StatisticOperation::MEDIAN:
      return cache.median;
    case StatisticOperation::COUNT:
      return cache.count;
    default:
      return NAN;
  }

}


std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries>
terrama2::services::analysis::core::getMonitoredObjectContextDataSeries(MonitoredObjectContextPtr context, std::shared_ptr<DataManager>& dataManagerPtr)
{
  std::shared_ptr<ContextDataSeries> contextDataSeries;

  auto analysis = context->getAnalysis();

  for(const AnalysisDataSeries& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(dataSeries->datasetList.size() == 1);
      auto datasetMO = dataSeries->datasetList[0];

      if(!context->exists(datasetMO->id))
      {
        QString errMsg(QObject::tr("Could not recover monitored object dataset."));

        context->addError(errMsg.toStdString());
        return contextDataSeries;
      }

      return context->getContextDataset(datasetMO->id);
    }
  }

  return contextDataSeries;
}

double terrama2::services::analysis::core::getValue(terrama2::core::SynchronizedDataSetPtr syncDs,
                                                    const std::string& attribute, uint64_t i, int attributeType)
{
  if(attribute.empty())
    return NAN;

  double value = NAN;
  switch(attributeType)
  {
    case te::dt::INT16_TYPE:
    {
      value = syncDs->getInt16(i, attribute);
    }
      break;
    case te::dt::INT32_TYPE:
    {
      value = syncDs->getInt32(i, attribute);
    }
      break;
    case te::dt::INT64_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getInt64(i, attribute));
    }
      break;
    case te::dt::DOUBLE_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getDouble(i, attribute));
    }
      break;
    case te::dt::NUMERIC_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getNumeric(i, attribute));
    }
      break;
    default:
      break;
  }

  return value;
}

void terrama2::services::analysis::core::calculateStatistics(std::vector<double>& values, OperatorCache& cache)
{
  if(values.size() == 0)
    return;

  cache.mean = cache.sum / cache.count;
  std::sort(values.begin(), values.end());
  double half = values.size() / 2;
  if(values.size() > 1 && values.size() % 2 == 0)
  {
    cache.median = (values[(int) half] + values[(int) half - 1]) / 2;
  }
  else
  {
    cache.median = values.size() == 1 ? values[0] : 0.;
  }

  // calculates the variance
  double sumVariance = 0.;
  for(unsigned int i = 0; i < values.size(); ++i)
  {
    double value = values[i];
    sumVariance += (value - cache.mean) * (value - cache.mean);
  }

  cache.standardDeviation = sumVariance / cache.count;
}



BOOST_PYTHON_MODULE (terrama2)
{
  // specify that this module is actually a package
  object package = scope();
  package.attr("__path__") = "terrama2";


  def("add_value", terrama2::services::analysis::core::addValue);

  // Export BufferType enum to python
  enum_<terrama2::services::analysis::core::BufferType>("BufferType")
          .value("none", terrama2::services::analysis::core::NONE)
          .value("only_buffer", terrama2::services::analysis::core::ONLY_BUFFER)
          .value("outside_plus_inside", terrama2::services::analysis::core::OUTSIDE_PLUS_INSIDE)
          .value("object_plus_buffer", terrama2::services::analysis::core::OBJECT_PLUS_BUFFER)
          .value("object_minus_buffer", terrama2::services::analysis::core::OBJECT_MINUS_BUFFER)
          .value("distance_zone", terrama2::services::analysis::core::DISTANCE_ZONE);

  // Export class Buffer enum to python
  class_<terrama2::services::analysis::core::Buffer>("Buffer", init<>())
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

void terrama2::services::analysis::core::populateNamespace()
{
  INIT_MODULE();
}

void terrama2::services::analysis::core::initInterpreter()
{
  PyEval_InitThreads();
  Py_Initialize();
  // INIT_MODULE();
  PyEval_ReleaseLock();
}

void terrama2::services::analysis::core::finalizeInterpreter()
{
  // shut down the interpreter
//  PyEval_AcquireLock();
//FIXME: crashing here!
//  Py_Finalize();
}

void terrama2::services::analysis::core::readInfoFromDict(OperatorCache& cache)
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

  switch(analysis->type)
  {
    case AnalysisType::PCD_TYPE:
    case AnalysisType::MONITORED_OBJECT_TYPE:
    {
      // Geom index
      PyObject* geomKey = PyString_FromString("index");
      PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
      if(geomKey != NULL)
      {
        cache.index = PyInt_AsLong(geomIdPy);
      }
    }
    case AnalysisType::GRID_TYPE:
    {
      // Ouput raster row
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
    }

  }
}


std::string terrama2::services::analysis::core::prepareScript(terrama2::services::analysis::core::BaseContextPtr context)
{
  AnalysisPtr analysis = context->getAnalysis();
  std::string formatedScript = analysis->script;

  // Adds indent after line break
  size_t pos = 0;
  std::string lineBreak = "\n";
  std::string formatedLineBreak = "\n    ";
  while ((pos = formatedScript.find(lineBreak, pos)) != std::string::npos)
  {
    formatedScript.replace(pos, lineBreak.length(), formatedLineBreak);
    pos += formatedLineBreak.length();
  }

  // Adds indent to the first line
  formatedScript = "    "  + formatedScript;

  // Adds the function declaration
  switch(analysis->type)
  {
    case AnalysisType::GRID_TYPE:
      formatedScript = "from terrama2 import *\ndef analysis(analysisHashCode, row, col):\n" + formatedScript;
      break;
    case AnalysisType::MONITORED_OBJECT_TYPE:
      formatedScript = "from terrama2 import *\ndef analysis(analysisHashCode, index):\n" + formatedScript;
      break;
    case AnalysisType::PCD_TYPE:
      formatedScript = "from terrama2 import *\ndef analysis(analysisHashCode):\n" + formatedScript;
      break;
  }

  return formatedScript;
}



// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop
