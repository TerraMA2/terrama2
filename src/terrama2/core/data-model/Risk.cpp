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
  for (size_t i = 0; i < riskLevels.size(); ++i)
  {
    auto riskLevel = riskLevels[i];
    if(value >= riskLevels[i].value)
    {
      bool hasNext = i + 1 < riskLevels.size();
      if(hasNext && value < riskLevels[i + 1].value)
      {
        return std::make_tuple(riskLevel.level, riskLevel.name);
      }

      if(!hasNext)
      {
        return std::make_tuple(riskLevel.level, riskLevel.name);
      }
    }
  }

  QString errMsg = QObject::tr("Risk level not defined for value: %1").arg(value);
  TERRAMA2_LOG_ERROR() << errMsg;
  throw DataSeriesRiskException() << ErrorDescription(errMsg);
}

std::string terrama2::core::Risk::riskName(const int level) const
{
  for (unsigned int i = 0; i < riskLevels.size(); ++i)
  {
    if(level == riskLevels[i].level)
    {
      return riskLevels[i].name;
    }
  }

  QString errMsg = QObject::tr("Risk not defined for level: %1").arg(level);
  TERRAMA2_LOG_ERROR() << errMsg;
  throw DataSeriesRiskException() << ErrorDescription(errMsg);
}
