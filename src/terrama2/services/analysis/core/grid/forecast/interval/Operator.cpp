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
  \file terrama2/services/analysis/core/grid/Operator.cpp

  \brief Contains grid analysis operators.

  \author Paulo R. M. Oliveira
  \author Jano Simas
*/

// TerraMA2
#include "Operator.hpp"
#include "../Operator.hpp"

double terrama2::services::analysis::core::grid::forecast::interval::min(const std::string& dataSeriesName, const std::string& dateFilterBegin, const std::string& dateFilterEnd, const size_t var)
{
  return terrama2::services::analysis::core::grid::forecast::operatorImpl(StatisticOperation::MIN, dataSeriesName, dateFilterBegin, dateFilterEnd, var);
}

double terrama2::services::analysis::core::grid::forecast::interval::max(const std::string& dataSeriesName, const std::string& dateFilterBegin, const std::string& dateFilterEnd, const size_t var)
{
  return terrama2::services::analysis::core::grid::forecast::operatorImpl(StatisticOperation::MAX, dataSeriesName, dateFilterBegin, dateFilterEnd, var);
}

double terrama2::services::analysis::core::grid::forecast::interval::mean(const std::string& dataSeriesName, const std::string& dateFilterBegin, const std::string& dateFilterEnd, const size_t var)
{
  return terrama2::services::analysis::core::grid::forecast::operatorImpl(StatisticOperation::MEAN, dataSeriesName, dateFilterBegin, dateFilterEnd, var);
}

double terrama2::services::analysis::core::grid::forecast::interval::median(const std::string& dataSeriesName, const std::string& dateFilterBegin, const std::string& dateFilterEnd, const size_t var)
{
  return terrama2::services::analysis::core::grid::forecast::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, dateFilterBegin, dateFilterEnd, var);
}

double terrama2::services::analysis::core::grid::forecast::interval::standardDeviation(const std::string& dataSeriesName, const std::string& dateFilterBegin, const std::string& dateFilterEnd, const size_t var)
{
  return terrama2::services::analysis::core::grid::forecast::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, dateFilterBegin, dateFilterEnd, var);
}

double terrama2::services::analysis::core::grid::forecast::interval::variance(const std::string& dataSeriesName, const std::string& dateFilterBegin, const std::string& dateFilterEnd, const size_t var)
{
  return terrama2::services::analysis::core::grid::forecast::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, dateFilterBegin, dateFilterEnd, var);
}

double terrama2::services::analysis::core::grid::forecast::interval::sum(const std::string& dataSeriesName, const std::string& dateFilterBegin, const std::string& dateFilterEnd, const size_t var)
{
  return terrama2::services::analysis::core::grid::forecast::operatorImpl(StatisticOperation::SUM, dataSeriesName, dateFilterBegin, dateFilterEnd, var);
}
