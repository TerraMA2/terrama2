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
  \file terrama2/Exception.hpp

  \brief Base exception classes in TerraMA2.

  \author Gilberto Ribeiro de Queiroz
 */

#ifndef __TERRAMA2_EXCEPTION_HPP__
#define __TERRAMA2_EXCEPTION_HPP__

// STL
#include <stdexcept>
#include <string>

// Boost
#include <boost/exception/all.hpp>

// Qt
#include <QString>

namespace terrama2
{
  //! The base type for error report messages.
  typedef boost::error_info<struct tag_error_description, QString> ErrorDescription;

  //! Base exception class for TerraMA2.
  struct Exception: virtual std::exception, virtual boost::exception
  {
  public:
    virtual const char* what() const noexcept
    {
      return boost::get_error_info< terrama2::ErrorDescription >(*this)->toStdString().c_str();
    }
  };

  //! An exception indicating that a slot connection was not stablished.
  struct SlotConnectionError: virtual Exception { };

  //! An exception indicating that the client of the API is attempting to get a new instance.
  struct SingletonViolationError: virtual Exception { };

  //! An exception indicating that the singleton was not created.
  struct SingletonNotCreatedError: virtual Exception { };

  //! An exception indicating that the expected singleton type was not created.
  struct SingletonTypeError: virtual Exception { };

  //! An exception indicating an error during application initialization.
  struct InitializationError: virtual Exception { };

  //! An exception indicating a parser error.
  struct ParserError: virtual Exception { };

  //! An exception indicating a serialization error.
  struct SerializationError: virtual Exception { };

  //! An exception indicating a serialization error.
  struct FileOpenError: virtual Exception { };

  //! An exception indicating that an argument is missing or violating any requirement.
  struct InvalidArgumentError: virtual Exception { };

}  // end namespace terrama2

#endif  // __TERRAMA2_EXCEPTION_HPP__
