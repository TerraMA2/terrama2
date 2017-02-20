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

#include "Logger.hpp"
#include "SemanticsManager.hpp"

#include <QObject>
#include <QString>

#include <memory>

void terrama2::core::DataAccessorFactory::add(const std::string& semanticsCode, FactoryFnctType f)
{
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  try
  {
    auto semantics = semanticsManager.getSemantics(semanticsCode);
    auto it = factoriesMap_.find(semantics);

    if(it != factoriesMap_.end())
    {
      QString errMsg = QObject::tr("A data accessor factory with this name already exists: %1.").arg(semantics.code.c_str());
      TERRAMA2_LOG_ERROR() << errMsg.toStdString();
      throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
    }

    factoriesMap_[semantics] = f;
  }
  catch(const terrama2::core::SemanticsException& e)
  {
    // Could not add the factory because the given semantic isn't registered.
    QString errMsg = QObject::tr("Could not register data accessor factory\n Sematics not registered!");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
  }
}

void terrama2::core::DataAccessorFactory::remove(const std::string& semanticsCode)
{
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  auto semantics = semanticsManager.getSemantics(semanticsCode);
  std::map<terrama2::core::DataSeriesSemantics, FactoryFnctType>::const_iterator it = factoriesMap_.find(semantics);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered data accessor factory named : %1.").arg(semantics.code.c_str());
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::core::DataAccessorFactory::find(const std::string& semanticsCode)
{
  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();
  auto semantics = semanticsManager.getSemantics(semanticsCode);
  std::map<terrama2::core::DataSeriesSemantics, FactoryFnctType>::const_iterator it = factoriesMap_.find(semantics);

  return (it != factoriesMap_.end());
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorFactory::make(terrama2::core::DataProviderPtr dataProvider, terrama2::core::DataSeriesPtr dataSeries)
{

  std::map<terrama2::core::DataSeriesSemantics, FactoryFnctType>::const_iterator it = factoriesMap_.find(dataSeries->semantics);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("Could not find a data accessor factory for this semantic: %1.").arg(QString::fromStdString(dataSeries->semantics.driver));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }

  return factoriesMap_[dataSeries->semantics](dataProvider, dataSeries);
}
