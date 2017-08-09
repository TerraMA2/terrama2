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
  \file terrama2/services/analysis/core/python/PythonBindingMonitoredObject.hpp

  \brief

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHON_MONITORED_OBJECT_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHON_MONITORED_OBJECT_HPP__

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
          namespace DCP
          {
            /*!
              \brief Registers all functions for monitored object analysis.
            */
            void registerFunctions();

            /*!
              \brief Registers DCP functions in the Python interpreter.
            */
            void registerDCPFunctions();

            /*!
              \brief Registers DCP zonal functions in the Python interpreter.
            */
            void registerDCPZonalFunctions();

            /*!
              \brief Registers DCP history functions in the Python interpreter.
            */
            void registerDCPZonalHistoryFunctions();

            /*!
              \brief Registers DCP history interval functions in the Python interpreter.
            */
            void registerDCPZonalHistoryIntervalFunctions();

            /*!
              \brief Registers DCP influence functions in the Python interpreter.
            */
            void registerDCPZonalInfluenceFunctions();

          } /* MonitoredObject */
        } /* python */
      }
    }
  }
}

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHON_MONITORED_OBJECT_HPP__
