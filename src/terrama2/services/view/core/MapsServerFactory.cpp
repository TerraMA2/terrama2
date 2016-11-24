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
  \file terrama2/services/view/core/MapsServerFactory.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "MapsServerFactory.hpp"
#include "MapsServer.hpp"
#include "Exception.hpp"
#include "../../../core/utility/Logger.hpp"

// Qt
#include <QObject>
#include <QString>

void terrama2::services::view::core::MapsServerFactory::add(const MapsServerType& mapsServertype, FactoryFnctType f)
{
  auto it = factoriesMap_.find(mapsServertype);

  if(it != factoriesMap_.end())
  {
    QString errMsg = QObject::tr("A maps server factory for this type already exists!");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw MapsServerFactoryException() << ErrorDescription(errMsg);
  }

  factoriesMap_.emplace(mapsServertype, f);
}

void terrama2::services::view::core::MapsServerFactory::remove(const MapsServerType& mapsServertype)
{
  auto it = factoriesMap_.find(mapsServertype);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered maps server factory for this type.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw MapsServerFactoryException() << ErrorDescription(errMsg);
  }

  factoriesMap_.erase(it);
}

bool terrama2::services::view::core::MapsServerFactory::find(const MapsServerType& mapsServertype)
{
  auto it = factoriesMap_.find(mapsServertype);

  return (it != factoriesMap_.end());
}

terrama2::services::view::core::MapsServerPtr terrama2::services::view::core::MapsServerFactory::make(const te::core::URI uri,
                                                                                         const MapsServerType& mapsServertype) const
{
  auto it = factoriesMap_.find(mapsServertype);

  if(it == factoriesMap_.end())
  {
    QString errMsg = QObject::tr("There is no registered maps server factory for this type.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw MapsServerFactoryException() << ErrorDescription(errMsg);
  }

  return it->second(uri);
}
