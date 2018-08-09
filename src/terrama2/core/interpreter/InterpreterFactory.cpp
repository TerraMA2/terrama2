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
  \file terrama2/core/utility/DataAccessorFactory.hpp

  \brief

  \author Jano Simas
 */

#include <QObject>
#include <QString>
#include <string>
#include <utility>

#include "../../Exception.hpp"
#include "../Exception.hpp"
#include "../utility/Logger.hpp"
#include "InterpreterFactory.hpp"

void terrama2::core::InterpreterFactory::add(const terrama2::core::InterpreterType& interpreterType, FactoryFnctType f, RaiiInterpreterFnctType raiiFunction)
{
  auto it = factoriesMap_.find(interpreterType);
  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("An interpreter factory for %1 already exists!").arg(QString::fromStdString(interpreterType));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataStoragerException() << ErrorDescription(errMsg);
  }

  factoriesMap_.emplace(interpreterType, f);

  // if registered a raii interpreter function
  // add to the raii vector to be released at destruction
  if(raiiFunction)
  {
    raiiInterpreterVector_.emplace_back(raiiFunction());
  }
}

void terrama2::core::InterpreterFactory::remove(const terrama2::core::InterpreterType& interpreterType)
{
  auto it = factoriesMap_.find(interpreterType);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered interpreter factory for %1.").arg(QString::fromStdString(interpreterType));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataStoragerException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::core::InterpreterFactory::find(const terrama2::core::InterpreterType& interpreterType)
{
  auto it = factoriesMap_.find(interpreterType);
  return (it != factoriesMap_.end());
}

terrama2::core::InterpreterPtr terrama2::core::InterpreterFactory::make(const terrama2::core::InterpreterType& interpreterType) const
{
  auto it = factoriesMap_.find(interpreterType);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("Could not find an interpreter factory for %1.").arg(QString::fromStdString(interpreterType));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataStoragerException() << ErrorDescription(errMsg);
  }

  return it->second();
}
