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

#include <boost/algorithm/string/replace.hpp>

terrama2::core::PythonInterpreter::PythonInterpreter()
  : Interpreter()
{
  interpreterState_ = Py_NewInterpreter();
}

terrama2::core::PythonInterpreter::~PythonInterpreter()
{
  Py_EndInterpreter(interpreterState_);
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

std::string terrama2::core::PythonInterpreter::extractException()
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

  object main = object(handle<>(borrowed(PyImport_AddModule("__main__"))));
  object nspace = main.attr("__dict__");
  object obj = nspace[name];

  extract<double> value(obj);

  return value.check() ? boost::optional<double>(value()) : boost::none;
}

boost::optional<std::string> terrama2::core::PythonInterpreter::getString(const std::string& name) const
{
  auto lock = holdState();

  using namespace boost::python;

  object main = object(handle<>(borrowed(PyImport_AddModule("__main__"))));
  object nspace = main.attr("__dict__");
  object obj = nspace[name];

  extract<std::string> value(obj);

  return value.check() ? boost::optional<std::string>(value()) : boost::none;
}

void terrama2::core::PythonInterpreter::runScript(const std::string& script)
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
  catch( error_already_set )
  {
    // extractException();
    PyErr_Print();
  }
}

terrama2::core::PythonInterpreter::StateLock terrama2::core::PythonInterpreter::holdState() const
{
  return StateLock(interpreterState_);
}

std::mutex terrama2::core::PythonInterpreter::StateLock::mutex_;

terrama2::core::PythonInterpreter::StateLock::StateLock(PyThreadState * state)
{
  mutex_.lock();
  gilState_ = PyGILState_Ensure();
  oldState_ = PyThreadState_Swap(state);
}

terrama2::core::PythonInterpreter::StateLock::~StateLock()
{
  PyThreadState_Swap(oldState_);
  PyGILState_Release(gilState_);
  mutex_.unlock();
}
