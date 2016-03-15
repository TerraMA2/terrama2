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
  \file terrama2/analysis/core/Context.cpp

  \brief Class to store the context of execution of an python script.

  \author Paulo R. M. Oliveira
*/

#include "Context.hpp"
#include "../../core/Logger.hpp"
#include "../../Exception.hpp"

// QT
#include <QString>
#include <QObject>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/sam/kdtree.h>


std::map<std::string, double> terrama2::analysis::core::Context::analysisResult(uint64_t analysisId)
{
  std::unique_lock<std::mutex> lock(mutex_);
  return analysisResult_[analysisId];
}

void terrama2::analysis::core::Context::setAnalysisResult(uint64_t analysisId, std::string geomId, double result)
{
  std::unique_lock<std::mutex> lock(mutex_);
  auto& valueMap = analysisResult_[analysisId];
  valueMap[geomId] = result;
}

terrama2::analysis::core::Analysis terrama2::analysis::core::Context::getAnalysis(const uint64_t id)
{
  std::unique_lock<std::mutex> lock(mutex_);
  auto it = analysis_.find(id);
  if(it != analysis_.end())
    return it->second;
  else
  {
    QString msg(QObject::tr("Could not add operation context"));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);
  }
}

void terrama2::analysis::core::Context::addAnalysis(const Analysis& analysis)
{
  std::unique_lock<std::mutex> lock(mutex_);
  analysis_[analysis.id()] = analysis;
}

std::shared_ptr<terrama2::analysis::core::ContextDataset> terrama2::analysis::core::Context::getContextDataset(const uint64_t analysisId, const uint64_t datasetItemId, const std::string& dateFilter) const
{
  std::unique_lock<std::mutex> lock(mutex_);

  ContextKey key;
  key.datasetItemId_ = datasetItemId;
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

std::shared_ptr<terrama2::analysis::core::ContextDataset> terrama2::analysis::core::Context::addDataset(const uint64_t analysisId, const uint64_t datasetItemId, const std::string& dateFilter, std::shared_ptr<te::da::DataSet>& dataset, std::string identifier, bool createSpatialIndex)
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
  key.datasetItemId_ = datasetItemId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;
  datasetMap_[key] = datasetContext;

  return datasetContext;
}

std::shared_ptr<terrama2::analysis::core::ContextDataset> terrama2::analysis::core::Context::addDCP(const uint64_t analysisId, const uint64_t datasetItemId, const terrama2::core::PCD& dcp, const std::string& dateFilter, std::shared_ptr<te::da::DataSet>& dataset, std::shared_ptr<te::da::DataSetType>& datasetType)
{
  std::unique_lock<std::mutex> lock(mutex_);

  std::shared_ptr<ContextDataset> datasetContext(new ContextDataset);


  datasetContext->datasetType = datasetType;

  std::shared_ptr<SyncronizedDataSet> syncDataset(new SyncronizedDataSet(dataset));

  auto it = analysis_.find(analysisId);
  if(it == analysis_.end())
  {
    QString msg(QObject::tr("Could not find analysis %1 in context.").arg(analysisId));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(msg);
  }
  auto analysis = it->second;

  uint64_t datasetId = dcp.dataSetItem().dataset();
  auto influence = analysis.influence(datasetId);

  if(influence.type != Analysis::REGION)
  {
    auto buffer = dcp.location()->buffer(influence.radius);
    datasetContext->rtree.insert(*buffer->getMBR(), dcp.dataSetItem().id());
  }
  else
  {
    assert(false);
    // TODO: Implement influence by region
  }

  datasetContext->dataset = syncDataset;

  ContextKey key;
  key.datasetItemId_ = datasetItemId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;
  datasetMap_[key] = datasetContext;

  return datasetContext;
}

bool terrama2::analysis::core::Context::exists(const uint64_t analysisId, const uint64_t datasetItemId, const std::string& dateFilter) const
{
  std::unique_lock<std::mutex> lock(mutex_);

  ContextKey key;
  key.datasetItemId_ = datasetItemId;
  key.analysisId_ = analysisId;
  key.dateFilter_ = dateFilter;

  auto it = datasetMap_.find(key);
  return it != datasetMap_.end();
}
