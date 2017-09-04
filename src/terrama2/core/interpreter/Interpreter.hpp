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
  \file terrama2/core/interpreter/Interpreter.hpp

  \brief Base class for interpreter classes

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_INTERPRETER_HPP__
#define __TERRAMA2_CORE_INTERPRETER_HPP__

#include <string>

#include <boost/optional.hpp>

namespace terrama2
{
  namespace core
  {
    using InterpreterType = std::string;
    /*!
      \brief Interpreter encapsulating class

      Creates an interpreter instance and ensure isolation.
    */
    class Interpreter
    {
    public:
      //! Creates a python interpreter instance
      Interpreter() = default;
      //! Free the interpreter instance
      virtual ~Interpreter() = default;
      Interpreter(const Interpreter& other) = delete;
      Interpreter(Interpreter&& other) = delete;
      Interpreter& operator=(const Interpreter& other) = delete;
      Interpreter& operator=(Interpreter&& other) = delete;

      /*!
        \brief Register a double value at the interpreter.

        The value is registered at the global scope and is available to the scripts
      */
      virtual void setDouble(const std::string& name, const double value) = 0;

      /*!
        \brief Register a string value at the interpreter.

        The value is registered at the global scope and is available to the scripts
      */
      virtual void setString(const std::string& name, const std::string value) = 0;
      /*!
        \brief Recover a numeric value from the interpreter.

        If the value doesn't exist return a boost::none.
      */
      virtual boost::optional<double> getNumeric(const std::string& name) const = 0;

      /*!
        \brief Recover a string value from the interpreter.

        If the value doesn't exist return a boost::none.
      */
      virtual boost::optional<std::string> getString(const std::string& name) const = 0;
      /*!
        \brief Execute the script in the interpreter instace.

          - any former modification is still valid.
          - any alteration of the instance is persisted.
      */
      virtual void runScript(const std::string& script) = 0;
    };
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_INTERPRETER_HPP__
