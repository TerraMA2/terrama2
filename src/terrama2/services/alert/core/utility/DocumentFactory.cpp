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
  \file terrama2/services/alert/core/utility/DocumentFactory.cpp

  \brief

  \author Vinicius Campanha
*/

#include "DocumentFactory.hpp"
#include "../Exception.hpp"
#include "../../../../core/utility/Logger.hpp"

#include <QString>
#include <QObject>

void terrama2::services::alert::core::DocumentFactory::add(const std::string& documentCode, FactoryFnctType f)
{
  auto it = factoriesMap_.find(documentCode);
  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A document factory with this name already exists: %1.").arg(QString::fromStdString(documentCode));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::services::alert::NotifierException() << ErrorDescription(errMsg);
  }

  factoriesMap_[documentCode] = f;
}

void terrama2::services::alert::core::DocumentFactory::remove(const std::string& documentCode)
{
  std::map<std::string, FactoryFnctType>::const_iterator it = factoriesMap_.find(documentCode);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered document factory named : %1.").arg(QString::fromStdString(documentCode));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::services::alert::NotifierException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::services::alert::core::DocumentFactory::find(const std::string& documentCode)
{
  std::map<std::string, FactoryFnctType>::const_iterator it = factoriesMap_.find(documentCode);
  return (it != factoriesMap_.end());
}

std::string terrama2::services::alert::core::DocumentFactory::makeDocument(const std::string& documentCode,
                                                                           ReportPtr report)
{
  std::map<std::string, FactoryFnctType>::const_iterator it = factoriesMap_.find(documentCode);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("Could not find a document factory for this code: %1.").arg(QString::fromStdString(documentCode));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::services::alert::NotifierException() << ErrorDescription(errMsg);
  }

  return factoriesMap_[documentCode](report);
}
