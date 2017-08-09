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
  \file terrama2/services/analysis/core/dcp/DCPOperator.hpp

  \brief Contains DCP analysis operators.

  \author Paulo R. M. Oliveira
*/



#include "Operator.hpp"
#include "../utility/Utils.hpp"
#include "../utility/Verify.hpp"
#include "../Exception.hpp"
#include "../ContextManager.hpp"
#include "../python/PythonUtils.hpp"
#include "../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../core/data-model/Filter.hpp"
#include "../../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../../core/Shared.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/GeoUtils.hpp"
#include "zonal/influence/Operator.hpp"

// QT
#include <QObject>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/UnitsOfMeasureManager.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>

#include <cmath>
#include <algorithm>
#include <boost/range/algorithm_ext/for_each.hpp>
#include <boost/range/join.hpp>

using namespace boost::python;

double terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation statisticOperation,
    const std::string& attribute,
    boost::python::list pcds,
    const std::string& dateFilterBegin,
    const std::string& dateFilterEnd)
{
  ///////////////////////////////////////////////////////////////
  // check analysis

  // A DCP attribute must be given
  if(attribute.empty())
    return std::nan("");

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::services::analysis::core::verify::analysisMonitoredObject(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return std::nan("");
  }

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
    // In case an error has already occurred, there is nothing to do.
    if(context->hasError())
      return std::nan("");
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return std::nan("");
  }

}

int terrama2::services::analysis::core::dcp::count(Buffer buffer)
{
  return static_cast<int>(zonal::influence::byRule(buffer).size());
}

double terrama2::services::analysis::core::dcp::min(const std::string& attribute, boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MIN, attribute, ids);
}

double terrama2::services::analysis::core::dcp::max(const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MAX, attribute, ids);
}

double terrama2::services::analysis::core::dcp::mean(const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MEAN, attribute, ids);
}

double terrama2::services::analysis::core::dcp::median(const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MEDIAN, attribute, ids);
}

double terrama2::services::analysis::core::dcp::sum(const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::SUM, attribute, ids);
}

double terrama2::services::analysis::core::dcp::standardDeviation(const std::string& dataSeriesName,
    const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, attribute, ids);
}

double terrama2::services::analysis::core::dcp::variance(const std::string& dataSeriesName,
    const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::VARIANCE, attribute, ids);
}
