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
  \file terrama2/gui/Exception.hpp

  \brief Base exceptions TerraMA2 for gui module.

  \author Raphael Willian da Costa
  \author Evandro Delatin
 */


#ifndef __TERRAMA2_GUI_EXCEPTION_HPP__
#define __TERRAMA2_GUI_EXCEPTION_HPP__

#include "../Exception.hpp"

namespace terrama2
{
  namespace gui
  {
    //! Base GUI module exception
    struct Exception : virtual terrama2::Exception{ };

    //! It is used when some url is invalid
    struct URLError: virtual Exception{ };

    //! It is used when the input value is invalid
    struct FieldError: virtual Exception{ };

    //! It is used for specifics directory errors
    struct DirectoryError: virtual Exception{ };

    //! It is used for handling file error
    struct FileError: virtual Exception{ };

    //! It is used when connection has been failed
    struct ConnectionError: virtual Exception{ };

    //! It is used when the value in input is invalid or not expected.
    struct ValueError: virtual Exception{ };
  }
}

#endif // __TERRAMA2_GUI_EXCEPTION_HPP__
