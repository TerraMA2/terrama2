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
  \file terrama2/core/Exception.hpp

  \brief Specific exception classes from core module.

  \author Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_CORE_EXCEPTION_HPP__
#define __TERRAMA2_CORE_EXCEPTION_HPP__

// TerraMA2
#include "../Exception.hpp"

namespace terrama2
{
  namespace core
  {
    //! Base excection for core module.
    struct Exception : virtual terrama2::Exception { };


    //! Raised when the data manager is not a valid pointer.
    struct InvalidDataManagerException: virtual Exception {};

    //#################################
    //     Data Access Module Exceptions

    struct DataAccessException: virtual Exception{ };

    //! Base Exception for DataAccessor
    struct DataAccessorException: virtual DataAccessException { };
    //! Raised when the DataProvider is innactive and SHOULD be active
    struct DataProviderException: virtual DataAccessorException { };
    //! Raised when trying to retrieve data from a non retrivable Retriever
    struct NotRetrivableException: virtual DataAccessorException { };
    //! Raised when a format value is accessed and not set.
    struct UndefinedTagException: virtual DataAccessorException { };
    //! Raised when there is no data to be accessed
    struct NoDataException: virtual DataAccessorException { };
    //! Raised when a DataAccessor trys to access data from a DataSeries with a different semantics
    struct WrongDataSeriesSemanticsException: virtual DataAccessorException { };

    //! Base Exception for DataStorager
    struct DataStoragerException: virtual DataAccessException { };

    //#################################
    // Implementation Module Exceptions
    //! Base exception for DataRetrieverFTP
    struct DataRetrieverFTPException : virtual Exception { };

    //#################################
    //     Data Model Module Exceptions

    struct DataModelException: virtual Exception{ };

    struct DataManagerException: virtual DataModelException{ };

    //#################################

    //#################################
    //     Network Module Exceptions

    struct NetworkException: virtual Exception{ };

    //#################################

    //#################################
    //     Utility Module Exceptions

    struct UtilityException: virtual Exception{ };

    //! Raised when the receives an invalid collect frequency
    struct InvalidCollectFrequencyException: virtual UtilityException { };

    //! An exception indicating a parser error.
    struct JSonParserException: virtual UtilityException { };

    struct ServiceException: virtual UtilityException { };

    struct SemanticsException: virtual UtilityException { };

    //#################################

  }  // end namespace core
}  // end namespace terrama2

#endif  // __TERRAMA2_CORE_EXCEPTION_HPP__
