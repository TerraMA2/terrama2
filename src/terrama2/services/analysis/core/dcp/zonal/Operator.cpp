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
#include "../../utility/Utils.hpp"
#include "../../utility/Verify.hpp"
#include "../../Exception.hpp"
#include "../../ContextManager.hpp"
#include "../../python/PythonUtils.hpp"
#include "../../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../../core/data-model/Filter.hpp"
#include "../../../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../../../core/Shared.hpp"
#include "../../../../../core/utility/Logger.hpp"
#include "../../../../../core/utility/GeoUtils.hpp"
#include "influence/Operator.hpp"

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

double terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation statisticOperation,
    const std::string& dataSeriesName,
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

  std::vector< std::string > vecDCPAlias;
  terrama2::services::analysis::core::python::pythonToVector< std::string >(pcds, vecDCPAlias);
  if(vecDCPAlias.empty())
  {
    return std::nan("");
  }
  // end check analysis
  ///////////////////////////////////////////////////////////////

  // After the lock is released it's not allowed to return any value because it doesn't have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  try
  {
    bool hasData = false;

    ///////////////////////////////////////////////////////////////
    // get dataseries
    AnalysisPtr analysis = context->getAnalysis();

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }
    auto dataSeries = dataManagerPtr->findDataSeries(analysis->id, dataSeriesName);
    dataManagerPtr.reset();
    if(!dataSeries)
    {
      QString errMsg(QObject::tr("Could not find a data series with the given name: %1"));
      errMsg = errMsg.arg(QString::fromStdString(dataSeriesName));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }
    // end get dataseries
    ///////////////////////////////////////////////////////////////

    //results map
    std::vector<double> results;

    {
      // Frees the GIL, from now on it's not allowed to return any value because it doesn't have the interpreter lock.
      // In case an exception is thrown, we need to catch it and set a flag.
      // Once the code left the lock is acquired we should return NAN.
      terrama2::services::analysis::core::python::OperatorLock operatorLock; Q_UNUSED(operatorLock)

      try
      {
        terrama2::core::Filter filter;
        if(dateFilterBegin.empty() && dateFilterEnd.empty())
        {
          //dcp::zonal::operatorImpl
          filter.lastValues = std::make_shared<size_t>(1);
          filter.discardAfter = context->getStartTime();
        }
        else
        {
          //dcp::zonal::history::operatorImpl
          filter.discardBefore = context->getTimeFromString(dateFilterBegin);
          filter.discardAfter = context->getTimeFromString(dateFilterEnd);
        }

        context->addDCPDataSeries(dataSeries, filter);
        // For DCP operator count returns the number of DCP that influence the monitored object
        uint32_t influenceCount = 0;
        for(auto& dcpAlias : vecDCPAlias)
        {
          for(const auto& dataset : dataSeries->datasetList)
          {
            auto dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
            if(!dcpDataset)
            {
              QString errMsg(QObject::tr("Invalid dataset for data series: ").arg(dataSeriesName.c_str()));
              throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
            }

            ///////////////////////////////////////////////////////////////
            // check dataset
            try
            {
              if(dcpDataset->alias() != dcpAlias)
                continue;
            }
            catch(const std::out_of_range&)
            {
              QString errMsg(QObject::tr("DCP dataset does not have an alias."));
              throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
            }

            if(dcpDataset->position == nullptr)
            {
              QString errMsg(QObject::tr("DCP dataset does not have a valid position."));
              throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
            }

            std::shared_ptr<ContextDataSeries> contextDataSeries = context->getContextDataset(dcpDataset->id, filter);
            if(!contextDataSeries)
              continue;
            auto syncDs = contextDataSeries->series.syncDataSet;
            if(syncDs->size() == 0)
              continue;

            auto property = contextDataSeries->series.teDataSetType->getProperty(attribute);
           // only operation COUNT can be done without attribute.
            if(!property && statisticOperation != StatisticOperation::COUNT)
              continue;

            // end check dataset
            ///////////////////////////////////////////////////////////////

            ++influenceCount;
            std::vector<double> values;
            int attributeType = property->getType();
            for(unsigned int i = 0; i < syncDs->size(); ++i)
            {
              try
              {
                if(syncDs->isNull(i, attribute))
                  continue;

                hasData = true;
                double value = getValue(syncDs, attribute, i, attributeType);
                if(std::isnan(value))
                  continue;
                values.push_back(value);
              }
              catch(...)
              {
                // In case the DCP doesn't have the specified column
                continue;
              }
            }//end for syncDs

            results.insert(results.end(), values.begin(), values.end());
          }//end for each dataSeries->datasetList
        }//end for each vecDCPAlias
      }
      catch(const terrama2::Exception& e)
      {
        context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
        exceptionOccurred = true;
      }
      catch(const std::exception& e)
      {
        context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
        exceptionOccurred = true;
      }
      catch(...)
      {
        QString errMsg = QObject::tr("An unknown exception occurred.");
        context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
        exceptionOccurred = true;
      }
    }// Destroy the OperatorLock object and acquires the lock

    if(exceptionOccurred)
      return std::nan("");

    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return std::nan("");
    }

    calculateStatistics(results, cache);
    // return value of the operation
    return getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return std::nan("");
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    return std::nan("");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return std::nan("");
  }
}

int terrama2::services::analysis::core::dcp::zonal::count(const std::string& dataSeriesName, Buffer buffer)
{
  return static_cast<int>(terrama2::services::analysis::core::dcp::zonal::influence::byRule(dataSeriesName, buffer).size());
}

double terrama2::services::analysis::core::dcp::zonal::min(const std::string& dataSeriesName, const std::string& attribute, boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, attribute, ids);
}

double terrama2::services::analysis::core::dcp::zonal::max(const std::string& dataSeriesName, const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, attribute, ids);
}

double terrama2::services::analysis::core::dcp::zonal::mean(const std::string& dataSeriesName, const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, attribute, ids);
}

double terrama2::services::analysis::core::dcp::zonal::median(const std::string& dataSeriesName, const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, attribute, ids);
}

double terrama2::services::analysis::core::dcp::zonal::sum(const std::string& dataSeriesName, const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, attribute, ids);
}

double terrama2::services::analysis::core::dcp::zonal::standardDeviation(const std::string& dataSeriesName,
    const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, attribute, ids);
}

double terrama2::services::analysis::core::dcp::zonal::variance(const std::string& dataSeriesName,
    const std::string& attribute,
    boost::python::list ids)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, attribute, ids);
}

terrama2::services::analysis::core::InfluenceType terrama2::services::analysis::core::dcp::zonal::getInfluenceType(
  AnalysisPtr analysis)
{
  // Reads influence type
  auto it = analysis->metadata.find("INFLUENCE_TYPE");

  if(it == analysis->metadata.cend())
    return InfluenceType::RADIUS_TOUCHES;

  std::string typeStr = it->second;
  int type = std::atoi(typeStr.c_str());
  if(type == 0 || type > 3)
  {
    QString errMsg(QObject::tr("Invalid influence type for DCP analysis."));
    errMsg = errMsg.arg(analysis->id);
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }
  InfluenceType influenceType = static_cast<InfluenceType>(type);
  return influenceType;
}


std::shared_ptr<te::gm::Geometry> terrama2::services::analysis::core::dcp::zonal::createDCPInfluenceBuffer(
  AnalysisPtr analysis,
  std::shared_ptr<te::gm::Geometry> position,
  int monitoredObjectSrid,
  InfluenceType influenceType)
{
  std::shared_ptr<te::gm::Geometry> buffer;

  // For influence based on radius, creates a buffer for the DCP location
  switch(influenceType)
  {
    case InfluenceType::RADIUS_CENTER:
    case InfluenceType::RADIUS_TOUCHES:
    {
      // Reads influence type
      auto itRad = analysis->metadata.find("INFLUENCE_RADIUS");
      auto itUnit = analysis->metadata.find("INFLUENCE_RADIUS_UNIT");

      std::string radiusStr = itRad == analysis->metadata.cend() ? "0" : itRad->second;
      std::string radiusUnit = itUnit == analysis->metadata.cend() ? "km" : itUnit->second;

      std::shared_ptr<te::gm::Geometry> geomPosition(dynamic_cast<te::gm::Geometry*>(position->clone()));

      double influenceRadius = std::atof(radiusStr.c_str());
      if(influenceRadius <= 0)
        return geomPosition;

      influenceRadius = te::common::UnitsOfMeasureManager::getInstance().getConversion(radiusUnit, "METER") * influenceRadius;
      auto spatialReferenceSystem = te::srs::SpatialReferenceSystemManager::getInstance().getSpatialReferenceSystem(geomPosition->getSRID());
      std::string unitName = spatialReferenceSystem->getUnitName();

      if(unitName == "degree")
      {
        int srid = terrama2::core::getUTMSrid(position.get());
        geomPosition->transform(srid);
      }

      buffer.reset(geomPosition->buffer(influenceRadius, 16, te::gm::CapButtType));

      buffer->setSRID(geomPosition->getSRID());

      // Converts the buffer to monitored object SRID
      buffer->transform(monitoredObjectSrid);

      break;
    }
    case InfluenceType::REGION:
    {
      // TODO: Ticket #482
      QString errMsg = QObject::tr("NOT IMPLEMENTED YET.");
      throw Exception() << ErrorDescription(errMsg);
    }
  }
  return buffer;
}


bool terrama2::services::analysis::core::dcp::zonal::verifyDCPInfluence(InfluenceType influenceType,
    std::shared_ptr<te::gm::Geometry> geom,
    std::shared_ptr<te::gm::Geometry> dcpInfluenceBuffer)
{
  bool intersects = false;

  if(influenceType == InfluenceType::RADIUS_TOUCHES)
  {
    intersects = geom->intersects(dcpInfluenceBuffer.get());
  }
  else if(influenceType == InfluenceType::RADIUS_CENTER)
  {
    //TODO: use method from terralib_mod_sa_core :: Check for terrama2::core::Utils methods before!
    std::string geomType = geom->getGeometryType();
    if(geomType == "MultiPolygon")
    {
      auto polygon = dynamic_cast<te::gm::MultiPolygon*>(geom.get());
      if(polygon)
      {
        auto centroid = polygon->getCentroid();
        intersects = centroid->within(dcpInfluenceBuffer.get());
      }
    }
    else
    {
      intersects = geom->intersects(dcpInfluenceBuffer.get());
    }
  }
  else if(influenceType == InfluenceType::REGION)
  {
    // TODO: Ticket #482
    QString errMsg = QObject::tr("NOT IMPLEMENTED YET.");
    throw Exception() << ErrorDescription(errMsg);
  }
  return intersects;

}
