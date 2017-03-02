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
  \file terrama2/core/utility/DataRetrieverFactory.hpp

  \brief

  \author Jano Simas
 */

#include "DataRetrieverFactory.hpp"
#include "../data-access/DataRetriever.hpp"
#include "../data-model/DataProvider.hpp"
#include "../utility/Logger.hpp"

#include <QObject>
#include <QString>

void terrama2::core::DataRetrieverFactory::add(const DataProviderType& dataProviderType, FactoryFnctType f)
{
  auto it = factoriesMap_.find(dataProviderType);

  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A data retriever factory for this type already exists!");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataRetrieverException() << ErrorDescription(errMsg);
  }

  factoriesMap_.emplace(dataProviderType, f);
}

void terrama2::core::DataRetrieverFactory::remove(const DataProviderType& dataProviderType)
{
  auto it = factoriesMap_.find(dataProviderType);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered data retriever factory for this type.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::DataRetrieverException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::core::DataRetrieverFactory::find(const DataProviderType& dataProviderType)
{
  auto it = factoriesMap_.find(dataProviderType);

  return (it != factoriesMap_.end());
}

terrama2::core::DataRetrieverPtr terrama2::core::DataRetrieverFactory::make(terrama2::core::DataProviderPtr dataProvider) const
{
  auto it = factoriesMap_.find(dataProvider->dataProviderType);
  if(it == factoriesMap_.end())
  {
    // if the data retriever for this type is not registered,
    // create a base data retriever (non-retrievable)
    return DataRetrieverPtr(terrama2::core::DataRetriever::make(dataProvider));
  }

  return it->second(dataProvider);
}
