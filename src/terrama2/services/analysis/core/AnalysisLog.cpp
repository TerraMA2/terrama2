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
  \file terrama2/services/analysis/core/AnalysisLog.cpp

  \brief

  \author Vinicius Campanha
*/

// TerraMA2
#include "AnalysisLog.hpp"

terrama2::services::analysis::core::AnalysisLog::AnalysisLog(uint64_t processID)
 : ProcessLog(processID)
{
// VINICIUS:
}

void terrama2::services::analysis::core::AnalysisLog::addValue(std::string tag, std::string value)
{
// VINICIUS:
}

void terrama2::services::analysis::core::AnalysisLog::update(terrama2::core::Status status, te::dt::TimeInstantTZ dataTimestamp)
{
// VINICIUS:
}

void terrama2::services::analysis::core::AnalysisLog::error(std::string description)
{
// VINICIUS:
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::services::analysis::core::AnalysisLog::getLastProcessDate()
{
  // VINICIUS:
  return terrama2::core::TimeUtils::now();
}
