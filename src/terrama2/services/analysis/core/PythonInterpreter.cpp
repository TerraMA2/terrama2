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
#include "../../../core/utility/Logger.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "dcp/Operator.hpp"
#include "dcp/history/Operator.hpp"
#include "occurrence/Operator.hpp"
#include "occurrence/aggregation/Operator.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/UnitsOfMeasureManager.h>

#include <math.h>

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

void terrama2::services::analysis::core::runScriptMonitoredObjectAnalysis(PyThreadState* state, AnalysisHashCode analysisHashCode,
                                                                          std::vector<uint64_t> indexes)
{
  AnalysisPtr analysis = Context::getInstance().getAnalysis(analysisHashCode);

  for(uint64_t index : indexes)
  {
    // grab the global interpreter lock
    PyEval_AcquireLock();
    // swap in my thread state
    PyThreadState_Swap(state);
    PyThreadState_Clear(state);

    // Adds the monitored object index and analysis hashcode to the python state
    PyObject* indexValue = PyInt_FromLong(index);
    PyObject* analysisValue = PyInt_FromLong(analysisHashCode);

    PyObject* poDict = PyDict_New();
    PyDict_SetItemString(poDict, "index", indexValue);
    PyDict_SetItemString(poDict, "analysis", analysisValue);
    state->dict = poDict;

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
      Context::getInstance().addError(analysisHashCode, errMsg);
    }

    // release our hold on the global interpreter
    PyEval_ReleaseLock();
  }
}

void terrama2::services::analysis::core::runScriptDCPAnalysis(PyThreadState* state, AnalysisHashCode analysisHashCode)
{

  AnalysisPtr analysis = Context::getInstance().getAnalysis(analysisHashCode);

  // grab the global interpreter lock
  PyEval_AcquireLock();
  // swap in my thread state
  PyThreadState_Swap(state);
  PyThreadState_Clear(state);

  // Adds the analysis hashcode to the python state
  PyObject* analysisValue = PyInt_FromLong(analysisHashCode);
  PyObject* poDict = PyDict_New();
  PyDict_SetItemString(poDict, "analysis", analysisValue);
  state->dict = poDict;

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
    Context::getInstance().addError(analysisHashCode, errMsg);
  }

  // release our hold on the global interpreter
  PyEval_ReleaseLock();

}

void terrama2::services::analysis::core::addValue(const std::string& attribute, double value)
{
  OperatorCache cache;
  readInfoFromDict(cache);

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    Context::getInstance().addError(cache.analysisHashCode, errMsg.toStdString());
    return;
  }

  AnalysisPtr analysis = Context::getInstance().getAnalysis(cache.analysisHashCode);
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

        if(!Context::getInstance().exists(cache.analysisHashCode, datasetMO->id))
        {
          QString errMsg(QObject::tr("Could not recover monitored object dataset."));
          Context::getInstance().addError(cache.analysisHashCode, errMsg.toStdString());
          return;
        }

        moDsContext = Context::getInstance().getContextDataset(cache.analysisHashCode, datasetMO->id);

        if(moDsContext->identifier.empty())
        {
          QString errMsg(QObject::tr("Monitored object identifier is empty."));
          Context::getInstance().addError(cache.analysisHashCode, errMsg.toStdString());
          return;
        }

        // Stores the result in the context
        std::string geomId = moDsContext->series.syncDataSet->getString(cache.index, moDsContext->identifier);
        assert(!geomId.empty());

        Context::getInstance().addAttribute(cache.analysisHashCode, attribute);
        Context::getInstance().setAnalysisResult(cache.analysisHashCode, geomId, attribute, value);
      }
    }

    if(!found)
    {
      QString errMsg(QObject::tr("Could not find a monitored data series in this analysis."));
      Context::getInstance().addError(cache.analysisHashCode, errMsg.toStdString());
      return;
    }
  }
}


double terrama2::services::analysis::core::getOperationResult(OperatorCache& cache,
                                                              StatisticOperation statisticOperation)
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


std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> terrama2::services::analysis::core::getMonitoredObjectContextDataSeries(
        AnalysisHashCode analysisHashCode, std::shared_ptr<DataManager>& dataManagerPtr)
{
  std::shared_ptr<ContextDataSeries> contextDataSeries;

  auto analysis = Context::getInstance().getAnalysis(analysisHashCode);

  for(const AnalysisDataSeries& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(dataSeries->datasetList.size() == 1);
      auto datasetMO = dataSeries->datasetList[0];

      if(!Context::getInstance().exists(analysisHashCode, datasetMO->id))
      {
        QString errMsg(QObject::tr("Could not recover monitored object dataset."));

        Context::getInstance().addError(analysisHashCode, errMsg.toStdString());
        return contextDataSeries;
      }

      return Context::getInstance().getContextDataset(analysisHashCode, datasetMO->id);
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


// Declaration needed for default parameter ids
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMin_overloads, terrama2::services::analysis::core::dcp::min, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMax_overloads, terrama2::services::analysis::core::dcp::max, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMean_overloads, terrama2::services::analysis::core::dcp::mean, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMedian_overloads, terrama2::services::analysis::core::dcp::median, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpSum_overloads, terrama2::services::analysis::core::dcp::sum, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpStandardDeviation_overloads,
                                terrama2::services::analysis::core::dcp::standardDeviation, 3, 4);


void terrama2::services::analysis::core::registerDCPFunctions()
{
  // map the dcp namespace to a sub-module
  // make "from terrama2.dcp import <function>" work
  object dcpModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp"))));
  // make "from terrama2 import dcp" work
  scope().attr("dcp") = dcpModule;
  // set the current scope to the new sub-module
  scope dcpScope = dcpModule;
  // export functions inside dcp namespace
  def("min", terrama2::services::analysis::core::dcp::min,
      dcpMin_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Minimum operator for DCP"));
  def("max", terrama2::services::analysis::core::dcp::max,
      dcpMax_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Maximum operator for DCP"));
  def("mean", terrama2::services::analysis::core::dcp::mean,
      dcpMean_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Mean operator for DCP"));
  def("median", terrama2::services::analysis::core::dcp::median,
      dcpMedian_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Median operator for DCP"));
  def("sum", terrama2::services::analysis::core::dcp::sum,
      dcpSum_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Sum operator for DCP"));
  def("standard_deviation", terrama2::services::analysis::core::dcp::standardDeviation,
      dcpStandardDeviation_overloads(args("dataSeriesName", "attribute", "buffer", "ids"),
                                     "Standard deviation operator for DCP"));
  def("count", terrama2::services::analysis::core::dcp::count);

  // Register operations for dcp.history
  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.history"))));
  // make "from terrama2.dcp import history" work
  scope().attr("history") = dcpHistoryModule;
  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;

  // export functions inside history namespace
  def("min", terrama2::services::analysis::core::dcp::history::min);
  def("max", terrama2::services::analysis::core::dcp::history::max);
  def("mean", terrama2::services::analysis::core::dcp::history::mean);
  def("median", terrama2::services::analysis::core::dcp::history::median);
  def("sum", terrama2::services::analysis::core::dcp::history::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::history::standardDeviation);

}



// Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceCount_overloads, terrama2::services::analysis::core::occurrence::count, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMin_overloads, terrama2::services::analysis::core::occurrence::min, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMax_overloads, terrama2::services::analysis::core::occurrence::max, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMean_overloads, terrama2::services::analysis::core::occurrence::mean, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMedian_overloads, terrama2::services::analysis::core::occurrence::median, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceSum_overloads, terrama2::services::analysis::core::occurrence::sum, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::standardDeviation, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationCount_overloads, terrama2::services::analysis::core::occurrence::aggregation::count, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMin_overloads, terrama2::services::analysis::core::occurrence::aggregation::min, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMax_overloads, terrama2::services::analysis::core::occurrence::aggregation::max, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMean_overloads, terrama2::services::analysis::core::occurrence::aggregation::mean, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMedian_overloads, terrama2::services::analysis::core::occurrence::aggregation::median, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationSum_overloads, terrama2::services::analysis::core::occurrence::aggregation::sum, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::aggregation::standardDeviation, 6, 7);


void terrama2::services::analysis::core::registerOccurrenceFunctions()
{
  // map the occurrence namespace to a sub-module
  // make "from terrama2.occurrence import <function>" work
  object occurrenceModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence"))));
  // make "from terrama2 import occurrence" work
  scope().attr("occurrence") = occurrenceModule;
  // set the current scope to the new sub-module
  scope occurrenceScope = occurrenceModule;
  // export functions inside occurrence namespace
  def("count", terrama2::services::analysis::core::occurrence::count,
      occurrenceCount_overloads(args("dataSeriesName", "buffer", "dateFilter", "restriction"),
                                "Count operator for occurrence"));
  def("min", terrama2::services::analysis::core::occurrence::min,
      occurrenceMin_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Minimum operator for occurrence"));
  def("max", terrama2::services::analysis::core::occurrence::max,
      occurrenceMax_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Maximum operator for occurrence"));
  def("mean", terrama2::services::analysis::core::occurrence::mean,
      occurrenceMean_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Mean operator for occurrence"));
  def("median", terrama2::services::analysis::core::occurrence::median,
      occurrenceMedian_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Median operator for occurrence"));
  def("sum", terrama2::services::analysis::core::occurrence::sum,
      occurrenceSum_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Sum operator for occurrence"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::standardDeviation,
      occurrenceStandardDeviation_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"),
                                            "Standard deviation operator for occurrence"));

  // Register operations for occurrence.aggregation
  object occurrenceAggregationModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence.aggregation"))));
  // make "from terrama2.occurrence import aggregation" work
  scope().attr("aggregation") = occurrenceAggregationModule;
  // set the current scope to the new sub-module
  scope occurrenceAggregationScope = occurrenceAggregationModule;

  // export functions inside aggregation namespace
  def("count", terrama2::services::analysis::core::occurrence::aggregation::count,
      occurrenceAggregationCount_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"),
                                           "Count operator for occurrence aggregation"));
  def("min", terrama2::services::analysis::core::occurrence::aggregation::min,
      occurrenceAggregationMin_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Minimum operator for occurrence aggregation"));
  def("max", terrama2::services::analysis::core::occurrence::aggregation::max,
      occurrenceAggregationMax_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Maximum operator for occurrence aggregation"));
  def("mean", terrama2::services::analysis::core::occurrence::aggregation::mean,
      occurrenceAggregationMean_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Mean operator for occurrence aggregation"));
  def("median", terrama2::services::analysis::core::occurrence::aggregation::median,
      occurrenceAggregationMedian_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Median operator for occurrence aggregation"));
  def("sum", terrama2::services::analysis::core::occurrence::aggregation::sum,
      occurrenceAggregationSum_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Sum operator for occurrence aggregation"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::aggregation::standardDeviation,
      occurrenceAggregationStandardDeviation_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"),
                                                       "Standard deviation operator for occurrence aggregation"));

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

  terrama2::services::analysis::core::registerDCPFunctions();
  terrama2::services::analysis::core::registerOccurrenceFunctions();

}

#if PY_MAJOR_VERSION >= 3
#   define INIT_MODULE PyInit_terrama2
extern "C" PyObject* INIT_MODULE();
#else
#   define INIT_MODULE initterrama2
extern "C" void INIT_MODULE();
#endif


void terrama2::services::analysis::core::initInterpreter()
{
  PyEval_InitThreads();
  Py_Initialize();
  INIT_MODULE();
  PyEval_ReleaseLock();
}

void terrama2::services::analysis::core::finalizeInterpreter()
{
  // shut down the interpreter
  PyEval_AcquireLock();
  Py_Finalize();
}

void terrama2::services::analysis::core::readInfoFromDict(OperatorCache& cache)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;

  // Geom index
  PyObject* geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  cache.index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject* analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  cache.analysisHashCode = PyInt_AsLong(analysisPy);
}











