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
#include "../Exception.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../core/data-model/Filter.hpp"
#include "../../../../core/data-access/SyncronizedDataSet.hpp"
#include "../../../../core/Shared.hpp"

// QT
#include <QObject>



// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/UnitsOfMeasureManager.h>

#include <math.h>

using namespace boost::python;


double terrama2::services::analysis::core::dcp::operatorImpl(StatisticOperation statisticOperation,
                                                             const std::string& dataSeriesName,
                                                             const std::string& attribute,
                                                             Buffer buffer, boost::python::list ids)
{
  OperatorCache cache;
  readInfoFromDict(cache);

  bool found = false;
  bool hasData = false;

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    return NAN;
  }

  Analysis analysis = Context::getInstance().getAnalysis(cache.analysisId);

  auto moDsContext = getMonitoredObjectContextDataset(analysis, dataManagerPtr);
  if(!moDsContext)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }


  auto geom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }


  std::shared_ptr<ContextDataSeries> contextDataSeries;



  // Frees the GIL, from now on can not use the interpreter
  Py_BEGIN_ALLOW_THREADS

    for(auto analysisDataSeries : analysis.analysisDataSeriesList)
    {
      terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

      if(dataSeries->name == dataSeriesName)
      {
        found = true;

        if(dataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesSemantics::DCP)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
          errMsg = errMsg.arg(cache.analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }


        Context::getInstance().addDCPDataSeries(cache.analysisId, dataSeries, "", true);

        // For DCP operator count returns the number of DCP that influence the monitored object
        uint64_t influenceCount = 0;

        for(auto dataset : dataSeries->datasetList)
        {
          contextDataSeries = Context::getInstance().getContextDataset(cache.analysisId, dataset->id);

          terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(
                  dataset);
          if(!dcpDataset)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> Could not recover DCP dataset."));
            errMsg = errMsg.arg(cache.analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NAN;
          }


          if(dcpDataset->position == nullptr)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
            errMsg = errMsg.arg(cache.analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NAN;
          }


          try
          {
            auto influenceType = getInfluenceType(analysis);

            auto dcpInfluenceBuffer = createDCPInfluenceBuffer(analysis, dcpDataset->position, geom->getSRID(),
                                                               influenceType);

            bool intersects = verifyDCPInfluence(influenceType, geom, dcpInfluenceBuffer);

            if(intersects)
            {
              ++influenceCount;

              auto syncDs = contextDataSeries->series.syncDataSet;

              int attributeType = 0;
              if(!attribute.empty())
              {
                auto property = contextDataSeries->series.teDataSetType->getProperty(attribute);

                // only operation COUNT can be done without attribute.
                if(!property && statisticOperation != COUNT)
                {
                  QString errMsg(QObject::tr("Analysis: %1 -> Invalid attribute name"));
                  errMsg = errMsg.arg(cache.analysisId);
                  TERRAMA2_LOG_ERROR() << errMsg;
                  return NAN;
                }
                attributeType = property->getType();
              }

              uint64_t countValues = 0;

              if(syncDs->size() == 0)
                continue;

              std::vector<double> values;
              for(unsigned int i = 0; i < syncDs->size(); ++i)
              {
                try
                {
                  if(!attribute.empty() && !syncDs->isNull(i, attribute))
                  {
                    hasData = true;
                    countValues++;
                    double value = getValue(syncDs, attribute, i, attributeType);
                    values.push_back(value);
                    cache.sum += value;
                    if(value > cache.max)
                      cache.max = value;
                    if(value < cache.min)
                      cache.min = value;
                  }
                }
                catch(...)
                {
                  // In case the DCP doesn't have the specified column
                  continue;
                }
              }

              if(countValues == 0)
                continue;

              // Statitics are calculated based on the number of values
              // but the operator count for DCP returns the number of DCPs that influence the monitored object

              cache.count = countValues;

              calculateStatistics(values, cache);
            }
          }
          catch(std::exception& e)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> %2").arg(e.what()));
            errMsg = errMsg.arg(cache.analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NAN;
          }


        }


        // Set the number of DCPs that influence the monitored object
        cache.count = influenceCount;
        break;
      }
    }



    // All operations are done, acquires the GIL and set the return value
  Py_END_ALLOW_THREADS

  if(!found)
    return NAN;

  if(!hasData && statisticOperation != COUNT)
  {
    return NAN;
  }

  return getOperationResult(cache, statisticOperation);
}

int terrama2::services::analysis::core::dcp::count(const std::string& dataSeriesName, Buffer buffer)
{
  return (int) operatorImpl(COUNT, dataSeriesName, "", buffer);
}

double terrama2::services::analysis::core::dcp::min(const std::string& dataSeriesName, const std::string& attribute,
                                                    Buffer buffer, boost::python::list ids)
{
  return operatorImpl(MIN, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcp::max(const std::string& dataSeriesName, const std::string& attribute,
                                                    Buffer buffer, boost::python::list ids)
{
  return operatorImpl(MAX, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcp::mean(const std::string& dataSeriesName, const std::string& attribute,
                                                     Buffer buffer, boost::python::list ids)
{
  return operatorImpl(MEAN, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcp::median(const std::string& dataSeriesName, const std::string& attribute,
                                                       Buffer buffer, boost::python::list ids)
{
  return operatorImpl(MEDIAN, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcp::sum(const std::string& dataSeriesName, const std::string& attribute,
                                                    Buffer buffer, boost::python::list ids)
{
  return operatorImpl(SUM, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcp::standardDeviation(const std::string& dataSeriesName,
                                                                  const std::string& attribute, Buffer buffer,
                                                                  boost::python::list ids)
{
  return operatorImpl(STANDARD_DEVIATION, dataSeriesName, attribute, buffer, ids);
}


terrama2::services::analysis::core::InfluenceType terrama2::services::analysis::core::dcp::getInfluenceType(
        const Analysis& analysis)
{
  // Reads influence type
  std::string typeStr = analysis.metadata.at("INFLUENCE_TYPE");
  int type = std::atoi(typeStr.c_str());
  if(type == 0 || type > 3)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence type for DCP analysis."));
    errMsg = errMsg.arg(analysis.id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }
  InfluenceType influenceType = (InfluenceType) type;
  return influenceType;
}


std::shared_ptr<te::gm::Geometry> terrama2::services::analysis::core::dcp::createDCPInfluenceBuffer(
        const Analysis& analysis,
        std::shared_ptr<te::gm::Geometry> position,
        int monitoredObjectSrid,
        InfluenceType influenceType)
{
  std::shared_ptr<te::gm::Geometry> buffer;

  // For influence based on radius, creates a buffer for the DCP location
  if(influenceType == RADIUS_CENTER || influenceType == RADIUS_TOUCHES)
  {

    if(analysis.metadata.at("INFLUENCE_RADIUS").empty())
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence radius."));
      errMsg = errMsg.arg(analysis.id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }


    std::string radiusStr = analysis.metadata.at("INFLUENCE_RADIUS");
    std::string radiusUnit = analysis.metadata.at("INFLUENCE_RADIUS_UNIT");

    if(radiusStr.empty())
      radiusStr = "0";
    if(radiusUnit.empty())
      radiusUnit = "km";

    double influenceRadius = std::atof(radiusStr.c_str());

    influenceRadius =
            te::common::UnitsOfMeasureManager::getInstance().getConversion(radiusUnit, "METER") * influenceRadius;

    buffer.reset(position->buffer(influenceRadius, 16, te::gm::CapButtType));

    int srid = position->getSRID();
    buffer->setSRID(srid);

    // Converts the buffer to monitored object SRID
    buffer->transform(monitoredObjectSrid);
  }
  return buffer;
}


bool terrama2::services::analysis::core::dcp::verifyDCPInfluence(InfluenceType influenceType,
                                                                 std::shared_ptr<te::gm::Geometry> geom,
                                                                 std::shared_ptr<te::gm::Geometry> dcpInfluenceBuffer)
{
  bool intersects = false;
  if(influenceType == RADIUS_TOUCHES)
  {
    intersects = geom->intersects(dcpInfluenceBuffer.get());
  }
  else if(influenceType == RADIUS_CENTER)
  {
    //TODO: use method from terralib_mod_sa_core
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

  return intersects;

}










