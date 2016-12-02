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
  \file terrama2/services/analysis/core/AnalysisLogger.cpp

  \brief

  \author Vinicius Campanha
*/

// TerraMA2
#include "AnalysisLogger.hpp"
#include "../../../core/utility/ServiceManager.hpp"

terrama2::services::analysis::core::AnalysisLogger::AnalysisLogger()
 : ProcessLogger()
{
}

void terrama2::services::analysis::core::AnalysisLogger::setConnectionInfo(const te::core::URI& uri)
{
  terrama2::core::ProcessLogger::setConnectionInfo(uri);

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
  std::string tableName = "analysis_"+std::to_string(serviceManager.instanceId());
  setTableName(tableName);
}
