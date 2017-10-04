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
  \file terrama2/core/utility/InterpreterFactory.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_INTERPRETER_FACTORY_HPP__
#define __TERRAMA2_CORE_INTERPRETER_FACTORY_HPP__

#include "../Shared.hpp"
#include "Interpreter.hpp"

// TerraLib
#include <terralib/common/Singleton.h>

// STL
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace terrama2
{
  namespace core
  {
    class InterpreterFactory : public te::common::Singleton<InterpreterFactory>
    {
      public:
        //! DataStorager constructor function.
        typedef std::function<terrama2::core::InterpreterPtr()> FactoryFnctType;
        typedef std::function<terrama2::core::InterpreterRAII()> RaiiInterpreterFnctType;
        //! Register a new DataStorager constructor associated with the DataProviderType.
        void add(const terrama2::core::InterpreterType& interpreterType, FactoryFnctType f, RaiiInterpreterFnctType raiiFunction = nullptr);
        //! Remove the DataStorager constructor associated with the DataProviderType.
        void remove(const terrama2::core::InterpreterType& interpreterType);
        //! Returns if exists a factory for the given format.
        bool find(const terrama2::core::InterpreterType& interpreterType);
        /*!
          \brief Creates a DataStorager

          The DataStorager is constructed based on the format of the DataSeries.
        */
        terrama2::core::InterpreterPtr make(const terrama2::core::InterpreterType& interpreterType) const;


      protected:
        friend class te::common::Singleton<InterpreterFactory>;
        //! Default constructor
        InterpreterFactory() {}
        //! Default destructor
        ~InterpreterFactory() {}

        InterpreterFactory(const InterpreterFactory& other) = delete;
        InterpreterFactory(InterpreterFactory&& other) = delete;
        InterpreterFactory& operator=(const InterpreterFactory& other) = delete;
        InterpreterFactory& operator=(InterpreterFactory&& other) = delete;

        std::map<terrama2::core::InterpreterType, FactoryFnctType> factoriesMap_;

        //! List of raii structures to finalize the interpreters.
        std::vector<InterpreterRAII> raiiInterpreterVector_;
    };


  } /* core */

} /* terrama2 */

#endif // __TERRAMA2_CORE_INTERPRETER_FACTORY_HPP__
