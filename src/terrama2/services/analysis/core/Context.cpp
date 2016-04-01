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
  \file terrama2/services/analysis/core/Context.cpp

  \brief Class to store the context of execution of an python script.

  \author Paulo R. M. Oliveira
*/

#include "Context.hpp"
#include "Analysis.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/Exception.hpp"

// QT
#include <QString>
#include <QObject>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/sam/kdtree.h>


std::map<std::string, double> terrama2::services::analysis::core::Context::analysisResult(uint64_t analysisId)
{
  std::unique_lock<std::mutex> lock(mutex_);
  return analysisResult_[analysisId];
}

void terrama2::services::analysis::core::Context::setAnalysisResult(uint64_t analysisId, std::string geomId, double result)
{
  std::unique_lock<std::mutex> lock(mutex_);
  auto& valueMap = analysisResult_[analysisId];
  valueMap[geomId] = result;
}


std::shared_ptr<terrama2::services::analysis::core::ContextDataset> terrama2::services::analysis::core::Context::getContextDataset(const uint64_t analysisId, const uint64_t datasetId, const std::string& dateFilter) const
{
  std::unique_lock<std::mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  if(it != datasetMap_.end())
  {
    return it->second;
  }

  std::shared_ptr<ContextDataset> empty;
  return empty;
}

std::shared_ptr<terrama2::services::analysis::core::ContextDataset> terrama2::services::analysis::core::Context::addDataset(const uint64_t analysisId, const DataSetId datasetId, const std::string& dateFilter, std::shared_ptr<te::mem::DataSet>& dataset, std::string identifier, bool createSpatialIndex)
{
 std::unique_lock<std::mutex> lock(mutex_);

  std::shared_ptr<ContextDataset> datasetContext(new ContextDataset);

  std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(dataset.get(), te::dt::GEOMETRY_TYPE);

  std::shared_ptr<SyncronizedDataSet> syncDataset(new SyncronizedDataSet(dataset));

  datasetContext->identifier = identifier;
  datasetContext->geometryPos = geomPropertyPosition;
  datasetContext->dataset = syncDataset;

  if(createSpatialIndex)
  {
    for(std::size_t i = 0; i < syncDataset->size(); ++i)
    {
      auto geom = syncDataset->getGeometry(i, geomPropertyPosition);
      datasetContext->rtree.insert(*geom->getMBR(), i);
    }
  }

  ContextKey key;
  key.datasetId_ = datasetId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;
  datasetMap_[key] = datasetContext;

  return datasetContext;
}

std::shared_ptr<terrama2::services::analysis::core::ContextDataset> terrama2::services::analysis::core::Context::addDCP(const uint64_t analysisId, terrama2::core::DataSetDcpPtr dcp, const std::string& dateFilter, std::shared_ptr<te::mem::DataSet>& dataset)
{
  std::unique_lock<std::mutex> lock(mutex_);

  std::shared_ptr<ContextDataset> datasetContext(new ContextDataset);

  std::shared_ptr<SyncronizedDataSet> syncDataset(new SyncronizedDataSet(dataset));

  auto analysis = getAnalysis(analysisId);
  if(analysis.id != analysisId)
  {
    QString msg(QObject::tr("Could not find analysis %1 in context.").arg(analysisId));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);
  }

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    for(auto dataset : analysisDataSeries.dataSeries->datasetList)
    {
      if(dataset->id == dcp->id)
      {
        auto metadata = analysisDataSeries.metadata;

        if(metadata["INFLUENCE_TYPE"] != "REGION")
        {
          auto buffer = dcp->position->buffer(atof(metadata["RADIUS"].c_str()));
          datasetContext->rtree.insert(*buffer->getMBR(), dcp->id);
        }
        else
        {
          assert(false);
          // TODO: Implement influence by region
        }

      }
    }
  }

  datasetContext->dataset = syncDataset;

  ContextKey key;
  key.datasetId_ = dcp->id;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;
  datasetMap_[key] = datasetContext;

  return datasetContext;
}

bool terrama2::services::analysis::core::Context::exists(const uint64_t analysisId, const DataSetId datasetId, const std::string& dateFilter) const
{
  std::unique_lock<std::mutex> lock(mutex_);

  ContextKey key;
  key.datasetId_ = datasetId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  return it != datasetMap_.end();
}

terrama2::services::analysis::core::Analysis terrama2::services::analysis::core::Context::getAnalysis(AnalysisId analysisId) const
{
  return dataManager_.lock()->findAnalysis(analysisId);
}
