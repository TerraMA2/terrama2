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
  \file terrama2/services/analysis/core/occurrence/zonal/interval/Operator.cpp

  \brief Contains occurrence analysis operators by time interval.

  \author Paulo R. M. Oliveira
*/


// TerraMA2
#include "Operator.hpp"
#include "../Operator.hpp"

int terrama2::services::analysis::core::occurrence::zonal::interval::count(const std::string& dataSeriesName,
    const std::string& dateFilterBegin, const std::string& dateFilterEnd, Buffer buffer, const std::string& restriction)
{
  return (int) operatorImpl(StatisticOperation::COUNT, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), "",
                            StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::zonal::interval::min(const std::string& dataSeriesName,
                                                           const std::string& attribute,
                                                           const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                                                           Buffer buffer,
                                                           const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::zonal::interval::max(const std::string& dataSeriesName,
    const std::string& attribute,
    const std::string& dateFilterBegin, const std::string& dateFilterEnd, Buffer buffer, const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::zonal::interval::mean(const std::string& dataSeriesName,
                                                            const std::string& attribute,
                                                            const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                                                            Buffer buffer,
                                                            const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::zonal::interval::median(const std::string& dataSeriesName,
                                                              const std::string& attribute,
                                                              const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                                                              Buffer buffer,
                                                              const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::zonal::interval::standardDeviation(const std::string& dataSeriesName,
                                                                         const std::string& attribute,
                                                                         const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                                                                         Buffer buffer,
                                                                         const std::string& restriction)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::zonal::interval::variance(const std::string& dataSeriesName,
                                                                const std::string& attribute,
                                                                const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                                                                Buffer buffer,
                                                                const std::string& restriction)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::zonal::interval::sum(const std::string& dataSeriesName,
                                                           const std::string& attribute,
                                                           const std::string& dateFilterBegin, const std::string& dateFilterEnd,
                                                           Buffer buffer,
                                                           const std::string& restriction)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, buffer, dateFilterBegin,  dateFilterEnd, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}
