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
  \file terrama2/services/analysis/core/DataManager.hpp

  \brief Manages metadata about analysis, data providers and its dataseries.

  \author Paulo R. M. Oliveira
*/

#include "DataManager.hpp"
#include "../../../Exception.hpp"


terrama2::services::analysis::core::DataManager::DataManager()
{

}

terrama2::services::analysis::core::DataManager::~DataManager ()
{
}

void terrama2::services::analysis::core::DataManager::add(const Analysis& analysis)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(analysis.name.empty())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add an analysis with empty name."));

    if(analysis.id == terrama2::core::InvalidId())
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Can not add an analysis with an invalid id."));

    analysis_[analysis.id] = analysis;
  }

  emit analysisAdded(analysis);
}

void terrama2::services::analysis::core::DataManager::update(const Analysis& analysis)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    blockSignals(true);
    removeAnalysis(analysis.id);
    add(analysis);
    blockSignals(false);
  }

  emit analysisUpdated(analysis);
}

void terrama2::services::analysis::core::DataManager::removeAnalysis(const AnalysisId analysisId)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto itPr = analysis_.find(analysisId);
    if(itPr == analysis_.end())
    {
      throw terrama2::InvalidArgumentException() <<
            ErrorDescription(QObject::tr("Analysis not registered."));
    }

    analysis_.erase(itPr);

  }

  emit analysisRemoved(analysisId);
}

terrama2::services::analysis::core::Analysis terrama2::services::analysis::core::DataManager::findAnalysis(const AnalysisId analysisId)
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = std::find_if(analysis_.cbegin(), analysis_.cend(), [analysisId](std::pair<AnalysisId, Analysis> analysis){ return analysis.second.id == analysisId;});
  if(it == analysis_.cend())
  {
    throw terrama2::InvalidArgumentException() <<
          ErrorDescription(QObject::tr("Analysis not registered."));
  }

  return it->second;
}
