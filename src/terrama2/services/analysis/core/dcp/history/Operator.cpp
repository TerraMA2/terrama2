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
  \file terrama2/services/analysis/core/PythonInterpreter.cpp

  \brief Manages the communication of Python and C.

  \author Paulo R. M. Oliveira
*/



#include "Operator.hpp"
#include "../Operator.hpp"
#include "../../utility/Utils.hpp"
#include "../../utility/Verify.hpp"
#include "../../ContextManager.hpp"


#include "../../Exception.hpp"
#include "../../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../../core/data-model/Filter.hpp"
#include "../../../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../../../core/Shared.hpp"
#include "../../../../../core/utility/Logger.hpp"
#include "../../python/PythonUtils.hpp"


// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>

double terrama2::services::analysis::core::dcp::history::sum(const std::string& attribute,
    const std::string& dateFilter,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::SUM, attribute, ids, dateFilter, "0s");
}

double terrama2::services::analysis::core::dcp::history::mean(const std::string& attribute,
    const std::string& dateFilter,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MEAN, attribute, ids, dateFilter, "0s");
}

double terrama2::services::analysis::core::dcp::history::min(const std::string& attribute,
   const std::string& dateFilter,
   boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MIN, attribute, ids, dateFilter, "0s");
}

double terrama2::services::analysis::core::dcp::history::max(const std::string& attribute,
   const std::string& dateFilter,
   boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MAX, attribute, ids, dateFilter, "0s");
}

double terrama2::services::analysis::core::dcp::history::median(const std::string& attribute,
    const std::string& dateFilter,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MEDIAN, attribute, ids, dateFilter, "0s");
}

double terrama2::services::analysis::core::dcp::history::standardDeviation(const std::string& attribute,
   const std::string& dateFilter,
   boost::python::list ids)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, attribute, ids, dateFilter, "0s");
}

double terrama2::services::analysis::core::dcp::history::variance(const std::string& attribute,
    const std::string& dateFilter,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::VARIANCE, attribute, ids, dateFilter, "0s");
}
