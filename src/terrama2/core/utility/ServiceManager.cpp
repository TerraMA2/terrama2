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

void terrama2::core::ServiceManager::setLogger(std::shared_ptr<terrama2::core::ProcessLogger> logger)
{
  if(!logger)
  {
    throw std::invalid_argument(tr("Error registering logger.").toStdString());
  }

  logger_ = logger;
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
  emit listeningPortUpdated(listeningPort_);
}
int terrama2::core::ServiceManager::listeningPort() const
{
  return listeningPort_;
}

void terrama2::core::ServiceManager::setNumberOfThreads(int numberOfThreads)
{
  numberOfThreads_ = numberOfThreads;
  emit numberOfThreadsUpdated(static_cast<size_t>(numberOfThreads_));
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
  obj.insert("service_loaded", serviceLoaded());

  if(serviceLoaded())
  {
    obj.insert("instance_id", static_cast<int>(instanceId()));
    obj.insert("instance_name", QString::fromStdString(instanceName()));
    obj.insert("start_time", QString::fromStdString(startTime_->toString()));
    obj.insert("terrama2_version",  QString::fromStdString(TERRAMA2_VERSION_STRING));
    obj.insert("shutting_down",  isShuttingDown_);
    obj.insert("logger_online",  logger_->isValid());
  }

  return obj;
}

void terrama2::core::ServiceManager::updateService(const QJsonObject& obj)
{
  setInstanceId(static_cast<ServiceInstanceId>(obj["instance_id"].toInt()));
  setInstanceName(obj["instance_name"].toString().toStdString());
  setListeningPort(obj["listening_port"].toInt());
  setNumberOfThreads(obj["number_of_threads"].toInt());
  auto logDatabaseObj = obj["log_database"].toObject();

  te::core::URI uri("pgsql://"
  +logDatabaseObj["PG_USER"].toString().toStdString()
  +":"+logDatabaseObj["PG_PASSWORD"].toString().toStdString()
  +"@"+logDatabaseObj["PG_HOST"].toString().toStdString()
  +":"+std::to_string(logDatabaseObj["PG_PORT"].toInt())
  +"/"+logDatabaseObj["PG_DB_NAME"].toString().toStdString());

  setLogConnectionInfo(uri);

  if(obj.contains("additional_info"))
  {
    emit additionalInfoUpdated(obj["additional_info"].toObject());
  }
}

void terrama2::core::ServiceManager::setLogConnectionInfo(const te::core::URI& logDbUri)
{
  logDbUri_ = logDbUri;
  logger_->setConnectionInfo(logDbUri);
}

te::core::URI terrama2::core::ServiceManager::logConnectionInfo() const
{
  return logDbUri_;
}

void terrama2::core::ServiceManager::setShuttingDownProcessInitiated()
{
  isShuttingDown_ = true;
}
