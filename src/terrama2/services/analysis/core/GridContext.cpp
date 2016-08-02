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
  \file terrama2/services/analysis/core/GridContext.cpp

  \brief Contains grid analysis context.

  \author Jano Simas
*/

#include "GridContext.hpp"
#include "grid/Operator.hpp"
#include "DataManager.hpp"
#include "Utils.hpp"
#include "PythonInterpreter.hpp"
#include "../../../core/data-model/DataSetGrid.hpp"
#include "../../../core/data-access/GridSeries.hpp"
#include "../../../core/data-access/DataAccessorGrid.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/TimeUtils.hpp"

#include <terralib/raster/Raster.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/srs/Converter.h>
#include <terralib/geometry/Utils.h>
#include <terralib/common/StringUtils.h>

// Boost Python
#include <boost/python/call.hpp>
#include <boost/python/object.hpp>
#include <boost/python/import.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/make_function.hpp>
#include <boost/bind.hpp>

terrama2::services::analysis::core::GridContext::GridContext(terrama2::services::analysis::core::DataManagerPtr dataManager, terrama2::services::analysis::core::AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
  : BaseContext(dataManager, analysis, startTime)
{
  createOutputRaster();
}

std::shared_ptr<te::rst::Raster> terrama2::services::analysis::core::GridContext::getOutputRaster()
{
  return outputRaster_;
}

void terrama2::services::analysis::core::GridContext::createOutputRaster()
{
  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  try
  {
    auto rinfo = getOutputRasterInfo();

    te::rst::Grid* grid = nullptr;
    std::vector<te::rst::BandProperty*> bands;
    std::tie(grid, bands) = terrama2::services::analysis::core::getOutputRasterInfo(rinfo);
    assert(grid);
    outputRaster_.reset(te::rst::RasterFactory::make("EXPANSIBLE", grid, bands, {}));
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    QString errMsg = QObject::tr("Could not create output raster.\n%1").arg(e.what());
    throw Exception() << ErrorDescription(errMsg);
  }

}

te::gm::Coord2D terrama2::services::analysis::core::GridContext::convertoTo(const te::gm::Coord2D& point, const int srid)
{
  te::gm::Coord2D newPoint;
  std::shared_ptr<te::srs::Converter> converter;

  auto it = converterMap_.find(srid);
  if(it == converterMap_.end())
  {
    converter.reset(new te::srs::Converter());
    auto raster = getOutputRaster();
    auto outputSrid = raster->getSRID();

    converter->setSourceSRID(outputSrid);
    converter->setTargetSRID(srid);

    converterMap_.emplace(srid, converter);
  }
  else
    converter = it->second;

  double x = point.getX();
  double y = point.getY();

  converter->convert(x,y);

  newPoint.x = x;
  newPoint.y = y;

  return newPoint;
}


std::vector< std::shared_ptr<te::rst::Raster> >
terrama2::services::analysis::core::GridContext::getRasterList(const terrama2::core::DataSeriesPtr& dataSeries,
    const DataSetId datasetId, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  ObjectKey key;
  key.objectId_ = datasetId;
  key.dateFilter_ = dateDiscardBefore+dateDiscardAfter;

  auto it = rasterMap_.find(key);
  if(it != rasterMap_.end())
    return it->second;
  else
  {
    auto dataManager = dataManager_.lock();
    if(!dataManager.get())
    {
      //FIXME: throw if no dataManager
      assert(0);
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
        auto outputGridConfig = getOutputRasterInfo();
        auto resampledRaster = terrama2::services::analysis::core::resampleRaster(dsRaster, outputGridConfig, analysis_->outputGridPtr->interpolationMethod);

        addRaster(key, resampledRaster);
      }
    });

    return rasterMap_[key];
  }
}

void terrama2::services::analysis::core::GridContext::addRaster(ObjectKey key, std::shared_ptr<te::rst::Raster> raster)
{
  rasterMap_[key].push_back(raster);
}

std::map<std::string, std::string> terrama2::services::analysis::core::GridContext::getOutputRasterInfo()
{
  if(outputRasterInfo_.empty())
  {
    outputRasterInfo_["MEM_SRC_RASTER_DRIVER_TYPE"] = "GDAL";
    outputRasterInfo_["MEM_RASTER_DATATYPE"] = te::common::Convert2String(te::dt::DOUBLE_TYPE);
    outputRasterInfo_["MEM_RASTER_NBANDS"] = "1";

    if(!analysis_->outputGridPtr)
    {
      QString errMsg = QObject::tr("Invalid output grid configuration.");
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    outputRasterInfo_["MEM_RASTER_NODATA"] = std::to_string(analysis_->outputGridPtr->interpolationDummy);
    addInterestAreaToRasterInfo(outputRasterInfo_);
    addResolutionToRasterInfo(outputRasterInfo_);

    auto resX = std::stod(outputRasterInfo_["MEM_RASTER_RES_X"]);
    auto resY = std::stod(outputRasterInfo_["MEM_RASTER_RES_Y"]);

    auto minX = std::stod(outputRasterInfo_["MEM_RASTER_MIN_X"]);
    auto minY = std::stod(outputRasterInfo_["MEM_RASTER_MIN_Y"]);
    auto maxX = std::stod(outputRasterInfo_["MEM_RASTER_MAX_X"]);
    auto maxY = std::stod(outputRasterInfo_["MEM_RASTER_MAX_Y"]);

    auto nRows = static_cast<unsigned int>(std::abs(std::ceil((maxY - minY) / resY)));
    auto nCols = static_cast<unsigned int>(std::abs(std::ceil((maxX - minX) / resX)));

    outputRasterInfo_["MEM_RASTER_NROWS"] = std::to_string(nRows);
    outputRasterInfo_["MEM_RASTER_NCOLS"] = std::to_string(nCols);
  }

  return outputRasterInfo_;
}

void terrama2::services::analysis::core::GridContext::addResolutionToRasterInfo(std::map<std::string, std::string>& outputRasterInfo)
{
  double resX = 0;
  double resY = 0;

  auto dataManager = dataManager_.lock();
  if(!dataManager.get())
  {
    //FIXME: throw invalid datamanager;
    return;
  }

  switch(analysis_->outputGridPtr->resolutionType)
  {
    case ResolutionType::SAME_FROM_DATASERIES:
    {
      try
      {
        auto gridMap = getGridMap(dataManager, analysis_->outputGridPtr->resolutionDataSeriesId);
        if(gridMap.empty())
        {
          QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis_->outputGridPtr->resolutionDataSeriesId);
          throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
        }

        auto raster = gridMap.begin()->second;
        if(!raster)
        {
          QString errMsg = QObject::tr("Could not recover raster for dataset: %1.").arg(gridMap.begin()->first->id);
          throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
        }

        resX = raster->getResolutionX();
        resY = raster->getResolutionY();
      }
      catch(terrama2::core::NoDataException e)
      {
      }

      break;
    }
    case ResolutionType::BIGGEST_GRID:
    {

      double  pixelArea = 0;
      for(auto analysisDataSeries : analysis_->analysisDataSeriesList)
      {
        try
        {
          auto gridMap = getGridMap(dataManager, analysisDataSeries.dataSeriesId);

          if(gridMap.empty())
          {
            QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis_->outputGridPtr->resolutionDataSeriesId);
            throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
          }

          auto raster = gridMap.begin()->second;
          if(!raster)
          {
            QString errMsg = QObject::tr("Could not recover raster for dataset: %1.").arg(gridMap.begin()->first->id);
            throw terrama2::InvalidArgumentException() <<
                ErrorDescription(errMsg);
          }

          // Calculates the area of the pixel
          double area = raster->getResolutionX() * raster->getResolutionY();
          if(area >= pixelArea)
          {
            // In case the area is the same, gives priority to the greater in X.
            if(area == pixelArea)
            {
              if(raster->getResolutionX() > resX)
              {
                resX = raster->getResolutionX();
                resY = raster->getResolutionY();
                pixelArea = area;
              }
            }
            else
            {
              resX = raster->getResolutionX();
              resY = raster->getResolutionY();
              pixelArea = area;
            }
          }
        }
        catch(terrama2::core::NoDataException e)
        {
          continue;
        }
      }
      break;
    }
    case ResolutionType::SMALLEST_GRID:
    {

      double  pixelArea = std::numeric_limits<double>::max();
      for(auto analysisDataSeries : analysis_->analysisDataSeriesList)
      {
        try
        {
          auto gridMap = getGridMap(dataManager, analysisDataSeries.dataSeriesId);

          if(gridMap.empty())
          {
            QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis_->outputGridPtr->resolutionDataSeriesId);
            throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
          }

          auto raster = gridMap.begin()->second;
          if(!raster)
          {
            QString errMsg = QObject::tr("Could not recover raster for dataset: %1.").arg(gridMap.begin()->first->id);
            throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
          }

          // Calculates the area of the pixel
          double area = raster->getResolutionX() * raster->getResolutionY();
          if(area <= pixelArea)
          {
            // In case the area is the same, gives priority to the greater in X.
            if(area == pixelArea)
            {
              if(raster->getResolutionX() > resX)
              {
                resX = raster->getResolutionX();
                resY = raster->getResolutionY();
                pixelArea = area;
              }
              //TODO: shouldn't have an else?
            }
            else
            {
              resX = raster->getResolutionX();
              resY = raster->getResolutionY();
              pixelArea = area;
            }
          }
        }
        catch(terrama2::core::NoDataException e)
        {
          continue;
        }

      }
      break;
    }
    case ResolutionType::CUSTOM:
    {
      resX = analysis_->outputGridPtr->resolutionX;
      resY = analysis_->outputGridPtr->resolutionY;
      break;
    }
  }

  outputRasterInfo["MEM_RASTER_RES_X"] = std::to_string(resX);
  outputRasterInfo["MEM_RASTER_RES_Y"] = std::to_string(resY);
}

void terrama2::services::analysis::core::GridContext::addInterestAreaToRasterInfo(std::map<std::string, std::string>& outputRasterInfo)
{
  Srid srid = 0;
  std::shared_ptr<te::gm::Envelope> box(new te::gm::Envelope());

  auto dataManager = dataManager_.lock();
  if(!dataManager.get())
  {
    //FIXME: throw invalid datamanager;
    return;
  }

  switch(analysis_->outputGridPtr->interestAreaType)
  {
    case InterestAreaType::UNION:
    {
      for(auto analysisDataSeries : analysis_->analysisDataSeriesList)
      {
        try
        {
          auto gridMap = getGridMap(dataManager, analysisDataSeries.dataSeriesId);

          if(gridMap.empty())
          {
            QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis_->outputGridPtr->resolutionDataSeriesId);
            throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
          }

          auto raster = gridMap.begin()->second;
          if(!raster)
          {
            QString errMsg = QObject::tr("Could not recover raster for dataset: %1.").arg(gridMap.begin()->first->id);
            throw terrama2::InvalidArgumentException()
                << ErrorDescription(errMsg);
          }

          if(srid == 0)
          {
            box->Union(*raster->getExtent());
            srid = raster->getSRID();
            continue;
          }

          std::shared_ptr<te::gm::Geometry> geomBox(te::gm::GetGeomFromEnvelope(raster->getExtent(), raster->getSRID()));
          if(raster->getSRID() != srid)
          {
            geomBox->transform(srid);
          }

          box->Union(*geomBox->getMBR());
        }
        catch(terrama2::core::NoDataException e)
        {
          continue;
        }
      }
      break;
    }
    case InterestAreaType::SAME_FROM_DATASERIES:
    {
      try
      {
        auto gridMap = getGridMap(dataManager, analysis_->outputGridPtr->interestAreaDataSeriesId);
        if(gridMap.empty())
        {
          QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis_->outputGridPtr->resolutionDataSeriesId);
          throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
        }

        auto raster = gridMap.begin()->second;
        if(!raster)
        {
          QString errMsg = QObject::tr("Could not recover raster for dataset: %1.").arg(gridMap.begin()->first->id);
          throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
        }

        box->Union(*raster->getExtent());
        srid = raster->getSRID();
      }
      catch(terrama2::core::NoDataException e)
      {
      }

      break;
    }
    case InterestAreaType::CUSTOM:
    {
      box->Union(*analysis_->outputGridPtr->interestAreaBox->getMBR());
      srid = analysis_->outputGridPtr->interestAreaBox->getSRID();
      break;
    }
  }

  outputRasterInfo["MEM_RASTER_SRID"] = std::to_string(srid);
  outputRasterInfo["MEM_RASTER_MIN_X"] = std::to_string(box->getLowerLeftX());
  outputRasterInfo["MEM_RASTER_MIN_Y"] = std::to_string(box->getLowerLeftY());
  outputRasterInfo["MEM_RASTER_MAX_X"] = std::to_string(box->getUpperRightX());
  outputRasterInfo["MEM_RASTER_MAX_Y"] = std::to_string(box->getUpperRightY());
}

std::unordered_multimap<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> >
terrama2::services::analysis::core::GridContext::getGridMap(terrama2::services::analysis::core::DataManagerPtr dataManager,
    DataSeriesId dataSeriesId,
    const std::string& dateDiscardBefore,
    const std::string& dateDiscardAfter)
{
  ObjectKey key;
  key.objectId_ = dataSeriesId;
  key.dateFilter_ = dateDiscardBefore+dateDiscardAfter;

  auto it = analysisInputGrid_.find(key);
  if(it == analysisInputGrid_.end())
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

    terrama2::core::Filter filter;
    filter.lastValue = true;

    filter.discardAfter = startTime_;
    if(!dateDiscardBefore.empty() || !dateDiscardAfter.empty())
    {
      if(!dateDiscardBefore.empty())
      {
        boost::local_time::local_date_time ldt = terrama2::core::TimeUtils::nowBoostLocal();
        double seconds = terrama2::core::TimeUtils::convertTimeString(dateDiscardBefore, "SECOND", "h");
        //TODO: PAULO: review losing precision
        ldt -= boost::posix_time::seconds(seconds);

        std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
        filter.discardBefore = std::move(titz);

        filter.lastValue = false;
      }

      if(!dateDiscardAfter.empty())
      {
        boost::local_time::local_date_time ldt = terrama2::core::TimeUtils::nowBoostLocal();
        double seconds = terrama2::core::TimeUtils::convertTimeString(dateDiscardAfter, "SECOND", "h");
        ldt -= boost::posix_time::seconds(seconds);

        std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
        filter.discardAfter = std::move(titz);

        filter.lastValue = false;
      }
    }
    else
    {
      // no interval set,
      // use analysis execution timestamp as last valid date
      filter.discardAfter = std::unique_ptr<te::dt::TimeInstantTZ>(static_cast<te::dt::TimeInstantTZ*>(startTime_->clone()));
    }


    auto gridSeries = accessorGrid->getGridSeries(filter);

    if(!gridSeries)
    {
      QString errMsg = QObject::tr("Invalid grid series for data series: %1.").arg(dataSeriesId);
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    auto inputGrid =  gridSeries->gridMap();
    analysisInputGrid_.emplace(key, inputGrid);
    return inputGrid;
  }

  return it->second;
}
