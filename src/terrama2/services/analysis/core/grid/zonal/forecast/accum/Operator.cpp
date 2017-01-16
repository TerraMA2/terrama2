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
  \file terrama2/services/analysis/core/grid/zonal/Operator.cpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/



#include "Operator.hpp"
#include "../../Operator.hpp"
#include "../../Utils.hpp"
#include "../../../../utility/Utils.hpp"
#include "../../../../utility/Verify.hpp"
#include "../../../../python/PythonInterpreter.hpp"
#include "../../../../ContextManager.hpp"
#include "../../../../MonitoredObjectContext.hpp"

#include "../../../../../../../core/data-model/Filter.hpp"
#include "../../../../../../../core/utility/TimeUtils.hpp"
#include "../../../../../../../core/utility/Logger.hpp"
#include "../Operator.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/geometry/Utils.h>
#include <terralib/raster/PositionIterator.h>

double terrama2::services::analysis::core::grid::zonal::forecast::accum::getAbsTimeFromString(const std::string& timeStr)
{
  auto begin = timeStr.find_first_of("0123456789");
  auto end = timeStr.find_last_of("0123456789");

  if(begin == std::string::npos || end == std::string::npos)
    return NAN;

  auto time = timeStr.substr(begin, end-begin);
  return std::stod(time);
}

double terrama2::services::analysis::core::grid::zonal::forecast::accum::count(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::COUNT, dataSeriesName, "", dateFilter, buffer);
}


double terrama2::services::analysis::core::grid::zonal::forecast::accum::min(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::MIN, dataSeriesName, "", dateFilter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::accum::max(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::MAX, dataSeriesName, "", dateFilter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::accum::mean(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::MEAN, dataSeriesName, "", dateFilter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::accum::median(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, "", dateFilter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::accum::standardDeviation(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, "", dateFilter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::accum::variance(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, "", dateFilter, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::accum::sum(const std::string& dataSeriesName, const std::string& dateFilter, terrama2::services::analysis::core::Buffer buffer)
{
  return zonal::forecast::operatorImpl(StatisticOperation::SUM, dataSeriesName, "", dateFilter, buffer);
}
