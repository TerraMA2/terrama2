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
  \file terrama2/collector/Exception.hpp

  \brief Exceptions for collector module.

  \author Jano Simas
*/

#include "../Exception.hpp"

#ifndef __TERRAMA2_COLLECTOR_EXCEPTION_HPP__
#define __TERRAMA2_COLLECTOR_EXCEPTION_HPP__

namespace terrama2
{
  namespace collector
  {
    struct Exception : virtual terrama2::Exception { };

    //! Base exception for CollectorService
    struct CollectorServiceException : virtual Exception { };

    //! Base exception for DataRetrieverFTP
    struct DataRetrieverFTPException : virtual Exception { };

    //! Raised when the collector service could not be started. Check error info for more information.
    struct UnableToStartServiceException : virtual CollectorServiceException { };
    //! Raised when start() is callend and the CollectorService is already running.
    struct ServiceAlreadyRunnningException: virtual UnableToStartServiceException { };

    //! Base exception for DataSetTimer
    struct DataSetTimerException: virtual Exception { };
    //! Raised when a dataset has id == 0 or doesn't have a name.
    struct InvalidDataSetException: virtual DataSetTimerException { };
    //! Raised when core::DataSet collection frequency is equal or lesser then zero.
    struct InvalidCollectFrequencyException: virtual DataSetTimerException { };

    //! Base exception for DataRetriever
    struct DataRetrieverException: virtual Exception { };
    //! Raised when dataprovider doesn't have an id or a name.
    struct InvalidDataProviderException: virtual DataRetrieverException { };

    //! Base exception to factory namespace.
    struct FactoryException: virtual Exception { };
    //! Raise when Parser type and uri scheme are incompatible.
    struct ConflictingParserTypeSchemeException: virtual FactoryException { };
    //! Raised when the Factory was unable to create a Parser. Check error message for more information.
    struct UnableToCreateParserException: virtual FactoryException { };
    //! Raised when the Factory was unable to create a Storager. Check error message for more information.
    struct UnableToCreateStoragerException: virtual FactoryException { };

    //! Base exception for Parser
    struct ParserException: virtual Exception { };
    //! Raised when a folder that doesn't exist is provided.
    struct InvalidFolderException: virtual ParserException { };
    //! Raised when there is no dataset to parse.
    struct NoDataSetFoundException: virtual ParserException { };

    //! Base exception for DataFilter
    struct DataFilterException: virtual Exception { };
    //! Raised when core::DataSetItem mask is empty.
    struct EmptyMaskException: virtual DataFilterException { };

    //! Raised when the DataSet could not be read. Check error info for more information.
    struct UnableToReadDataSetException: virtual ParserException { };

    //! Raised when failed to log.
    struct LogException: terrama2::Exception{ };
  }
}

#endif //__TERRAMA2_COLLECTOR_EXCEPTION_HPP__
