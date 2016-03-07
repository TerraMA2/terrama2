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
  \file terrama2/core/data-model/ServiceInstance.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_MODEL_SERVICE_INSTANCE_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_SERVICE_INSTANCE_HPP__

#include "../Config.hpp"

namespace terrama2
{
  namespace core
  {
    class Project
    {
      enum ServiceType
      {
        COLLECTOR_SERVICE,//!< Service responsible of retrieving and storaging data.
        ANALYSIS_SERVICE,//<! Service responsible for processing data
        ALERT_SERVICE//!< Service responsible for reports and risk alerts
      }
      
      uint64_t id; //!< Instance metadata unique identifier.
      ServiceType service_type; //!< Defines the type of service that must be instantiated.
      std::string name;//!< Service instance unique name. Human readable.
      std::string description;//!< Instance description.
      std::string host;//!< Host where the service must be instantiated.
      uint64_t port;//!< Port the service should listed to TCP comunication.
      std::string ssh_user;//!< User used to connect to the host when instantiating.
      uint64_t ssh_port;//!< Port used to connect to the host when instantiating.
      uint64_t number_of_threads;//!< Number of threads the service may use simultaneously.
      std::string path_to_binary;//!< Path to the service executable binary.
      std::string run_environment;//!< Environment variable used when starting the service.
    };
  }
}

#endif// __TERRAMA2_CORE_DATA_MODEL_PROJECT_HPP__
