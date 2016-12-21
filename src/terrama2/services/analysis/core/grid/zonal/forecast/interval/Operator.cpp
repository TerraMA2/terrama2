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
  \file terrama2/services/analysis/core/grid/zonal/forecast/Operator.cpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/

#include "Operator.hpp"
#include "../Operator.hpp"

double terrama2::services::analysis::core::grid::zonal::forecast::interval::min(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MIN, dataSeriesName, dateDiscardBefore, dateDiscardAfter, var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::interval::max(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MAX, dataSeriesName, dateDiscardBefore, dateDiscardAfter, var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::interval::mean(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MEAN, dataSeriesName, dateDiscardBefore, dateDiscardAfter, var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::interval::median(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, dateDiscardBefore, dateDiscardAfter, var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::interval::standardDeviation(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, dateDiscardBefore, dateDiscardAfter, var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::interval::variance(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, dateDiscardBefore, dateDiscardAfter, var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::interval::sum(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const std::string& dateDiscardAfter, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::SUM, dataSeriesName, dateDiscardBefore, dateDiscardAfter, var, buffer);
}
