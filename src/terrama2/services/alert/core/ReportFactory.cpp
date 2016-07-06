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
  \file terrama2/services/alert/core/ReportFactory.cpp

  \brief

  \author Jano Simas
 */

#include "ReportFactory.hpp"
#include "Exception.hpp"
#include "../../../core/utility/Logger.hpp"

#include <QObject>

bool terrama2::services::alert::core::ReportFactory::add(terrama2::services::alert::core::ReportType reportType, FactoryFnctType f)
{
  auto it = factoriesMap_.find(reportType);
  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A report factory with this type already exists: %1!").arg(QString::fromStdString(reportType));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();

    return false;
  }
  else
  {
    factoriesMap_.emplace(reportType, f);
    return true;
  }
}

bool terrama2::services::alert::core::ReportFactory::find(terrama2::services::alert::core::ReportType reportType)
{
  auto it = factoriesMap_.find(reportType);
  return it != factoriesMap_.end();
}

terrama2::services::alert::core::ReportPtr terrama2::services::alert::core::ReportFactory::make(terrama2::services::alert::core::ReportType reportType, std::unordered_map<std::string, std::string> reportMetadata)
{
  auto it = factoriesMap_.find(reportType);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A report factory with this type doesn't exists: %1!").arg(QString::fromStdString(reportType));
    TERRAMA2_LOG_ERROR() << errMsg;

    throw ReportException() << ErrorDescription(errMsg);
  }
  else
  {
    return it->second(reportMetadata);
  }
}

void terrama2::services::alert::core::ReportFactory::remove(terrama2::services::alert::core::ReportType reportType)
{
  auto it = factoriesMap_.find(reportType);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered report factory with type : %1!").arg(QString::fromStdString(reportType));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    
    throw ReportException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}
