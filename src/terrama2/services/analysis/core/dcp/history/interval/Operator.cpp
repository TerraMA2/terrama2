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


double terrama2::services::analysis::core::dcp::history::interval::sum(const std::string& attribute,
                                                                       const std::string& dateFilterBegin,
                                                                       const std::string& dateFilterEnd,
                                                                       boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation::SUM, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::history::interval::mean(const std::string& attribute,
                                                                        const std::string& dateFilterBegin,
                                                                        const std::string& dateFilterEnd,
                                                                        boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation::MEAN, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::history::interval::min(const std::string& attribute,
                                                                     const std::string& dateFilterBegin,
                                                                       const std::string& dateFilterEnd,
                                                                       boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation::MIN, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::history::interval::max(const std::string& attribute,
                                                                       const std::string& dateFilterBegin,
                                                                       const std::string& dateFilterEnd,
                                                                       boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation::MAX, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::history::interval::median(const std::string& attribute,
                                                                          const std::string& dateFilterBegin,
                                                                          const std::string& dateFilterEnd,
                                                                          boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation::MEDIAN, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::history::interval::standardDeviation(const std::string& attribute,
                                                                                     const std::string& dateFilterBegin,
                                                                                     const std::string& dateFilterEnd,
                                                                                     boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation::STANDARD_DEVIATION, attribute, ids, dateFilterBegin, dateFilterEnd);
}

double terrama2::services::analysis::core::dcp::history::interval::variance(const std::string& attribute,
                                                                            const std::string& dateFilterBegin,
                                                                            const std::string& dateFilterEnd,
                                                                            boost::python::list ids)
{
  return terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation::VARIANCE, attribute, ids, dateFilterBegin, dateFilterEnd);
}
