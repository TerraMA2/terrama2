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
#include <memory>
#include <utility>

#include "../../Exception.hpp"
#include "../Exception.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../utility/Logger.hpp"
#include "DataStoragerFactory.hpp"

void terrama2::core::DataStoragerFactory::add(const std::string& code, FactoryFnctType f)
{
  auto it = factoriesMap_.find(code);

  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A data storager factory for this format already exists!");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataStoragerException() << ErrorDescription(errMsg);
  }

  factoriesMap_.emplace(code, f);
}

void terrama2::core::DataStoragerFactory::remove(const std::string& code)
{
  auto it = factoriesMap_.find(code);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered data storager factory for this format.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataStoragerException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::core::DataStoragerFactory::find(const std::string& code)
{
  auto it = factoriesMap_.find(code);

  return (it != factoriesMap_.end());
}

terrama2::core::DataStoragerPtr terrama2::core::DataStoragerFactory::make(terrama2::core::DataSeriesPtr outputDataSeries, terrama2::core::DataProviderPtr dataProvider) const
{
  auto it = factoriesMap_.find(outputDataSeries->semantics.code);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("Could not find a data storager factory for this format.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataStoragerException() << ErrorDescription(errMsg);
  }

  return it->second(outputDataSeries, dataProvider);
}
