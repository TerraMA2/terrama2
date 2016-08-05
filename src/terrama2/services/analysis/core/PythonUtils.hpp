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
  \file terrama2/services/analysis/core/PythonUtils.hpp

  \brief Python utility functions for TerraMA2 Analysis module.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHONUTILS_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHONUTILS_HPP__

// STL
#include <vector>

// Boost
#include <boost/python/stl_iterator.hpp>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace python
        {
          template<typename T>
          void pythonToVector(boost::python::object o, std::vector<T>& vec)
          {
            boost::python::stl_input_iterator<T> begin(o);
            boost::python::stl_input_iterator<T> end;

            vec.insert(vec.end(), begin, end);
          }
        }   // end namespace python
      }     // end namespace core
    }       // end namespace analysis
  }         // end namespace services
}           // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHONUTILS_HPP__
