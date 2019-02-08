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
#include "../../../../core/utility/TimeUtils.hpp"
#include "../../../../core/utility/Utils.hpp"
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
#include <terralib/datatype/Utils.h>

// QT
#include <QObject>
#include <QUrl>

//STL
#include <cmath>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

using namespace boost::accumulators;

terrama2::services::analysis::core::AnalysisType terrama2::services::analysis::core::ToAnalysisType(uint32_t type)
{
  //Map webapp enum to c++ enum
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
      throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid analysis data series getInt64type"));
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
  filter.lastValues = std::make_shared<size_t>(1);
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
      throw terrama2::InvalidArgumentException() << ErrorDescription( QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(attribute.c_str())).arg(te::dt::ConvertDataTypeToString(attributeType).c_str()));
  }

  return value;
}

/*!
 Calculates the variance of the elements of a container

 source: http://roth.cs.kuleuven.be/w-ess/index.php/Accurate_variance_and_mean_calculations_in_C%2B%2B11

 */
template <typename T, typename Container>
T internal_variance(const Container& xs)
{
    // begin(xs) will point to the first element in xs
    // end(xs) will point to the last element in xs
    // the distance between them gives the number of elements
    size_t N = end(xs) - begin(xs);
    // first pass through all data (hidden in accumulate):
    T m = std::accumulate(begin(xs), end(xs), T(0)) / N;
    T s2 = 0;
    // second pass through all data:
    for(auto x : xs) {
        s2 += (x - m) * (x - m);
    }
    return s2 / (N-1);
}

void terrama2::services::analysis::core::calculateStatistics(std::vector<double>& values, OperatorCache& cache)
{
  if(values.empty())
    return;

  accumulator_set<double, stats<tag::min, tag::sum, tag::max, tag::mean, tag::variance, tag::median> > acc;
  acc = std::for_each(values.begin(), values.end(), acc);

  cache.count = values.size();
  cache.sum = sum(acc);
  cache.mean = mean(acc);
  cache.min = min(acc);
  cache.max = max(acc);
  cache.median = median(acc);
  //============================================
  // WARNING
  //
  // The boost function returned a different then expected value
  // check internal_variance
  //
  //  cache.variance = variance(acc);
  cache.variance = internal_variance<double>(values);
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

std::pair<size_t, size_t> terrama2::services::analysis::core::getBandInterval(terrama2::core::DataSetPtr dataset, double secondsPassed, std::string dateDiscardBefore, std::string dateDiscardAfter)
{
  auto intervalStr = terrama2::core::getTimeInterval(dataset);
  double interval = terrama2::core::TimeUtils::convertTimeString(intervalStr, "SECOND", "H");
  double secondsToBefore = terrama2::core::TimeUtils::convertTimeString(dateDiscardBefore, "SECOND", "H");
  double secondsToAfter = terrama2::core::TimeUtils::convertTimeString(dateDiscardAfter, "SECOND", "H");

  // - find how much time has passed from the file original timestamp
  size_t temp = static_cast<size_t>(std::floor((secondsPassed + secondsToBefore)/interval));
  size_t bandBegin = static_cast<size_t>(std::ceil((secondsPassed + secondsToBefore)/interval));
  // If the bandBegin is exactly the "current" time band, we don't want it
  // This data is forecast, if this is the current time, it's "old" data
  if(temp == bandBegin)
    ++bandBegin;

  // calculate how many bands have "passed" (time/band_interval)
  size_t bandsOperator = static_cast<size_t>(std::floor((secondsToAfter-secondsToBefore)/interval));
  // The first band is already included, remove one from last
  size_t bandEnd = bandBegin+bandsOperator-1;

  return std::make_pair(bandBegin, bandEnd);
}

std::string terrama2::services::analysis::core::operationAsString(terrama2::services::analysis::core::StatisticOperation op)
{
  switch(op)
  {
    case StatisticOperation::SUM:
      return "sum";
    case StatisticOperation::MEAN:
      return "avg";
    case StatisticOperation::MIN:
      return "min";
    case StatisticOperation::MAX:
      return "max";
    case StatisticOperation::COUNT:
      return "count";
    case StatisticOperation::VARIANCE:
      return "variance";
    default:
      return "";
  }
}
