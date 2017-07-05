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
  \file terrama2/core/data-model/Risk.hpp

  \brief Models the information of a DataSeries.

  \author Jano Simas
*/

#include "Risk.hpp"
#include "../utility/Logger.hpp"
#include "../Exception.hpp"

#include <QString>
#include <QObject>

std::tuple<int, std::string> terrama2::core::Risk::riskLevel(double value) const
{
  struct lessThen
  {
    bool operator()(const double &a, const terrama2::core::RiskLevel &b){ return a < b.value; }
    bool operator()(const terrama2::core::RiskLevel &a, const double &b){ return a.value < b; }
  };

  // find the first level lower than value
  auto lowerBound = std::lower_bound(riskLevels.begin(), riskLevels.end(), value, lessThen());
  // if level not found, return defaultLevel
  if(lowerBound == riskLevels.end())
    return std::make_tuple(defaultRisk.level, defaultRisk.name);

  //get next level, same as value or greater
  ++lowerBound;

  // if level not found, return defaultLevel
  if(lowerBound == riskLevels.end())
    return std::make_tuple(defaultRisk.level, defaultRisk.name);

  return std::make_tuple(lowerBound->level, lowerBound->name);
}

std::string terrama2::core::Risk::riskName(const int level) const
{
  //default risk
  if(level == defaultRisk.level)
    return defaultRisk.name;

  //find risk level
  auto it = std::find_if(riskLevels.begin(), riskLevels.end(), [level](const RiskLevel &b){ return level == b.level;});
  if(it == riskLevels.end())
  {
    //the level is not a valid
    QString errMsg = QObject::tr("Risk not defined for level: %1").arg(level);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataSeriesRiskException() << ErrorDescription(errMsg);
  }

  return it->name;
}
