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
  \file terrama2/core/data-model/SemanticsManager.cpp

  \brief

  \author Jano Simas
*/

#include "SemanticsManager.hpp"
#include "../Exception.hpp"
#include "Logger.hpp"

// Qt
#include <QString>
#include <QObject>

terrama2::core::DataSeriesSemantics terrama2::core::SemanticsManager::addSemantics(const std::string& code,
                                                                                   const std::string& name,
                                                                                   const std::string& driver,
                                                                                   const DataSeriesType& dataSeriesType,
                                                                                   const DataSeriesTemporality& dataSeriesTemporality,
                                                                                   const DataFormat& format,
                                                                                   const std::vector<DataProviderType>& providersTypeList,
                                                                                   const std::unordered_map<std::string, std::string>& metadata)
{
  auto it = semanticsMap_.find(code);
  if(it != semanticsMap_.cend())
  {
    QString errMsg = QObject::tr("Semantics %1 already registered.").arg(QString::fromStdString(name));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::SemanticsException() << ErrorDescription(errMsg);
  }

  DataSeriesSemantics semantics;
  semantics.code = code;
  semantics.name = name;
  semantics.driver = driver;
  semantics.dataSeriesType = dataSeriesType;
  semantics.temporality = dataSeriesTemporality;
  semantics.dataFormat = format;
  semantics.providersTypeList = providersTypeList;
  semantics.metadata = metadata;

  semanticsMap_[code] = semantics;

  return semantics;
}

terrama2::core::DataSeriesSemantics terrama2::core::SemanticsManager::getSemantics(const std::string& semanticsCode)
{
  auto it = semanticsMap_.find(semanticsCode);
  if(it == semanticsMap_.cend())
  {
    QString errMsg = QObject::tr("Semantics %1 not registered.").arg(QString::fromStdString(semanticsCode));
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw terrama2::core::SemanticsException() << ErrorDescription(errMsg);
  }

  return it->second;
}
