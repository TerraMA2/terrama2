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
  \file terrama2/coreProject.cpp

  \brief Encapsulates a TerraMA2 project information.

  \author Jano Simas
 */

#include "Project.hpp"
#include "Utils.hpp"

//QT
#include <QJsonObject>
#include <QJsonArray>

terrama2::core::Project::Project(const char* pathToProject)
{
  QJsonDocument jdoc = terrama2::core::ReadJsonFile(pathToProject);
  loadJsonProject(jdoc);
}

terrama2::core::Project::Project(const QJsonDocument& jsonProject)
{
  loadJsonProject(jsonProject);
}

terrama2::core::Project::Project(const terrama2::core::Project& project)
  : name_(project.name_),
    version_(project.version_),
    logFile_(project.logFile_),
    databaseConnectionData_(project.databaseConnectionData_),
    serviceMap_(project.serviceMap_)
{
}

terrama2::core::Project& terrama2::core::Project::operator=(const terrama2::core::Project& project)
{
  name_ = project.name_;
  version_ = project.version_;
  logFile_ = project.logFile_;

  databaseConnectionData_ = project.databaseConnectionData_;

  serviceMap_ = project.serviceMap_;

  return *this;
}

const QJsonDocument&terrama2::core::Project::toJson()
{

}

void terrama2::core::Project::loadJsonProject(const QJsonDocument& jsonProject)
{
  QJsonObject localObject = jsonProject.object();
  name_ = localObject.value(nameTag).toString().toStdString();
  version_ = localObject.value(versionTag).toString().toStdString();
  logFile_ = localObject.value(logTag).toString("terrama2.log").toStdString();

  QJsonObject database = localObject.value(databaseTag).toObject();

  databaseConnectionData_.databaseName = database.value(databaseNameTag).toString().toStdString();
  databaseConnectionData_.host = database.value(databaseHostTag).toString("localhost").toStdString();
  databaseConnectionData_.port = database.value(databasePortTag).toString("5432").toStdString();
  databaseConnectionData_.user = database.value(databaseUserTag).toString().toStdString();
  databaseConnectionData_.password = database.value(databasePasswordTag).toString().toStdString();
  databaseConnectionData_.encoding = database.value(databaseEncodingTag).toString("UTF-8").toStdString();
  databaseConnectionData_.driver = database.value(databaseDriverTag).toString("POSTGIS").toStdString();

  QJsonArray serviceArray = localObject.value(servicesTag).toArray();
  for(const auto& value : serviceArray)
  {
    QJsonObject serviceObj = value.toObject();
    ServiceData service;
    service.name = serviceObj.value(serviceNameTag).toString().toStdString();
    service.host = serviceObj.value(serviceHostTag).toString("localhost").toStdString();
    service.user = serviceObj.value(serviceUserTag).toString().toStdString();
    service.type = static_cast<ServiceData::ServiceType>(serviceObj.value(serviceTypeTag).toInt());
    service.pathToBinary = serviceObj.value(serviceBinaryTag).toString().toStdString();
    service.servicePort = serviceObj.value(servicePortTag).toInt();

    serviceMap_.emplace(service.name, service);
  }
}
