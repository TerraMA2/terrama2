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
  \file terrama2/services/analysis/core/BaseContext.cpp

  \brief Base class for analysis context

  \author Jano Simas
*/

#include "BaseContext.hpp"
#include "PythonInterpreter.hpp"
#include "../../../core/data-model/DataSetGrid.hpp"
#include "../../../core/data-access/GridSeries.hpp"
#include "../../../core/data-access/DataAccessorGrid.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/TimeUtils.hpp"

terrama2::services::analysis::core::BaseContext::BaseContext(terrama2::services::analysis::core::DataManagerPtr dataManager, terrama2::services::analysis::core::AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
  : dataManager_(dataManager),
    analysis_(analysis),
    startTime_(startTime)
{
}

terrama2::services::analysis::core::BaseContext::~BaseContext()
{
}

void terrama2::services::analysis::core::BaseContext::addError(const std::string& errorMessage)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  errorsSet_.insert(errorMessage);
}

terrama2::core::DataSeriesPtr terrama2::services::analysis::core::BaseContext::findDataSeries(const std::string& dataSeriesName)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto it = dataSeriesMap_.find(dataSeriesName);
  if(it == dataSeriesMap_.end())
  {
    auto dataManagerPtr = getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    auto dataSeries = dataManagerPtr->findDataSeries(analysis_->id, dataSeriesName);

    dataSeriesMap_.emplace(dataSeriesName, dataSeries);
    return dataSeries;
  }

  return it->second;
}

std::shared_ptr<te::rst::Interpolator> terrama2::services::analysis::core::BaseContext::getInterpolator(std::shared_ptr<te::rst::Raster> raster)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  return interpolatorMap_[raster];
}

std::vector< std::shared_ptr<te::rst::Raster> >
terrama2::services::analysis::core::BaseContext::getRasterList(const terrama2::core::DataSeriesPtr& dataSeries,
    const DataSetId datasetId, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ObjectKey key;
  key.objectId_ = datasetId;
  key.dateFilterBegin_ = dateDiscardBefore;
  key.dateFilterEnd_ = dateDiscardAfter;

  auto it = rasterMap_.find(key);
  if(it != rasterMap_.end())
    return it->second;
  else
  {
    auto dataManager = dataManager_.lock();
    if(!dataManager)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    // First call, need to call sample for each dataset raster and store the result in the context.
    auto gridMap = getGridMap(dataManager, dataSeries->id, dateDiscardBefore, dateDiscardAfter);

    std::for_each(gridMap.begin(), gridMap.end(), [this, datasetId, key](decltype(*gridMap.begin()) it)
    {
      if(it.first->id == datasetId)
      {
        auto datasetGrid = it.first;
        auto dsRaster = it.second;

        if(!dsRaster)
        {
          QString errMsg(QObject::tr("Invalid raster for dataset: %1").arg(datasetGrid->id));
          throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
        }

        addRaster(key, dsRaster);
        //FIXME: configure interpolation method in monitored object analysis grid dataseries
        auto interpolationMethod = static_cast<int>(analysis_->outputGridPtr->interpolationMethod);
        if(interpolationMethod == 0)
          interpolationMethod = 1;
        std::shared_ptr<te::rst::Interpolator> interpolator(new te::rst::Interpolator(dsRaster.get(), interpolationMethod));
        interpolatorMap_.emplace(dsRaster, interpolator);
      }
    });

    return rasterMap_[key];
  }
}

std::unordered_multimap<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> >
terrama2::services::analysis::core::BaseContext::getGridMap(terrama2::services::analysis::core::DataManagerPtr dataManager,
    DataSeriesId dataSeriesId,
    const std::string& dateDiscardBefore,
    const std::string& dateDiscardAfter)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  ObjectKey key;
  key.objectId_ = dataSeriesId;
  key.dateFilterBegin_ = dateDiscardBefore;
  key.dateFilterEnd_ = dateDiscardAfter;

  auto it = analysisGridMap_.find(key);
  if(it == analysisGridMap_.end())
  {
    auto dataSeriesPtr = dataManager->findDataSeries(dataSeriesId);
    if(!dataSeriesPtr)
    {
      QString errMsg = QObject::tr("Could not recover data series: %1.").arg(dataSeriesId);
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    auto dataProviderPtr = dataManager->findDataProvider(dataSeriesPtr->dataProviderId);

    terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProviderPtr, dataSeriesPtr);
    std::shared_ptr<terrama2::core::DataAccessorGrid> accessorGrid = std::dynamic_pointer_cast<terrama2::core::DataAccessorGrid>(accessor);
    if(!accessorGrid)
    {
      QString errMsg = QObject::tr("Could not create a DataAccessor to the data series: %1.").arg(dataSeriesId);
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    terrama2::core::Filter filter = createFilter(dateDiscardBefore, dateDiscardAfter);
    auto gridSeries = accessorGrid->getGridSeries(filter);

    if(!gridSeries)
    {
      QString errMsg = QObject::tr("Invalid grid series for data series: %1.").arg(dataSeriesId);
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    auto gridMap =  gridSeries->gridMap();
    analysisGridMap_.emplace(key, gridMap);
    return gridMap;
  }

  return it->second;
}

terrama2::core::Filter terrama2::services::analysis::core::BaseContext::createFilter(const std::string& dateDiscardBefore, const std::string& dateDiscardAfter)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  terrama2::core::Filter filter;

  filter.discardAfter = startTime_;
  if(!dateDiscardBefore.empty() || !dateDiscardAfter.empty())
  {
    if(!dateDiscardBefore.empty())
    {
      boost::local_time::local_date_time ldt = startTime_->getTimeInstantTZ();
      double seconds = terrama2::core::TimeUtils::convertTimeString(dateDiscardBefore, "SECOND", "h");
      //TODO: PAULO: review losing precision
      ldt -= boost::posix_time::seconds(seconds);

      std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
      filter.discardBefore = std::move(titz);

      filter.lastValue = false;
    }

    if(!dateDiscardAfter.empty())
    {
      boost::local_time::local_date_time ldt = startTime_->getTimeInstantTZ();
      double seconds = terrama2::core::TimeUtils::convertTimeString(dateDiscardAfter, "SECOND", "h");
      ldt -= boost::posix_time::seconds(seconds);

      std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
      filter.discardAfter = std::move(titz);

      filter.lastValue = false;
    }
  }
  else
  {
    // no filter set
    // use start date as last value
    filter.discardAfter = std::unique_ptr<te::dt::TimeInstantTZ>(static_cast<te::dt::TimeInstantTZ*>(startTime_->clone()));
    filter.lastValue = true;
  }

  return filter;
}

std::unordered_map<terrama2::core::DataSetPtr,terrama2::core::DataSetSeries >
terrama2::services::analysis::core::BaseContext::getSeriesMap(DataSeriesId dataSeriesId,
    const std::string& dateDiscardBefore,
    const std::string& dateDiscardAfter)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  ObjectKey key;
  key.objectId_ = dataSeriesId;
  key.dateFilterBegin_ = dateDiscardBefore;
  key.dateFilterEnd_ = dateDiscardAfter;

  auto dataManager = getDataManager().lock();

  auto it = analysisSeriesMap_.find(key);
  if(it == analysisSeriesMap_.end())
  {
    auto dataSeriesPtr = dataManager->findDataSeries(dataSeriesId);
    if(!dataSeriesPtr)
    {
      QString errMsg = QObject::tr("Could not recover data series: %1.").arg(dataSeriesId);
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    auto dataProviderPtr = dataManager->findDataProvider(dataSeriesPtr->dataProviderId);

    terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProviderPtr, dataSeriesPtr);
    std::shared_ptr<terrama2::core::DataAccessorGrid> accessorGrid = std::dynamic_pointer_cast<terrama2::core::DataAccessorGrid>(accessor);

    terrama2::core::Filter filter = createFilter(dateDiscardBefore, dateDiscardAfter);
    auto gridSeries = accessorGrid->getGridSeries(filter);

    if(!gridSeries)
    {
      QString errMsg = QObject::tr("Invalid grid series for data series: %1.").arg(dataSeriesId);
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    auto series =  gridSeries->getSeries();
    analysisSeriesMap_.emplace(key, series);
    return series;
  }

  return it->second;
}
