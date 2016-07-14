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
  \file terrama2/services/analysis/core/Utils.cpp

  \brief Utility functions for TerraMA2 Analysis module.

  \author Paulo R. M. Oliveira
*/

#include "DataManager.hpp"
#include "Context.hpp"
#include "Utils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataAccessorGrid.hpp"
#include "../../../core/data-access/GridSeries.hpp"


// TerraLib
#include <terralib/common/StringUtils.h>
#include <terralib/raster/Reprojection.h>
#include <terralib/memory/Raster.h>
#include <terralib/rp/Functions.h>

// QT
#include <QObject>


terrama2::services::analysis::core::AnalysisType terrama2::services::analysis::core::ToAnalysisType(uint32_t type)
{
  switch(type)
  {
    case 1:
      return AnalysisType::PCD_TYPE;
    case 2:
      return AnalysisType::MONITORED_OBJECT_TYPE;
    case 3:
      return AnalysisType::GRID_TYPE;
  }

  throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid analysis type"));

}


terrama2::services::analysis::core::AnalysisDataSeriesType terrama2::services::analysis::core::ToAnalysisDataSeriesType(uint32_t type)
{
  switch(type)
  {
    case 1:
      return AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    case 2:
      return AnalysisDataSeriesType::DATASERIES_GRID_TYPE;
    case 3:
      return AnalysisDataSeriesType::DATASERIES_PCD_TYPE;
    case 4:
      return AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;
    default:
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid analysis data series type"));
  }


}

terrama2::services::analysis::core::ScriptLanguage terrama2::services::analysis::core::ToScriptLanguage(uint32_t scriptLanguage)
{
  switch(scriptLanguage)
  {
    case 1:
      return ScriptLanguage::PYTHON;
    case 2:
      return ScriptLanguage::LUA;
    default:
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid script language"));
  }
}


terrama2::services::analysis::core::InterpolationMethod terrama2::services::analysis::core::ToInterpolationMethod(uint32_t interpolationMethod)
{
  switch(interpolationMethod)
  {
    case 0:
      return InterpolationMethod::UNDEFINTERPMETHOD;
    case 1:
      return InterpolationMethod::NEARESTNEIGHBOR;
    case 2:
      return InterpolationMethod::BILINEAR;
    case 3:
      return InterpolationMethod::BICUBIC;
    default:
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid interpolation method"));
  }
}

terrama2::services::analysis::core::ResolutionType terrama2::services::analysis::core::ToResolutionType(uint32_t resolutionType)
{
  switch(resolutionType)
  {
    case 0:
      return ResolutionType::SMALLEST_GRID;
    case 1:
      return ResolutionType::BIGGEST_GRID;
    case 2:
      return ResolutionType::SAME_FROM_DATASERIES;
    case 3:
      return ResolutionType::CUSTOM;
    default:
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid resolution type"));
  }
}

terrama2::services::analysis::core::InterestAreaType terrama2::services::analysis::core::ToInterestAreaType(uint32_t interestAreaType)
{
  switch(interestAreaType)
  {
    case 0:
      return InterestAreaType::UNION;
    case 1:
      return InterestAreaType::SAME_FROM_DATASERIES;
    case 2:
      return InterestAreaType::CUSTOM;
    default:
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid interest area type"));
  }
}

const std::unordered_map<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> > terrama2::services::analysis::core::getGridMap(DataManagerPtr dataManager, DataSeriesId dataSeriesId)
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
  auto gridSeries = accessorGrid->getGridSeries(filter);

  if(!gridSeries)
  {
    QString errMsg = QObject::tr("Invalid grid series for data series: %1.").arg(dataSeriesId);
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return gridSeries->gridMap();
}

std::map<std::string, std::string> terrama2::services::analysis::core::getOutputRasterInfo(DataManagerPtr dataManager, AnalysisHashCode analysisHashCode)
{
  auto analysis = Context::getInstance().getAnalysis(analysisHashCode);

  if(!analysis->outputGridPtr)
  {
    QString errMsg(QObject::tr("Invalid analysis output grid configuration."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  if(!dataManager)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  double resX = 0;
  double resY = 0;
  unsigned int nRows = 0;
  unsigned int nCols = 0;
  Srid srid = 0;
  std::shared_ptr<te::gm::Envelope> box(new te::gm::Envelope());


  switch(analysis->outputGridPtr->interestAreaType)
  {
    case InterestAreaType::UNION:
    {
      for(auto analysisDataSeries : analysis->analysisDataSeriesList)
      {
        try
        {
          auto gridMap = getGridMap(dataManager, analysisDataSeries.dataSeriesId);

          if(gridMap.empty())
          {
            QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis->outputGridPtr->resolutionDataSeriesId);
            throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
          }

          auto raster = gridMap.begin()->second;
          if(!raster)
          {
            QString errMsg = QObject::tr("Could not recover raster for dataset: %1.").arg(gridMap.begin()->first->id);
            throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
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
        auto gridMap = getGridMap(dataManager, analysis->outputGridPtr->interestAreaDataSeriesId);
        if(gridMap.empty())
        {
          QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis->outputGridPtr->resolutionDataSeriesId);
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
      box->Union(*analysis->outputGridPtr->interestAreaBox->getMBR());
      srid = analysis->outputGridPtr->interestAreaBox->getSRID();
      break;
    }
  }

  switch(analysis->outputGridPtr->resolutionType)
  {
    case ResolutionType::SAME_FROM_DATASERIES:
    {
      try
      {
        auto gridMap = getGridMap(dataManager, analysis->outputGridPtr->resolutionDataSeriesId);
        if(gridMap.empty())
        {
          QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis->outputGridPtr->resolutionDataSeriesId);
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
      for(auto analysisDataSeries : analysis->analysisDataSeriesList)
      {
        try
        {
          auto gridMap = getGridMap(dataManager, analysisDataSeries.dataSeriesId);

          if(gridMap.empty())
          {
            QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis->outputGridPtr->resolutionDataSeriesId);
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
      for(auto analysisDataSeries : analysis->analysisDataSeriesList)
      {
        try
        {
          auto gridMap = getGridMap(dataManager, analysisDataSeries.dataSeriesId);

          if(gridMap.empty())
          {
            QString errMsg = QObject::tr("Could not recover grid for data series: %1.").arg(analysis->outputGridPtr->resolutionDataSeriesId);
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
        catch(terrama2::core::NoDataException e)
        {
          continue;
        }

      }
      break;
    }
    case ResolutionType::CUSTOM:
    {
      resX = analysis->outputGridPtr->resolutionX;
      resY = analysis->outputGridPtr->resolutionY;
      break;
    }
  }

  nRows = (unsigned int)std::abs(std::ceil((box->getUpperRightY() - box->getLowerLeftY()) / resY));
  nCols = (unsigned int)std::abs(std::ceil((box->getUpperRightX() - box->getLowerLeftX()) / resX));


  std::map<std::string, std::string> rinfo;

  rinfo["MEM_SRC_RASTER_DRIVER_TYPE"] = "GDAL";
  rinfo["MEM_RASTER_NROWS"] = std::to_string(nRows);
  rinfo["MEM_RASTER_NCOLS"] = std::to_string(nCols);
  rinfo["MEM_RASTER_RES_X"] = std::to_string(resX);
  rinfo["MEM_RASTER_RES_Y"] = std::to_string(resY);
  rinfo["MEM_RASTER_DATATYPE"] = te::common::Convert2String(te::dt::DOUBLE_TYPE);
  rinfo["MEM_RASTER_NBANDS"] = "1";
  rinfo["MEM_RASTER_SRID"] = std::to_string(srid);
  rinfo["MEM_RASTER_MIN_X"] = std::to_string(box->getLowerLeftX());
  rinfo["MEM_RASTER_MIN_Y"] = std::to_string(box->getLowerLeftY());
  rinfo["MEM_RASTER_MAX_X"] = std::to_string(box->getUpperRightX());
  rinfo["MEM_RASTER_MAX_Y"] = std::to_string(box->getUpperRightY());

  return rinfo;
}

std::shared_ptr<te::rst::Raster>
terrama2::services::analysis::core::reprojectRaster(std::shared_ptr<te::rst::Raster> inputRaster,
    std::map<std::string, std::string> outputRasterInfo,
    InterpolationMethod method)
{
  int resX = std::stoi(outputRasterInfo["MEM_RASTER_RES_X"]);
  int resY = std::stoi(outputRasterInfo["MEM_RASTER_RES_Y"]);
  int srid = std::stoi(outputRasterInfo["MEM_RASTER_SRID"]);
  double llx = std::stof(outputRasterInfo["MEM_RASTER_MIN_X"]);
  double lly = std::stof(outputRasterInfo["MEM_RASTER_MIN_Y"]);
  double urx = std::stof(outputRasterInfo["MEM_RASTER_MAX_X"]);
  double ury = std::stof(outputRasterInfo["MEM_RASTER_MAX_Y"]);

  // std::shared_ptr<te::rst::Raster> reprojectedRaster(te::rst::Reproject(inputRaster.get(), srid, llx, lly, urx, ury,
  //     resX, resY, outputRasterInfo, (int)method));

  std::shared_ptr<te::gm::Envelope> box(inputRaster->getExtent());
  auto oldNRows = (unsigned int)std::abs(std::ceil((box->getUpperRightY() - box->getLowerLeftY()) / resY));
  auto oldNCols = (unsigned int)std::abs(std::ceil((box->getUpperRightX() - box->getLowerLeftX()) / resX));

  int rows = std::stoi(outputRasterInfo["MEM_RASTER_NROWS"]);
  int cols = std::stoi(outputRasterInfo["MEM_RASTER_NCOLS"]);

  std::vector< unsigned int > inputRasterBands;


//  std::auto_ptr< te::rst::Raster > resampledRasterPtr(new te::mem::Raster());
  std::auto_ptr<te::rst::Raster> resampledRasterPtr(te::rst::RasterFactory::make("MEM", 0, std::vector<te::rst::BandProperty*>(), outputRasterInfo));
  auto ok = te::rp::RasterResample(*inputRaster, inputRasterBands, (te::rst::Interpolator::Method)method, 0, 0, oldNRows,
                                   oldNCols, rows, cols, outputRasterInfo,
                                   "MEM" ,resampledRasterPtr);

  return std::shared_ptr<te::rst::Raster>(resampledRasterPtr.release());
}
