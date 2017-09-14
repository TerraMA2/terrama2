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
  \file terrama2/core/interpreter/PythonInterpreter.hpp

  \brief class for the python interpreter

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_PYTHON_INTERPRETER_HPP__
#define __TERRAMA2_CORE_PYTHON_INTERPRETER_HPP__

#include "Interpreter.hpp"
#include "../Shared.hpp"

#include <memory>
#include <mutex>

struct StateLock;

namespace terrama2
{
  namespace core
  {
    class PythonInterpreter : public Interpreter
    {
    public:
      PythonInterpreter();
      virtual ~PythonInterpreter();
      PythonInterpreter(const PythonInterpreter& other) = delete;
      PythonInterpreter(PythonInterpreter&& other) = delete;
      PythonInterpreter& operator=(const PythonInterpreter& other) = delete;
      PythonInterpreter& operator=(PythonInterpreter&& other) = delete;

      virtual void setDouble(const std::string& name, const double value);
      virtual void setString(const std::string& name, const std::string value);

      virtual boost::optional<double> getNumeric(const std::string& name) const;
      virtual boost::optional<std::string> getString(const std::string& name) const;
      virtual void runScript(const std::string& script);
      virtual std::string runScriptWithStringResult(const std::string& script, const std::string& variableToReturn);

      static InterpreterType interpreterType() { return "PYTHON"; }
      static InterpreterPtr make();

    private:
      std::string extractException() const;

      //! Lock the instance and aquire the current state.
      StateLock holdState() const;
      struct Impl;
      std::unique_ptr<Impl> impl_;
    };
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_PYTHON_INTERPRETER_HPP__
