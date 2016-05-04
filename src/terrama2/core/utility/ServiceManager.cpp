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

terrama2::core::ServiceManager::ServiceManager()
 : startTime_(terrama2::core::TimeUtils::nowUTC())
{
}

void terrama2::core::ServiceManager::setInstanceName(const std::string& instanceName)
{
  instanceName_ = instanceName;
}
const std::string& terrama2::core::ServiceManager::instanceName() const
{
  return instanceName_;
}

void terrama2::core::ServiceManager::setInstanceId(int instanceId)
{
  instanceId_ = instanceId;
}
int terrama2::core::ServiceManager::instanceId() const
{
  return instanceId_;
}

void terrama2::core::ServiceManager::setServiceType(ServiceType serviceType)
{
  serviceType_ = serviceType;
}
terrama2::core::ServiceType terrama2::core::ServiceManager::serviceType() const
{
  return serviceType_;
}

void terrama2::core::ServiceManager::setListeningPort(int listeningPort)
{
  listeningPort_ = listeningPort;
}
int terrama2::core::ServiceManager::listeningPort() const
{
  return listeningPort_;
}

const std::string& terrama2::core::ServiceManager::terrama2Version() const
{
  return terrama2Version_;
}
const std::shared_ptr< te::dt::TimeInstantTZ >& terrama2::core::ServiceManager::startTime() const
{
  return startTime_;
}
const QJsonObject& terrama2::core::ServiceManager::status() const
{
  QJsonObject obj;
  obj.insert("instance_id", instanceId());
  obj.insert("instance_name", QString::fromStdString(instanceName()));
  obj.insert("start_time", QString::fromStdString(startTime_->toString()));
  obj.insert("terrama2_version",  QString::fromStdString(terrama2Version()));
  //TODO: Define status message
  return obj;
}
