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
  \file terrama2/services/analysis/core/dcp/history/interval/Operator.cpp

  \brief Contains DCP history interval analysis operators.

  \author Paulo R. M. Oliveira
*/

#include "Operator.hpp"
#include "../../Operator.hpp"


double terrama2::services::analysis::core::dcp::zonal::history::interval::sum(const std::string& dataSeriesName,
                                                                       const std::string& attribute,
                                                                       const std::string& dateFilterBegin,
                                                                       const std::string& dateFilterEnd,
                                                                       boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation::SUM, dataSeriesName, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::zonal::history::interval::mean(const std::string& dataSeriesName,
                                                                        const std::string& attribute,
                                                                        const std::string& dateFilterBegin,
                                                                        const std::string& dateFilterEnd,
                                                                        boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation::MEAN, dataSeriesName, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::zonal::history::interval::min(const std::string& dataSeriesName,
                                                                       const std::string& attribute,
                                                                     const std::string& dateFilterBegin,
                                                                       const std::string& dateFilterEnd,
                                                                       boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation::MIN, dataSeriesName, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::zonal::history::interval::max(const std::string& dataSeriesName,
                                                                       const std::string& attribute,
                                                                       const std::string& dateFilterBegin,
                                                                       const std::string& dateFilterEnd,
                                                                       boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation::MAX, dataSeriesName, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::zonal::history::interval::median(const std::string& dataSeriesName,
                                                                          const std::string& attribute,
                                                                          const std::string& dateFilterBegin,
                                                                          const std::string& dateFilterEnd,
                                                                          boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::zonal::history::interval::standardDeviation(const std::string& dataSeriesName,
                                                                                     const std::string& attribute,
                                                                                     const std::string& dateFilterBegin,
                                                                                     const std::string& dateFilterEnd,
                                                                                     boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::zonal::history::interval::variance(const std::string& dataSeriesName,
                                                                            const std::string& attribute,
                                                                            const std::string& dateFilterBegin,
                                                                            const std::string& dateFilterEnd,
                                                                            boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, attribute, ids, dateFilterBegin, dateFilterEnd);
}
