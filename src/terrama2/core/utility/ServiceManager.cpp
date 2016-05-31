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
  \file terrama2/core/utility/ServiceManager.cpp
  \brief
  \author Jano Simas
*/

#include "TimeUtils.hpp"
#include "ServiceManager.hpp"
#include "../../Version.hpp"

terrama2::core::ServiceManager::ServiceManager()
 : startTime_(terrama2::core::TimeUtils::nowUTC())
{
}

bool terrama2::core::ServiceManager::serviceLoaded() const
{
  return serviceLoaded_;
}


void terrama2::core::ServiceManager::setInstanceName(const std::string& instanceName)
{
  instanceName_ = instanceName;
}
const std::string& terrama2::core::ServiceManager::instanceName() const
{
  return instanceName_;
}

void terrama2::core::ServiceManager::setInstanceId(ServiceInstanceId instanceId)
{
  instanceId_ = instanceId;
  serviceLoaded_ = true;
}
ServiceInstanceId terrama2::core::ServiceManager::instanceId() const
{
  return instanceId_;
}

void terrama2::core::ServiceManager::setServiceType(const std::string& serviceType)
{
  serviceType_ = serviceType;
}
const std::string& terrama2::core::ServiceManager::serviceType() const
{
  return serviceType_;
}

void terrama2::core::ServiceManager::setListeningPort(int listeningPort)
{
  listeningPort_ = listeningPort;
  listeningPortUpdated(listeningPort_);
}
int terrama2::core::ServiceManager::listeningPort() const
{
  return listeningPort_;
}

void terrama2::core::ServiceManager::setNumberOfThreads(int numberOfThreads)
{
  numberOfThreads_ = numberOfThreads;
  numberOfThreadsUpdated(numberOfThreads_);
}
int terrama2::core::ServiceManager::numberOfThreads() const
{
  return numberOfThreads_;
}

const std::shared_ptr< te::dt::TimeInstantTZ >& terrama2::core::ServiceManager::startTime() const
{
  return startTime_;
}
QJsonObject terrama2::core::ServiceManager::status() const
{
  QJsonObject obj;
  obj.insert("instance_id", static_cast<int>(instanceId()));
  obj.insert("instance_name", QString::fromStdString(instanceName()));
  obj.insert("start_time", QString::fromStdString(startTime_->toString()));
  obj.insert("terrama2_version",  QString::fromStdString(TERRAMA2_VERSION_STRING));
  obj.insert("shutting_down",  isShuttingDown_);
  //TODO: Define status message
  return obj;
}

void terrama2::core::ServiceManager::updateService(const QJsonObject& obj)
{
  setInstanceId(obj["instance_id"].toInt());
  setInstanceName(obj["instance_name"].toString().toStdString());
  setListeningPort(obj["listening_port"].toInt());
  setNumberOfThreads(obj["number_of_threads"].toInt());
  auto logDatabaseObj = obj["log_database"].toObject();

  std::map<std::string, std::string> connInfo { {"PG_HOST", logDatabaseObj["PG_HOST"].toString().toStdString()},
                                                {"PG_PORT", std::to_string(logDatabaseObj["PG_PORT"].toInt())},
                                                {"PG_USER", logDatabaseObj["PG_USER"].toString().toStdString()},
                                                {"PG_PASSWORD", logDatabaseObj["PG_PASSWORD"].toString().toStdString()},
                                                {"PG_DB_NAME", logDatabaseObj["PG_DB_NAME"].toString().toStdString()},
                                                {"PG_CONNECT_TIMEOUT", "4"},
                                                {"PG_CLIENT_ENCODING", "UTF-8"}
                                              };
  setLogConnectionInfo(connInfo);
}

void terrama2::core::ServiceManager::setLogConnectionInfo(std::map<std::string, std::string> connInfo)
{
  connInfo_ = connInfo;
  emit logConnectionInfoUpdated(connInfo);
}

std::map<std::string, std::string> terrama2::core::ServiceManager::logConnectionInfo() const
{
  return connInfo_;
}

void terrama2::core::ServiceManager::setShuttingDownProcessInitiated()
{
  isShuttingDown_ = true;
}
