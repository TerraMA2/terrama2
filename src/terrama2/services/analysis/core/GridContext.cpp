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
#include "utility/Utils.hpp"
#include "python/PythonInterpreter.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Verify.hpp"
#include "../../../core/data-model/DataSetGrid.hpp"

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

  try
  {
    converter = converterMap_.at(srid);
  }
  catch (const std::out_of_range&)
  {
    converter.reset(new te::srs::Converter());
    auto raster = getOutputRaster();
    auto outputSrid = raster->getSRID();

    converter->setSourceSRID(outputSrid);
    converter->setTargetSRID(srid);

    converterMap_.emplace(srid, converter);
  }

  double x = point.getX();
  double y = point.getY();

  converter->convert(x,y);

  newPoint.x = x;
  newPoint.y = y;

  return newPoint;
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
    QString errMsg = QObject::tr("Unable to access DataManager");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
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
      catch(const terrama2::core::NoDataException&)
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
        catch(const terrama2::core::NoDataException&)
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
            }
            else
            {
              resX = raster->getResolutionX();
              resY = raster->getResolutionY();
              pixelArea = area;
            }
          }
        }
        catch(const terrama2::core::NoDataException&)
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
    QString errMsg = QObject::tr("Unable to access DataManager");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
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
            QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysisDataSeries.dataSeriesId);
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
        catch(const terrama2::core::NoDataException&)
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
          QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis_->outputGridPtr->interestAreaDataSeriesId);
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
      catch(const terrama2::core::NoDataException&)
      {
        QString errMsg = QObject::tr("Could not copy raster configuration from data series %1 because there is no data available.").arg(analysis_->outputGridPtr->interestAreaDataSeriesId);
        throw InvalidDataSeriesException() << ErrorDescription(errMsg);
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

  terrama2::core::verify::srid(srid);

  outputRasterInfo["MEM_RASTER_SRID"] = std::to_string(srid);
  outputRasterInfo["MEM_RASTER_MIN_X"] = std::to_string(box->getLowerLeftX());
  outputRasterInfo["MEM_RASTER_MIN_Y"] = std::to_string(box->getLowerLeftY());
  outputRasterInfo["MEM_RASTER_MAX_X"] = std::to_string(box->getUpperRightX());
  outputRasterInfo["MEM_RASTER_MAX_Y"] = std::to_string(box->getUpperRightY());
}
