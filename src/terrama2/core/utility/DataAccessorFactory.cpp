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
  \file terrama2/core/data-access/DataAccessorFactory.cpp

  \brief

  \author Paulo R. M. Oliveira
 */

#include <QObject>
#include <QString>
#include <memory>
#include <string>

#include "../../Exception.hpp"
#include "../Exception.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../data-model/DataSeries.hpp"
#include "DataAccessorFactory.hpp"
#include "Logger.hpp"

void terrama2::core::DataAccessorFactory::add(const SemanticsDriver& semanticsDriver, FactoryFnctType f)
{
  auto it = factoriesMap_.find(semanticsDriver);
  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A data accessor factory for the driver %1 already exists.").arg(QString::fromStdString(semanticsDriver));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  factoriesMap_[semanticsDriver] = f;
}

void terrama2::core::DataAccessorFactory::remove(const SemanticsDriver& semanticsDriver)
{
  auto it = factoriesMap_.find(semanticsDriver);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered data accessor factory for the driver : %1.").arg(QString::fromStdString(semanticsDriver));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::core::DataAccessorFactory::find(const SemanticsDriver& semanticsDriver)
{
  auto it = factoriesMap_.find(semanticsDriver);
  return (it != factoriesMap_.end());
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorFactory::make(terrama2::core::DataProviderPtr dataProvider, terrama2::core::DataSeriesPtr dataSeries)
{
  auto it = factoriesMap_.find(dataSeries->semantics.driver);
  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("Could not find a data accessor factory for the semantics driver: %1.").arg(QString::fromStdString(dataSeries->semantics.driver));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  return factoriesMap_[dataSeries->semantics.driver](dataProvider, dataSeries);
}
