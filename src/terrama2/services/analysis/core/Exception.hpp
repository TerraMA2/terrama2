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
  \file terrama2/services/analysis/core/Exception.hpp

  \brief Analysis module Exceptions.

  \author Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_EXCEPTION_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_EXCEPTION_HPP__

// TerraMA2
#include "../../../core/Exception.hpp"


namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        //! Base exception class for analysis module.
        struct Exception: virtual terrama2::core::Exception {};

        //! To be updated when an invalid frequency is detected in an analysis.
        struct InvalidFrequencyException: virtual Exception {};

        //! To be used when an invalid parameter is given.
        struct InvalidParameterException: virtual Exception {};

        //! To be used when an invalid dataset is dectected.
        struct InvalidDataSetException: virtual Exception {};

        //! To be used when an empty data series is dectected.
        struct EmptyDataSeriesException: virtual Exception {};

        //! To be used when an invalid data series is dectected.
        struct InvalidDataSeriesException: virtual Exception {};

        //! To be used when the monitored object data
        struct PythonInterpreterException: virtual Exception {};

        //! An exception indicating that an analysis returned an empty result.
        struct EmptyResultException: virtual Exception { };

        //! Base exception for the ContextManager.
        struct ContextManagerException: virtual Exception { };

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif  // __TERRAMA2_SERVICES_ANALYSIS_CORE_EXCEPTION_HPP__
