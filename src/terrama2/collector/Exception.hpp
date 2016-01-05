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

    //! Base exception for CollectorService exceptions
    struct CollectorServiceException : virtual Exception { };

    //! Base exception for DataRetrieverFTP excetptions
    struct DataRetrieverFTPException : virtual Exception { };

    /*!
       \brief Raised when the collector service could not be started.

       Check error info for more information.

     */
    struct UnableToStartServiceException : virtual CollectorServiceException { };
    //! Raised when start() is callend and the CollectorService is already running.
    struct ServiceAlreadyRunnningException: virtual UnableToStartServiceException { };


    //! Base exception for DataSetTimer exceptions
    struct DataSetTimerException: virtual Exception { };
    //! Raised when a dataset has id == 0 or doesn't have a name.
    struct InvalidDataSetException: virtual DataSetTimerException { };
    struct InvalidCollectFrequencyException: virtual DataSetTimerException { };


    //! Base exception for DataRetriever exceptions
    struct DataRetrieverException: virtual Exception { };
    //! Raised when dataprovider doesn't have an id or a name.
    struct InvalidDataProviderException: virtual DataRetrieverException { };

    struct WrongDataProviderKindException: virtual DataRetrieverException { };

    //! Base exception to factory namespace.
    struct FactoryException: virtual Exception { };
    struct ConflictingParserTypeSchemeException: virtual FactoryException { };
    struct UnableToCreateParserException: virtual FactoryException { };
    struct UnableToCreateStoragerException: virtual FactoryException { };

    //! Base exception for ParserError exceptions
    struct ParserException: virtual Exception { };
    struct InvalidFolderException: virtual ParserException { };
    struct NoDataSetFoundException: virtual ParserException { };

    //! Base exception for DataFilter exceptions
    struct DataFilterException: virtual Exception { };
    struct EmptyMaskException: virtual DataFilterException { };

    /*!
       \brief Raised when the DataSet could not be read.

       Check error info for more information.

     */
    struct UnableToReadDataSetException: virtual ParserException { };


    /*!
       \brief Raised when failed to log.

     */
    struct LogException: terrama2::Exception{ };
  }
}

#endif //__TERRAMA2_COLLECTOR_EXCEPTION_HPP__
