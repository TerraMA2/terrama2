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
  \file terrama2/services/analysis/core/utility/Verify.cpp

  \brief Utility functions for easy consistency check

  \author Jano Simas
*/

#include "Verify.hpp"
#include "../../../../core/utility/Logger.hpp"

#include "../Exception.hpp"
#include "../Analysis.hpp"

#include <QObject>
#include <QString>

void terrama2::core::verify::analysisType(terrama2::services::analysis::core::AnalysisPtr analysis, terrama2::services::analysis::core::AnalysisType analysisType)
{
  if(analysis->type != analysisType)
  {
    QString errMsg = QObject::tr("Wrong analysis type.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw VerifyException() << terrama2::ErrorDescription(errMsg);
  }
}

void terrama2::core::verify::analysisGrid(const terrama2::services::analysis::core::AnalysisPtr analysis)
{
  analysisType(analysis, terrama2::services::analysis::core::AnalysisType::GRID_TYPE);
}

void terrama2::core::verify::analysisMonitoredObject(const terrama2::services::analysis::core::AnalysisPtr analysis)
{
  analysisType(analysis, terrama2::services::analysis::core::AnalysisType::MONITORED_OBJECT_TYPE);
}

void terrama2::core::verify::analysisDCP(const terrama2::services::analysis::core::AnalysisPtr analysis)
{
  analysisType(analysis, terrama2::services::analysis::core::AnalysisType::DCP_TYPE);
}
