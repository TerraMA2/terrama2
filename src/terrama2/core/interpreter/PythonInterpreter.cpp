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
  \file terrama2/core/interpreter/PythonInterpreter.cpp

  \brief Implementation of the python interpreter

  \author Jano Simas
*/

#include "PythonInterpreter.hpp"
#include "../utility/Logger.hpp"

// Boost
#include <boost/algorithm/string/replace.hpp>
#include <boost/python.hpp>

struct StateLock
{
  public:
    StateLock(PyThreadState * state);
    virtual ~StateLock();
    StateLock(const StateLock& other) = delete;
    StateLock(StateLock&& other) = default;
    StateLock& operator=(const StateLock& other) = delete;
    StateLock& operator=(StateLock&& other) = default;

  protected:
    static std::mutex mutex_;
    PyThreadState *oldState_ = nullptr;
    PyGILState_STATE gilState_;
};

struct terrama2::core::PythonInterpreter::Impl
{
  PyThreadState *interpreterState_;
  PyThreadState *mainThreadState_;
};

terrama2::core::PythonInterpreter::PythonInterpreter()
  : Interpreter(),
  impl_(new Impl())
{
  impl_->mainThreadState_ = PyThreadState_Get();
  StateLock lock(impl_->mainThreadState_);

  impl_->interpreterState_ = Py_NewInterpreter();
}

terrama2::core::PythonInterpreter::~PythonInterpreter()
{
  auto lock = holdState();
  Py_EndInterpreter(impl_->interpreterState_);
}

void terrama2::core::PythonInterpreter::setDouble(const std::string& name, const double value)
{
  auto lock = holdState();

  using namespace boost::python;

  object main = object(handle<>(borrowed(PyImport_AddModule("__main__"))));
  object nspace = main.attr("__dict__");

  nspace[name] = value;
}

void terrama2::core::PythonInterpreter::setString(const std::string& name, const std::string value)
{
  auto lock = holdState();

  using namespace boost::python;

  object main = object(handle<>(borrowed(PyImport_AddModule("__main__"))));
  object nspace = main.attr("__dict__");

  nspace[name] = value;
}

std::string terrama2::core::PythonInterpreter::extractException() const
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

boost::optional<double> terrama2::core::PythonInterpreter::getNumeric(const std::string &name) const
{
  auto lock = holdState();

  using namespace boost::python;

  try
  {
    object main = object(handle<>(borrowed(PyImport_AddModule("__main__"))));
    object nspace = main.attr("__dict__");
    object obj = nspace[name];

    extract<double> value(obj);

    return value.check() ? boost::optional<double>(value()) : boost::none;
  }
  catch(const error_already_set&)
  {
    auto errMsg = extractException();
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::InterpreterException() << ErrorDescription(QString::fromStdString(errMsg));
  }
}

boost::optional<std::string> terrama2::core::PythonInterpreter::getString(const std::string& name) const
{
  auto lock = holdState();

  using namespace boost::python;

  try
  {
    object main_module = import("__main__");
    // load the dictionary object out of the main module
    object main_namespace = main_module.attr("__dict__");
    object obj = main_namespace[name.c_str()];

    extract<const char*> value(str(obj).encode("utf-8"));

    return value.check() ? boost::optional<std::string>(value()) : boost::none;
  }
  catch(const error_already_set&)
  {
    auto errMsg = extractException();
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::InterpreterException() << ErrorDescription(QString::fromStdString(errMsg));
  }
}

void terrama2::core::PythonInterpreter::runScript(const std::string& script)
{
  {
    auto lock = holdState();

    using namespace boost::python;

    try
    {
      object main = object(handle<>(borrowed(PyImport_AddModule("__main__"))));
      object nspace = main.attr("__dict__");

      handle<> ignored(( PyRun_String( script.c_str(),
                                       Py_file_input,
                                       nspace.ptr(),
                                       nspace.ptr() ) ));
    }
    catch(const error_already_set&)
    {
      auto errMsg = extractException();
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::InterpreterException() << ErrorDescription(QString::fromStdString(errMsg));
    }
  }
}

std::string terrama2::core::PythonInterpreter::runScriptWithStringResult(const std::string& script, const std::string& variableToReturn)
{
  auto lock = holdState();

  using namespace boost::python;

  try
  {
    PyObject *main = PyImport_AddModule("__main__");
    PyRun_SimpleString(script.c_str());
    PyObject *catcher = PyObject_GetAttrString(main, variableToReturn.c_str());
    std::string out = PyString_AsString(catcher);

    return out;
  }
  catch(const error_already_set&)
  {
    auto errMsg = extractException();
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::InterpreterException() << ErrorDescription(QString::fromStdString(errMsg));
  }
}

StateLock terrama2::core::PythonInterpreter::holdState() const
{
  return StateLock(impl_->interpreterState_);
}

terrama2::core::InterpreterPtr terrama2::core::PythonInterpreter::make()
{
  return std::make_shared<terrama2::core::PythonInterpreter>();
}

std::mutex StateLock::mutex_;

StateLock::StateLock(PyThreadState * state)
{
  mutex_.lock();
  oldState_ = PyThreadState_Get();
  PyEval_AcquireLock();
  PyThreadState_Swap(state);
}

StateLock::~StateLock()
{
  PyEval_ReleaseLock();
  PyThreadState_Swap(oldState_);
  mutex_.unlock();
}
