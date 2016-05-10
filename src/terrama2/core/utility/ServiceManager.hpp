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
  \file terrama2/core/utility/ServiceManager.hpp
  \brief
  \author Jano Simas
*/


#ifndef __TERRAMA2_CORE_SERVICE_MANAGER_HPP__
#define __TERRAMA2_CORE_SERVICE_MANAGER_HPP__

#include "../Typedef.hpp"

//Qt
#include <QJsonObject>
#include <QObject>

// TerraLib
#include <terralib/common/Singleton.h>
#include <terralib/datatype/TimeInstantTZ.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Controls service instance information.

      The ServiceManager class holds instance information as id and name.

      When the listening port is altered, the ServiceManager will send a signal to notify the TcpManager.

    */
    class ServiceManager : public QObject, public te::common::Singleton<ServiceManager>
    {
        Q_OBJECT

      public:
        //! Return true if the service information was received and loaded.
        virtual bool serviceLoaded() const;

        //! Set the human readable name used to identify the instance.
        void setInstanceName(const std::string& instanceName);
        //! Return the human readable name used to identify the instance.
        virtual const std::string& instanceName() const;

        //! Set the unique identification of the instance.
        void setInstanceId(ServiceInstanceId instanceId);
        //! Return the unique identification of the instance.
        virtual ServiceInstanceId instanceId() const;

        //! Set the type of the service running in this instance.
        void setServiceType(const std::string& serviceType);
        //! Return the type of the service running in this instance.
        virtual const std::string& serviceType() const;

        /*!
          \brief Set the port beeing listened for remote signals.

          This methos will emit a ServiceManager::listeningPortUpdated signal, the should be received by the TcpManager.
        */
        void setListeningPort(int listeningPort);
        //! Return the port beeing listened for remote signals.
        virtual int listeningPort() const;

        void setNumberOfThreads(int numberOfThreads);
        virtual int numberOfThreads() const;

        //! Return the TerraMA2 version of the runnning instance.
        virtual const std::string& terrama2Version() const;
        //! Return the Date/Time when the service was started.
        virtual const std::shared_ptr< te::dt::TimeInstantTZ >& startTime() const;

        /*!
          \brief Updates the service information.

          This method receives a json with the updated information,
          it may update the ServiceInstanceId, name and the listening port.

          Valid tags are:
            - instance_id
            - instance_name
            - listening_port
            - number_of_threads
        */
        void updateService(const QJsonObject& obj);
        /*!
          \brief Get the status of the service.

          This method will return a JSon object with the running service information.
          This will include the ServiceInstanceId, name, the Date/Time when the service was started and the version of the service.
        */
        virtual QJsonObject status() const;

        void setLogConnectionInfo(std::map<std::string, std::string> connInfo);
        virtual std::map<std::string, std::string> logConnectionInfo() const;


      signals:
        //! Signal emited when the listening is changed
        void listeningPortUpdated(int);
        void numberOfThreadsUpdated(int);

      protected:
        friend class te::common::Singleton<ServiceManager>;

        //! Constructor, initialize the start time of the service.
        ServiceManager();

        virtual ~ServiceManager() = default;
        ServiceManager(const ServiceManager& other) = delete;
        ServiceManager(ServiceManager&& other) = delete;
        ServiceManager& operator=(const ServiceManager& other) = delete;
        ServiceManager& operator=(ServiceManager&& other) = delete;

        std::string instanceName_;
        ServiceInstanceId instanceId_ = 0;
        std::string serviceType_;
        int listeningPort_;
        int numberOfThreads_;
        const std::string terrama2Version_ = "TerraMA2-4-alpha2";//FIXME: use the global version
        std::shared_ptr< te::dt::TimeInstantTZ > startTime_;
        bool serviceLoaded_ = false;
        std::map<std::string, std::string> connInfo_;
    };
  }
}

#endif //__TERRAMA2_CORE_SERVICE_MANAGER_HPP__
