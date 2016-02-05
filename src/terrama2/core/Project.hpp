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
  \file terrama2/coreProject.hpp

  \brief Encapsulates a TerraMA2 project information.

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_PROJECT_HPP__
#define __TERRAMA2_CORE_PROJECT_HPP__

#include "ServiceData.hpp"

//STL
#include <string>
#include <map>

//QT
#include <QJsonDocument>

namespace terrama2 {
  namespace core {

    struct ConnectionData
    {
      std::string host = "localhost";
      std::string port = "5432";
      std::string user;
      std::string password;
      std::string databaseName;
      std::string encoding = "UTF-8";
      std::string driver = "POSTGIS";
    };

    class Project
    {
    public:
      //! Default contructor, used for new projects.
      Project() {}//TODO: get current version
      //! Contruct from a project file.
      Project(const char* pathToProject);
      //! Contruct from a json.
      Project(const QJsonDocument& jsonProject);
      //! Copy constructor
      Project(const Project& project);
      Project& operator=(const Project& project);


      //! Name of the project.
      std::string name() const { return name_; }
      //! Set a new name for the project.
      void setName(const std::string& name) { name_ = name; }

      //! Version of creation of the project
      std::string version() const { return version_; }

      std::string logFile() const { return logFile_; }
      void setLogFile(const std::string& logFile) { logFile_ = logFile; }

      //! Connection data to a service
      ServiceData service(const std::string& serviceName) const { return serviceMap_.at(serviceName); }
      //! List of remote services information
      std::map<std::string, ServiceData> serviceList() const { return serviceMap_;}
      /*!
         \brief Add connection data to a remote service

         \exception //TODO: add exception to same name
       */
      void addServiceData(const ServiceData& serviceData);
      //! Remove a service from the project
      void removeService(const std::string& serviceName);

      //! Connection data to the database
      ConnectionData databaseData() const { return databaseConnectionData_;}
      //! Set connection data to the database
      void setDatabaseData(const ConnectionData& connectionData) { databaseConnectionData_ = connectionData; }

      //! Path to project file
      const std::string& path() const { return path_;}
      //! Set path to project file
      void setPath(const std::string& path);

      //! Write project to file
      void save();

      //! Creates a json document of the project
      const QJsonDocument&  toJson();

    private:
      void loadJsonProject(const QJsonDocument& jsonProject);

      std::string name_;
      std::string version_;
      std::string logFile_;

      ConnectionData databaseConnectionData_;

      std::map<std::string, ServiceData> serviceMap_;

      std::string path_;


      const QString nameTag = "name";
      const QString versionTag = "version";
      const QString logTag = "logFile";

      const QString databaseTag = "database";
      const QString databaseDriverTag = "driver";
      const QString databaseHostTag = "host";
      const QString databasePortTag = "port";
      const QString databaseNameTag = "name";
      const QString databaseUserTag = "user";
      const QString databasePasswordTag = "password";
      const QString databaseEncodingTag = "encoding";

      const QString servicesTag = "services";
      const QString serviceNameTag = "name";
      const QString serviceHostTag = "host";
      const QString serviceUserTag = "user";
      const QString serviceBinaryTag = "pathToBinary";
      const QString serviceTypeTag = "type";
      const QString servicePortTag = "port";
    };
  }
}

#endif// __TERRAMA2_CORE_PROJECT_HPP__
