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
  \file terrama2/services/alert/core/Exception.hpp

  \brief Base exception classes in TerraMA2.

  \author Gilberto Ribeiro de Queiroz
 */

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_EXCEPTION_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_EXCEPTION_HPP__

// TerraMA2
#include "../../../core/Exception.hpp"

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      //! Base exception class for Alert Module exceptions.
      struct Exception: virtual terrama2::core::Exception{};
      //! Base exception class for Notifier exceptions.
      struct NotifierException: virtual Exception{};
      //! Base exception class for Report exceptions.
      struct ReportException: virtual Exception{};
      //! Base exception class for AdditionalDataException exceptions.
      struct AdditionalDataException: virtual Exception{};
      //! Base exception class for ImageGenerationException exceptions.
      struct ImageGenerationException: virtual Exception{};
    }
  } /* services */
}  // end namespace terrama2

#endif  // __TERRAMA2_SERVICES_ALERT_CORE_EXCEPTION_HPP__
