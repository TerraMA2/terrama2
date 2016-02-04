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
  \file terrama2/core/ServiceManager.hpp

  \brief Manages the instance of remote services.

  \author Jano Simas
*/


#ifndef __TERRAMA2_CORE_SERVICE_MANAGER_HPP__
#define __TERRAMA2_CORE_SERVICE_MANAGER_HPP__

#include "DataManagerIntermediator.hpp"
#include "TcpDispatcher.hpp"
#include "ServiceData.hpp"

//STL
#include <string>

namespace terrama2
{
  namespace core
  {
    class DataManager;

    /*!
     \brief Manages the instance of remote services.
    */
    class ServiceManager
    {
    public:
      //! Constructor, connect to core::DataManager signals.
      ServiceManager(DataManager* dataManager);
      //! Creates a new service based on the ServiceData.
      void addService(const ServiceData& serviceData);
      //! Creates services based on a json array of service configuration..
      void addJsonServices(const QJsonArray& servicesArray);
      //! Removes an existing service.
      void removeService(const std::string& instanceName);
      //! Creates a DataManagerIntermediator for the service with name instanceName.
      DataManagerIntermediator intermediator(const std::string& instanceName);
      //! Returns the TcpDispatcher for the  the service with name instanceName.
      TcpDispatcherPtr tcpDispatcher(const std::string& instanceName);

    private:
      std::map<std::string, ServiceData> serviceDataMap_;//!< Map from instance name to service data.
      std::map<std::string, TcpDispatcherPtr> dispatcherMap_;//!< Map from instance name to TcpDispatcher.
    };
  }
}

#endif //__TERRAMA2_CORE_SERVICE_MANAGER_HPP__
