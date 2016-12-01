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

#include "../DataManager.hpp"
#include "Utils.hpp"
#include "../../../../core/Exception.hpp"
#include "../../../../core/data-model/Filter.hpp"
#include "../../../../core/utility/DataAccessorFactory.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/Raii.hpp"
#include "../../../../core/data-access/DataAccessor.hpp"
#include "../../../../core/data-access/DataAccessorGrid.hpp"
#include "../../../../core/data-access/GridSeries.hpp"

// TerraLib
#include <terralib/common/StringUtils.h>
#include <terralib/raster/Reprojection.h>
#include <terralib/memory/Raster.h>
#include <terralib/rp/Functions.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// QT
#include <QObject>
#include <QUrl>

//STL
#include <cmath>


terrama2::services::analysis::core::AnalysisType terrama2::services::analysis::core::ToAnalysisType(uint32_t type)
{
  switch(type)
  {
    case 1:
      return AnalysisType::DCP_TYPE;
    case 2:
      return AnalysisType::MONITORED_OBJECT_TYPE;
    case 3:
      return AnalysisType::GRID_TYPE;
  }

  QString errMsg = QObject::tr("Invalid analysis type");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);

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
    case 1:
      return ResolutionType::SMALLEST_GRID;
    case 2:
      return ResolutionType::BIGGEST_GRID;
    case 3:
      return ResolutionType::SAME_FROM_DATASERIES;
    case 4:
      return ResolutionType::CUSTOM;
    default:
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid resolution type"));
  }
}

terrama2::services::analysis::core::InterestAreaType terrama2::services::analysis::core::ToInterestAreaType(uint32_t interestAreaType)
{
  switch(interestAreaType)
  {
    case 1:
      return InterestAreaType::UNION;
    case 2:
      return InterestAreaType::SAME_FROM_DATASERIES;
    case 3:
      return InterestAreaType::CUSTOM;
    default:
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid interest area type"));
  }
}

std::unordered_multimap<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> >
terrama2::services::analysis::core::getGridMap(DataManagerPtr dataManager, DataSeriesId dataSeriesId, std::shared_ptr<terrama2::core::FileRemover> remover)
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
  auto gridSeries = accessorGrid->getGridSeries(filter, remover);

  if(!gridSeries)
  {
    QString errMsg = QObject::tr("Invalid grid series for data series: %1.").arg(dataSeriesId);
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  return gridSeries->gridMap();
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
  double nodata = std::stod(rinfo["MEM_RASTER_NODATA"]);

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
    ibprop->m_noDataValue = nodata;

    bands.push_back(ibprop);
  }

  return std::make_tuple(grid, bands);
}

double terrama2::services::analysis::core::getValue(terrama2::core::SynchronizedDataSetPtr syncDs,
    const std::string& attribute, uint32_t i, int attributeType)
{
  if(attribute.empty())
    return std::nan("");

  double value = std::nan("");
  switch(attributeType)
  {
    case te::dt::INT16_TYPE:
    {
      value = syncDs->getInt16(i, attribute);
    }
    break;
    case te::dt::INT32_TYPE:
    {
      value = syncDs->getInt32(i, attribute);
    }
    break;
    case te::dt::INT64_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getInt64(i, attribute));
    }
    break;
    case te::dt::DOUBLE_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getDouble(i, attribute));
    }
    break;
    case te::dt::NUMERIC_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getNumeric(i, attribute));
    }
    break;
    default:
      break;
  }

  return value;
}

void terrama2::services::analysis::core::calculateStatistics(std::vector<double>& values, OperatorCache& cache)
{
  if(values.size() == 0)
    return;

  for(const double& value : values)
  {
    cache.sum += value;

    if(value < cache.min)
      cache.min = value;

    if(value > cache.max)
      cache.max = value;
  }

  cache.count = values.size();
  cache.mean = cache.sum / cache.count;
  std::sort(values.begin(), values.end());
  double half = values.size() / 2;
  if(values.size() > 1 && values.size() % 2 == 0)
  {
    cache.median = (values[(int) half] + values[(int) half - 1]) / 2.;
  }
  else
  {
    cache.median = values.size() == 1 ? values[0] : 0.;
  }

  // calculates the variance
  double sumVariance = 0.;
  for(const double& value : values)
  {
    double diff = value - cache.mean;
    sumVariance += diff*diff;
  }

  cache.variance = sumVariance / cache.count;
  cache.standardDeviation = std::sqrt(cache.variance);
}


double terrama2::services::analysis::core::getOperationResult(OperatorCache& cache, StatisticOperation statisticOperation)
{
  switch(statisticOperation)
  {
    case StatisticOperation::SUM:
      return cache.sum;
    case StatisticOperation::MEAN:
      return cache.mean;
    case StatisticOperation::MIN:
      return cache.min;
    case StatisticOperation::MAX:
      return cache.max;
    case StatisticOperation::STANDARD_DEVIATION:
      return cache.standardDeviation;
    case StatisticOperation::MEDIAN:
      return cache.median;
    case StatisticOperation::COUNT:
      return cache.count;
    case StatisticOperation::VARIANCE:
      return cache.variance;
    default:
      return std::nan("");
  }
}


void terrama2::services::analysis::core::erasePreviousResult(DataManagerPtr dataManager, DataSeriesId dataSeriesId, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
{
  auto outputDataSeries = dataManager->findDataSeries(dataSeriesId);
  if(!outputDataSeries)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Invalid output data series for analysis.");
    return;
  }
  auto outputDataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);
  if(!outputDataProvider)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Invalid output data provider for analysis.");
    return;
  }

  if(outputDataProvider->dataProviderType == "POSTGIS")
  {
    auto dataset = outputDataSeries->datasetList[0];
    std::string tableName;

    try
    {
      tableName = dataset->format.at("table_name");
    }
    catch(...)
    {
      QString errMsg = QObject::tr("Undefined table name in dataset: %1.").arg(dataset->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::UndefinedTagException() << ErrorDescription(errMsg);
    }

    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("POSTGIS", outputDataProvider->uri));

    // RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    if(!datasource->isOpened())
    {
      QString errMsg = QObject::tr("DataProvider could not be opened.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

    auto dataSetNames = transactor->getDataSetNames();

    if(std::find(dataSetNames.cbegin(), dataSetNames.cend(), tableName) != dataSetNames.cend())
      transactor->execute("delete from " + tableName + " where execution_date = '" + startTime->toString() + "'");
  }

}
