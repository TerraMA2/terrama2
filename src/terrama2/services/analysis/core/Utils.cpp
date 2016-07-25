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

std::unordered_multimap<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> >
terrama2::services::analysis::core::getGridMap2(DataManagerPtr dataManager, DataSeriesId dataSeriesId)
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

  return gridSeries->gridMap2();
}

std::tuple<te::rst::Grid*, const std::vector<te::rst::BandProperty*> >
terrama2::services::analysis::core::getOutputRasterInfo(std::map<std::string, std::string> rinfo)
{
  auto ncols = static_cast<uint>(std::stoi(rinfo["MEM_RASTER_NCOLS"]));
  auto nrows = static_cast<uint>(std::stoi(rinfo["MEM_RASTER_NROWS"]));
  auto srid = std::stoi(rinfo["MEM_RASTER_SRID"]);

  double minx = std::stod(rinfo["MEM_RASTER_MIN_X"]);
  double miny = std::stod(rinfo["MEM_RASTER_MIN_Y"]);
  double maxx = std::stod(rinfo["MEM_RASTER_MAX_X"]);
  double maxy = std::stod(rinfo["MEM_RASTER_MAX_Y"]);
  double resx = std::stod(rinfo["MEM_RASTER_RES_X"]);
  double resy = std::stod(rinfo["MEM_RASTER_RES_Y"]);

  te::gm::Envelope* mbr = new te::gm::Envelope(minx, miny, maxx, maxy);

  auto grid = new te::rst::Grid(ncols, nrows, resx, resy, mbr, srid);

  std::vector<te::rst::BandProperty*> bands;
  auto dt = std::stoi(rinfo["MEM_RASTER_DATATYPE"]);
  std::size_t nbands = static_cast<std::size_t>(std::stoi(rinfo["MEM_RASTER_NBANDS"]));
  for(std::size_t b = 0; b < nbands; ++b)
  {
    te::rst::BandProperty* ibprop = new te::rst::BandProperty(b, dt);

    ibprop->m_blkh = 1;
    ibprop->m_blkw = ncols;
    ibprop->m_nblocksx = 1;
    ibprop->m_nblocksy = nrows;

    bands.push_back(ibprop);
  }

  return std::make_tuple(grid, bands);
}

std::shared_ptr<te::rst::Raster>
terrama2::services::analysis::core::reprojectRaster(std::shared_ptr<te::rst::Raster> inputRaster,
    std::map<std::string, std::string> outputRasterInfo,
    InterpolationMethod method)
{
  auto oldNRows = inputRaster->getNumberOfRows();
  auto oldNCols = inputRaster->getNumberOfColumns();

  unsigned int rows = static_cast<unsigned int>(std::stoi(outputRasterInfo["MEM_RASTER_NROWS"]));
  unsigned int cols = static_cast<unsigned int>(std::stoi(outputRasterInfo["MEM_RASTER_NCOLS"]));

  std::vector< unsigned int > inputRasterBands;
  for(unsigned int i = 0; i < inputRaster->getNumberOfBands(); ++i)
  {
    inputRasterBands.push_back(i);
  }

//  std::auto_ptr<te::rst::Raster> resampledRasterPtr(te::rst::RasterFactory::make("MEM", 0, std::vector<te::rst::BandProperty*>(), outputRasterInfo));

  te::rst::Grid* grid = nullptr;
  std::vector<te::rst::BandProperty*> bands;
  std::tie(grid, bands) = terrama2::services::analysis::core::getOutputRasterInfo(outputRasterInfo);
  assert(grid);
  std::auto_ptr<te::rst::Raster> resampledRasterPtr(te::rst::RasterFactory::make("EXPANSIBLE", grid, bands, {}));
  auto ok = te::rp::RasterResample(*inputRaster, inputRasterBands, (te::rst::Interpolator::Method)method, 0, 0, oldNRows,
                                   oldNCols, rows, cols, outputRasterInfo,
                                   "EXPANSIBLE" ,resampledRasterPtr);

  assert(ok);

  return std::shared_ptr<te::rst::Raster>(resampledRasterPtr.release());
}
