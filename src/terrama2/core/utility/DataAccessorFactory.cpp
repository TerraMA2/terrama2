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

#include "DataAccessorFactory.hpp"
#include "../data-access/DataAccessor.hpp"
#include "../Exception.hpp"
#include "../data-model/DataSet.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-model/DataProvider.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../utility/Logger.hpp"

#include <QObject>
#include <QString>

#include <memory>

void terrama2::core::DataAccessorFactory::add(const std::string& semanticName, FactoryFnctType f)
{
  std::map<std::string, FactoryFnctType>::const_iterator it = factoriesMap_.find(semanticName);

  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A data accessor factory with this name already exists: %1!").arg(semanticName.c_str());
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  factoriesMap_[semanticName] = f;
}

void terrama2::core::DataAccessorFactory::remove(const std::string& semanticName)
{
  std::map<std::string, FactoryFnctType>::iterator it = factoriesMap_.find(semanticName);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered data accessor factory named : %1!").arg(semanticName.c_str());
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::core::DataAccessorFactory::find(const std::string& semanticName)
{
  std::map<std::string, FactoryFnctType>::const_iterator it = factoriesMap_.find(semanticName);

  return (it != factoriesMap_.end());
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorFactory::make(terrama2::core::DataProviderPtr dataProvider, terrama2::core::DataSeriesPtr dataSeries, terrama2::core::Filter filter)
{

  std::map<std::string, FactoryFnctType>::const_iterator it = factoriesMap_.find(dataSeries->semantics.name);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("Could not find a data accessor factory for this semantic: %1!").arg(dataSeries->semantics.name.c_str());
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  std::shared_ptr<DataAccessor> dataAccessor(factoriesMap_[dataSeries->semantics.name.c_str()](dataProvider, dataSeries, filter));

  return dataAccessor;
}
