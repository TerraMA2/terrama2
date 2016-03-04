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
  \file terrama2/core/ServiceData.hpp

  \brief Remote service connection information.

  \author Jano Simas
*/


#ifndef __TERRAMA2_CORE_SERVICE_DATA_HPP__
#define __TERRAMA2_CORE_SERVICE_DATA_HPP__


#include <string>

namespace terrama2 {
  namespace core {
    //! Remote service connection information.
    struct ServiceData
    {
      //! Types of services recognized by the ServiceManager.
      enum ServiceType
      {
        VOID = 0,//!< Control value, invalid service.
        COLLECTOR = 1,//!< Service to collect and integrate data.
        ANALYSIS = 2,//!< Service to analyse data.
        ALERT = 3//!< Service for warning and alerts.
      };

      std::string name;//!< Unique name for the service.

      std::string host;//!< Host to run the service.
      std::string user;//!< Username used connect to the remote machine and start the service.
      std::string pathToBinary;//!< Path to the binary of the service.

      ServiceType type;//!< Type of the service.
      int servicePort;//!< Port the service should listen.

    };
  }
}

#endif //__TERRAMA2_CORE_SERVICE_DATA_HPP__
