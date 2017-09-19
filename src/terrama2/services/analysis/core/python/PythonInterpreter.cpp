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
#include <QJsonObject>
#include <QJsonValue>

#include "../Exception.hpp"
#include "../ContextManager.hpp"
#include "../GridContext.hpp"
#include "../MonitoredObjectContext.hpp"
#include "../BufferMemory.hpp"

#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/TimeUtils.hpp"
#include "../../../../core/data-model/Filter.hpp"
#include "../utility/Verify.hpp"

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
#include <QtCore/QJsonDocument>

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
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }

  // swap in my thread state
  PyThreadState_Swap(state);
  try
  {
    AnalysisPtr analysis = context->getAnalysis();
    AnalysisHashCode analysisHashCode = analysis->hashCode(context->getStartTime());

    auto& contextManager = ContextManager::getInstance();
    // In case an error has already occurred, there is nothing to be done
    if(!contextManager.getMessages(analysisHashCode, BaseContext::MessageType::ERROR_MESSAGE).empty())
    {
      return;
    }

    std::string script = prepareScript(analysis);

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
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
  }
}


void terrama2::services::analysis::core::python::runScriptGridAnalysis(PyThreadState* state, terrama2::services::analysis::core::GridContextPtr context, std::vector<uint32_t> rows)
{
  GILLock lock;

  if(!state)
  {
    QString errMsg = QObject::tr("Invalid thread state for python interpreter.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
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

    std::string script = prepareScript(analysis);
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
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
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
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg);
  }
}

void terrama2::services::analysis::core::python::addValue(const std::string& attribute, boost::python::object pyObjValue)
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
    verify::analysisMonitoredObject(analysis);

    if(pyObjValue.is_none())
    {
      context->setAnalysisResult(cache.index, attrName, boost::any());
      return;
    }

    {
      // if the return value is a double
      boost::python::extract<double> extDouble(pyObjValue);
      if(extDouble.check())
      {
        context->addAttribute(attrName, te::dt::DOUBLE_TYPE);
        double value = extDouble;
        if(std::isnan(value))
          context->setAnalysisResult(cache.index, attrName, boost::any());
        else
          context->setAnalysisResult(cache.index, attrName, value);
        return;
      }

      // if the return value is a string
      boost::python::extract<std::string> extString(pyObjValue);
      if(extString.check())
      {
        context->addAttribute(attrName, te::dt::STRING_TYPE);
        std::string value = extString;
        context->setAnalysisResult(cache.index, attrName, value);
        return;
      }

      QString errMsg = QObject::tr("Unknown type in 'add_value'.");
      context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
      return;
    }
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return;
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    return;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }
}

BOOST_PYTHON_MODULE(terrama2)
{
  // specify that this module is actually a package
  object package = scope();
  package.attr("__path__") = "terrama2";

  def("add_value", terrama2::services::analysis::core::python::addValue);
  def("get_attribute_value_as_json", terrama2::services::analysis::core::python::getAttributeValueAsJson);
  def("get_current_execution_date", terrama2::services::analysis::core::python::getCurrentExecutionDate);

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
  if(analysisPy != nullptr)
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


std::string terrama2::services::analysis::core::python::prepareScript(AnalysisPtr analysis)
{
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

//TODO: The functions get_string_value and get_numeric_value can be improved with a simple python dict populated in the c++
  // Adds indent to the first line
  formatedScript = "    "  + formatedScript;
  formatedScript = "from terrama2 import *\n"
                   "import json\n"
                   "import datetime\n"
                   "def get_value(attr):\n"
                   "    answer = get_attribute_value_as_json(attr)\n"
                   "    if(answer):\n"
                   "        attr_json = json.loads(answer)\n"
                   "        value = attr_json[attr]\n"
                   "        if isinstance(value, unicode):\n"
                   "            return value.encode('UTF-8')\n"
                   "        else:\n"
                   "            return value\n"
                   "    else:\n"
                   "        return None\n\n"
                   "def get_analysis_date():\n"
                   "    iso_string_date = get_current_execution_date()\n"
                   "    return datetime.datetime.strptime(iso_string_date.translate(None, ':-'), '%Y%m%dT%H%M%S.%fZ')\n\n"
                   "def analysis():\n"
                   + formatedScript;

  return formatedScript;
}

void terrama2::services::analysis::core::python::validateAnalysisScript(AnalysisPtr analysis, ValidateResult& validateResult)
{
  try
  {
    std::string script = prepareScript(analysis);
    PyObject *pCompiledFn = Py_CompileString(script.c_str(), "", Py_file_input);
    if (pCompiledFn == NULL)
    {
      QString errMsg(QObject::tr("Invalid script."));
      validateResult.messages.insert(validateResult.messages.end(), errMsg.toStdString());
      return;
    }

    // create a module
    PyObject *pModule = PyImport_ExecCodeModule((char *) "analysis", pCompiledFn);
    if (pModule == NULL)
    {
      QString errMsg(QObject::tr("Could not register the analysis function."));
      validateResult.messages.insert(validateResult.messages.end(), errMsg.toStdString());
      return;
    }

    boost::python::object analysisModule = boost::python::import("analysis");
    boost::python::object analysisFunction = analysisModule.attr("analysis");
  }
  catch (const error_already_set &)
  {
    std::string errMsg = extractException();
    validateResult.messages.insert(validateResult.messages.end(), errMsg);
  }
  catch(const std::exception& e)
  {
    validateResult.messages.insert(validateResult.messages.end(), e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    validateResult.messages.insert(validateResult.messages.end(), errMsg.toStdString());
  }
}

std::string terrama2::services::analysis::core::python::getCurrentExecutionDate()
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;

  //////////////////////////////////////////////////////
  // If this a monitored object analysis
  try
  {
    terrama2::services::analysis::core::verify::analysisMonitoredObject(analysis);
    terrama2::services::analysis::core::MonitoredObjectContextPtr context;
    try
    {
      context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
    }
    catch(const terrama2::Exception& e)
    {
      TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
      return "";
    }

    return terrama2::core::TimeUtils::getISOString(context->getStartTime());
  }
  catch (const terrama2::core::VerifyException&)
  {
  }

  //////////////////////////////////////////////////////
  // If this a grid analysis
  try
  {
    terrama2::services::analysis::core::verify::analysisGrid(analysis);
    terrama2::services::analysis::core::GridContextPtr context;
    try
    {
      context = ContextManager::getInstance().getGridContext(cache.analysisHashCode);
    }
    catch(const terrama2::Exception& e)
    {
      TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
      return "";
    }

    return terrama2::core::TimeUtils::getISOString(context->getStartTime());
  }
  catch (const terrama2::core::VerifyException&)
  {
  }

  contextManager.addError(cache.analysisHashCode, QObject::tr("Error in analysis type for analysis %1.").arg(analysis->id).toStdString());
  return "";
}

std::string terrama2::services::analysis::core::python::getAttributeValueAsJson(const std::string &attribute)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;

  try
  {
    terrama2::services::analysis::core::verify::analysisMonitoredObject(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return "";
  }

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return "";
  }


  try
  {
    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }


    std::shared_ptr<ContextDataSeries> moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    if(moDsContext->series.syncDataSet->size() == 0)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto attrProperty = moDsContext->series.teDataSetType->getProperty(attribute);
    if(attrProperty == nullptr)
    {
      QString errMsg(QObject::tr("The monitored object dataset does not contain an attribute with the name: %1.").arg(attribute.c_str()));
      throw InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    if(moDsContext->series.syncDataSet->isNull(cache.index, attribute))
      return "";

    QJsonObject json;


    switch(attrProperty->getType())
    {
      case te::dt::FLOAT_TYPE:
      {
        json.insert(QString::fromStdString(attribute), moDsContext->series.syncDataSet->getFloat(cache.index, attribute));
        break;
      }
      case te::dt::DOUBLE_TYPE:
      {
        json.insert(QString::fromStdString(attribute), moDsContext->series.syncDataSet->getDouble(cache.index, attribute));
        break;
      }
      case te::dt::INT16_TYPE:
      {
        json.insert(QString::fromStdString(attribute), moDsContext->series.syncDataSet->getInt16(cache.index, attribute));
        break;
      }
      case te::dt::INT32_TYPE:
      {
        json.insert(QString::fromStdString(attribute), moDsContext->series.syncDataSet->getInt32(cache.index, attribute));
        break;
      }
      case te::dt::INT64_TYPE:
      {
        json.insert(QString::fromStdString(attribute), static_cast<qint64>(moDsContext->series.syncDataSet->getInt64(cache.index, attribute)));
        break;
      }
      case te::dt::NUMERIC_TYPE:
      {
        json.insert(QString::fromStdString(attribute), QString::fromStdString(moDsContext->series.syncDataSet->getNumeric(cache.index, attribute).c_str()));
        break;
      }
      case te::dt::STRING_TYPE:
      {
        json.insert(QString::fromStdString(attribute), QString::fromStdString(moDsContext->series.syncDataSet->getString(cache.index, attribute)));
        break;
      }
      default:
        json.insert(QString::fromStdString(attribute), QString());
    }

    QJsonDocument doc(json);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    return strJson.toStdString();
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return "";
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    return "";
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return "";
  }

  return "";

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
