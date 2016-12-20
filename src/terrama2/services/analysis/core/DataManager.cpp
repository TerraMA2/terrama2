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
#include "utility/JSonUtils.hpp"
#include "Analysis.hpp"
#include "../../../Exception.hpp"
#include "../../../core/utility/Logger.hpp"


// Qt
#include <QJsonValue>
#include <QJsonArray>


terrama2::services::analysis::core::DataManager::DataManager()
{
}

terrama2::services::analysis::core::DataManager::~DataManager()
{
}

void terrama2::services::analysis::core::DataManager::addJSon(const QJsonObject& obj)
{
  try
  {
    terrama2::core::DataManager::addJSon(obj);

    auto analysisArray = obj["Analysis"].toArray();
    for(auto json : analysisArray)
    {
      auto dataPtr = terrama2::services::analysis::core::fromAnalysisJson(json.toObject());
      if(hasAnalysis(dataPtr->id))
        update(dataPtr);
      else
        add(dataPtr);
    }
  }
  catch(const terrama2::Exception& /*e*/)
  {
    // logged on throw...
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown error...");
  }
}

void terrama2::services::analysis::core::DataManager::add(AnalysisPtr analysis)
{
  // Inside a block so the lock is released before emitting the signal
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if(analysis->name.empty())
    {
      QString errMsg = QObject::tr("Can not add an analysis with empty name.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    if(analysis->id == terrama2::core::InvalidId())
    {
      QString errMsg = QObject::tr("Can not add an analysis with an invalid id.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    analysis_[analysis->id] = analysis;
  }

  emit analysisAdded(analysis);
}

void terrama2::services::analysis::core::DataManager::update(AnalysisPtr analysis)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    blockSignals(true);
    removeAnalysis(analysis->id);
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
      QString errMsg = QObject::tr("Analysis not registered: %1.").arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    analysis_.erase(itPr);

  }

  emit analysisRemoved(analysisId);
}

terrama2::services::analysis::core::AnalysisPtr terrama2::services::analysis::core::DataManager::findAnalysis(const AnalysisId analysisId) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = std::find_if(analysis_.cbegin(), analysis_.cend(),
                                [analysisId](std::pair<AnalysisId, AnalysisPtr> analysisPair)
                                { return analysisPair.second->id == analysisId; });
  if(it == analysis_.cend())
  {
    QString errMsg = QObject::tr("Analysis not registered: %1.").arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return it->second;
}

terrama2::core::DataSeriesPtr terrama2::services::analysis::core::DataManager::findDataSeries(const AnalysisId analysisId, const std::string& name) const
{
  terrama2::core::DataSeriesPtr dataSeries;
  auto analysis = findAnalysis(analysisId);
  for(auto analysisDataSeries : analysis->analysisDataSeriesList)
  {
    if(analysisDataSeries.alias == name)
    {
      dataSeries = terrama2::core::DataManager::findDataSeries(analysisDataSeries.dataSeriesId);
      break;
    }
  }


  if(!dataSeries)
  {

    dataSeries = terrama2::core::DataManager::findDataSeries(name);
  }


  bool inAdditionalDataSeriesList = false;
  for(auto analysisDataSeries : analysis->analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeriesId == dataSeries->id)
    {
      inAdditionalDataSeriesList = true;
      break;
    }
  }

  if(!inAdditionalDataSeriesList)
  {
    QString errMsg = QObject::tr("The selected data series is not present in the additional data list: %1.").arg(QString::fromStdString(name));
    throw InvalidParameterException() << ErrorDescription(errMsg);
  }

  return dataSeries;
}

bool terrama2::services::analysis::core::DataManager::hasAnalysis(const AnalysisId analysisId) const
{
  std::lock_guard<std::recursive_mutex> lock(mtx_);

  const auto& it = analysis_.find(analysisId);
  return it != analysis_.cend();
}

void terrama2::services::analysis::core::DataManager::validateJSon(const QJsonObject& obj)
{
  AnalysisPtr analysis = fromAnalysisJson(obj);
  emit validateAnalysis(analysis);
}