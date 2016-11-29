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
#include "influence/Operator.hpp"

// QT
#include <QObject>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/UnitsOfMeasureManager.h>

#include <math.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>


using namespace boost::python;


double terrama2::services::analysis::core::dcp::zonal::operatorImpl(StatisticOperation statisticOperation,
    const std::string& dataSeriesName,
    const std::string& attribute,
    boost::python::list ids)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict (cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::core::verify::analysisMonitoredObject(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return {};
  }

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return std::nan("");
  }



  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  try
  {
    // In case an error has already occurred, there is nothing to do.
    if(context->hasError())
      return std::nan("");

    std::vector<DataSetId> vecDCPIds;
    terrama2::services::analysis::core::python::pythonToVector<DataSetId>(ids, vecDCPIds);

    if(vecDCPIds.empty())
    {
      return std::nan("");
    }

    bool hasData = false;

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    AnalysisPtr analysis = context->getAnalysis();

    auto moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }


    auto geom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
    if(!geom.get())
    {
      QString errMsg(QObject::tr("Could not recover monitored object geometry."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    std::shared_ptr<ContextDataSeries> dcpContextDataSeries;

    // Frees the GIL, from now on it's not allowed to return any value because it doesn't have the interpreter lock.
    // In case an exception is thrown, we need to catch it and set a flag.
    // Once the code left the lock is acquired we should return NAN.

    {
      terrama2::services::analysis::core::python::OperatorLock operatorLock;

      try
      {

        auto dataSeries = dataManagerPtr->findDataSeries(analysis->id, dataSeriesName);

        if(!dataSeries)
        {
          QString errMsg(QObject::tr("Could not find a data series with the given name: %1"));
          errMsg = errMsg.arg(QString::fromStdString(dataSeriesName));
          throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
        }

        context->addDCPDataSeries(dataSeries, "", "", true);

        // For DCP operator count returns the number of DCP that influence the monitored object
        uint32_t influenceCount = 0;


        for(DataSetId dcpId : vecDCPIds)
        {
          bool found = false;
          for(auto dataset : dataSeries->datasetList)
          {
            if(dataset->id == dcpId)
            {
              found = true;

              // recover dataset from context
              dcpContextDataSeries = context->getContextDataset(dataset->id);

              ++influenceCount;

              auto dcpSyncDs = dcpContextDataSeries->series.syncDataSet;

              int attributeType = 0;
              if(!attribute.empty())
              {
                auto property = dcpContextDataSeries->series.teDataSetType->getProperty(attribute);

                // only operation COUNT can be done without attribute.
                if(!property && statisticOperation != StatisticOperation::COUNT)
                {
                  QString errMsg(QObject::tr("Invalid attribute name"));
                  throw InvalidParameterException() << terrama2::ErrorDescription(errMsg);
                }
                attributeType = property->getType();
              }


              if(dcpSyncDs->size() == 0)
              {
                continue;
              }

              // Allocates the space for the size of the dataset
              std::vector<double> values;
              values.reserve(dcpSyncDs->size());

              for(unsigned int i = 0; i < dcpSyncDs->size(); ++i)
              {
                try
                {
                  if(!attribute.empty() && !dcpSyncDs->isNull(i, attribute))
                  {
                    hasData = true;
                    double value = getValue(dcpSyncDs, attribute, i, attributeType);
                    if(std::isnan(value))
                      continue;

                    values.push_back(value);
                  }
                }
                catch(...)
                {
                  // In case the DCP doesn't have the specified column
                  continue;
                }
              }

              if(values.empty())
                continue;

              // Statistics are calculated based on the number of values
              // but the operator count for DCP returns the number of DCPs that influence the monitored object

              cache.count = values.size();

              calculateStatistics(values, cache);
            }
          }

          if(!found)
          {
            QString errMsg(QObject::tr("Invalid DCP identifier"));
            throw InvalidParameterException() << terrama2::ErrorDescription(errMsg);
          }
        }

        // Set the number of DCPs that influence the monitored object
        cache.count = influenceCount;

      }
      catch(const terrama2::Exception& e)
      {
        context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
        exceptionOccurred = true;
      }
      catch(const std::exception& e)
      {
        context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
        exceptionOccurred = true;
      }
      catch(...)
      {
        QString errMsg = QObject::tr("An unknown exception occurred.");
        context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
        exceptionOccurred = true;
      }

    // Destroy the OperatorLock object and acquires the lock
    }
    if(exceptionOccurred)
      return std::nan("");

    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return std::nan("");
    }

    return getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return std::nan("");
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    return std::nan("");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return std::nan("");
  }
}

int terrama2::services::analysis::core::dcp::zonal::count(const std::string& dataSeriesName, Buffer buffer)
{
  return (int)terrama2::services::analysis::core::dcp::zonal::influence::byRule(dataSeriesName, buffer).size();
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
  std::string typeStr = analysis->metadata.at("INFLUENCE_TYPE");
  int type = std::atoi(typeStr.c_str());
  if(type == 0 || type > 3)
  {
    QString errMsg(QObject::tr("Invalid influence type for DCP analysis."));
    errMsg = errMsg.arg(analysis->id);
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }
  InfluenceType influenceType = (InfluenceType) type;
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

      if(analysis->metadata.at("INFLUENCE_RADIUS").empty())
      {
        QString errMsg(QObject::tr("Invalid influence radius."));
        errMsg = errMsg.arg(analysis->id);
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      std::string radiusStr = analysis->metadata.at("INFLUENCE_RADIUS");
      std::string radiusUnit = analysis->metadata.at("INFLUENCE_RADIUS_UNIT");

      if(radiusStr.empty())
        radiusStr = "0";
      if(radiusUnit.empty())
        radiusUnit = "km";

      double influenceRadius = std::atof(radiusStr.c_str());

      influenceRadius =
              te::common::UnitsOfMeasureManager::getInstance().getConversion(radiusUnit, "METER") * influenceRadius;

      std::shared_ptr<te::gm::Geometry> geomPosition(dynamic_cast<te::gm::Geometry*>(position->clone()));

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
