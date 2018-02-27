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
  \file terrama2/services/analysis/core/BufferMemory.hpp

  \brief Utility function to create a buffer for a set of geometries.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "BufferMemory.hpp"
#include "utility/Utils.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/GeoUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>

std::shared_ptr<te::gm::Geometry> terrama2::services::analysis::core::createBuffer(Buffer buffer,
                                                                                   std::shared_ptr<te::gm::Geometry> geometry)
{
  if(buffer.bufferType == NONE)
  {
    return geometry;
  }

  std::shared_ptr<te::gm::Geometry> geomCopy;
  geomCopy.reset(dynamic_cast<te::gm::Geometry*>(geometry->clone()));

  bool needToTransform = false;

  // Converts the data to UTM if the geometry SRID is in decimal degrees
  int srid = geometry.get()->getSRID();

  if(srid != 0)
  {
    auto& spRefSysManager = te::srs::SpatialReferenceSystemManager::getInstance();
    std::string p4txt = spRefSysManager.getP4Txt(srid);

    // if there is no tag 'units', it's in decimal degrees, so we have to convert it
    if(p4txt.find("units") == std::string::npos)
    {
      needToTransform = true;
    }
  }
  else
  {
    needToTransform = true;
  }

  if(needToTransform)
  {
    geomCopy->transform(terrama2::core::getUTMSrid(geometry.get()));
  }

  std::shared_ptr<te::gm::Geometry> geomResult;
  std::shared_ptr<te::gm::Geometry> geomTemp;

  double distance = terrama2::core::convertDistanceUnit(buffer.distance, buffer.unit, "METER");


  switch(buffer.bufferType)
  {
    case IN:
    {
      geomTemp.reset(geomCopy->buffer(-distance, 16, te::gm::CapButtType));
      geomResult.reset(geomCopy->difference(geomTemp.get()));
      break;
    }
    case OUT:
    {
      geomTemp.reset(geomCopy->buffer(distance, 16, te::gm::CapButtType));
      geomResult.reset(geomTemp->difference(geomCopy.get()));
      break;
    }
    case IN_OUT:
    {
      double outDistance = terrama2::core::convertDistanceUnit(buffer.distance2, buffer.unit2, "METER");
      geomTemp.reset(geomCopy->buffer(outDistance, 16, te::gm::CapButtType));

      std::shared_ptr<te::gm::Geometry> auxGeom(geomCopy->buffer(-distance, 16, te::gm::CapButtType));
      auxGeom.reset(geomCopy->difference(auxGeom.get()));

      geomResult.reset(auxGeom->Union(geomTemp.get()));
      break;
    }
    case OUT_UNION:
    {
      if(buffer.distance < 0)
      {
        QString errMsg(QObject::tr(
                "The distance must be positive for the buffer type OUT_UNION, given value: %1.").arg(
                buffer.distance));
        throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
      }
      geomResult.reset(geomCopy->buffer(distance, 16, te::gm::CapButtType));
      break;
    }
    case IN_DIFF:
    {
      geomResult.reset(geomCopy->buffer(-distance, 16, te::gm::CapButtType));
      break;
    }
    case LEVEL:
    {
      geomTemp.reset(geomCopy->buffer(distance, 16, te::gm::CapButtType));

      double distance2 = terrama2::core::convertDistanceUnit(buffer.distance2, buffer.unit2, "METER");
      geomResult.reset(geomTemp->difference(geomCopy->buffer(distance2, 16, te::gm::CapButtType)));
      break;
    }
    default:
      break;
  }

  if(geomResult->getSRID() != srid)
  {
    geomResult->transform(srid);
  }

  return geomResult;
}


std::shared_ptr<te::mem::DataSet> terrama2::services::analysis::core::createAggregationBuffer(
        std::shared_ptr<ContextDataSeries> contextDataSeries, Buffer buffer,
        StatisticOperation aggregationStatisticOperation,
        const std::string& attribute)
{
  auto syncDs = contextDataSeries->series.syncDataSet;
  if(syncDs->size() == 0)
    return nullptr;

  ////////////////////////////////
  // Creates memory dataset for buffer
  te::da::DataSetType* dt = new te::da::DataSetType("buffer");

  auto sampleGeom = syncDs->getGeometry(0, contextDataSeries->geometryPos);
  int geomSampleSrid = sampleGeom->getSRID();

  te::gm::GeometryProperty* prop = new te::gm::GeometryProperty("geom", 0, te::gm::MultiPolygonType, true);
  prop->setSRID(geomSampleSrid);
  dt->add(prop);

  te::dt::SimpleProperty* prop02 = new te::dt::SimpleProperty("attribute", te::dt::DOUBLE_TYPE, true);
  dt->add(prop02);
  std::shared_ptr<te::mem::DataSet> dsOut= std::make_shared<te::mem::DataSet>(dt);
  ////////////////////////////////

  std::shared_ptr<te::gm::Envelope> box(syncDs->getExtent(contextDataSeries->geometryPos));

  if(buffer.unit.empty())
    buffer.unit = "m";

  // Inserts each geometry in the rtree, if there is a conflict, it makes the union of the two geometries
  std::vector<std::shared_ptr<OccurrenceAggregation>> occurrenceAggregationVec;
  te::sam::rtree::Index<size_t, 4> rtree;
  for(size_t i = 0; i < syncDs->size(); ++i)
  {
    auto geom = syncDs->getGeometry(i, contextDataSeries->geometryPos);
    double distance = terrama2::core::convertDistanceUnit(buffer.distance, buffer.unit, "METER");

    std::unique_ptr<te::gm::Geometry> tempGeom(dynamic_cast<te::gm::Geometry*>(geom.get()->clone()));
    if(!tempGeom)
    {
      QString errMsg(QObject::tr("Invalid geometry in dataset: ").arg(contextDataSeries->series.dataSet->id));
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }
    int utmSrid = terrama2::core::getUTMSrid(tempGeom.get());

    // Converts to UTM in order to create buffer in meters
    if(tempGeom->getSRID() != utmSrid)
      tempGeom->transform(utmSrid);

    std::unique_ptr<te::gm::Geometry> aggBuffer(tempGeom->buffer(distance, 16, te::gm::CapButtType));
    aggBuffer->setSRID(utmSrid);

    // Converts buffer to DataSet SRID in order to compare with the occurrences in the rtree
    aggBuffer->transform(geomSampleSrid);

    std::vector<size_t> vec;
    bool aggregated = false;

    // Search for occurrences in the same area
    rtree.search(*(aggBuffer->getMBR()), vec);
    for(std::size_t t = 0; t < vec.size(); ++t)
    {
      auto occurrenceIndex = vec.at(t);
      auto occurrenceAggregation = occurrenceAggregationVec.at(occurrenceIndex);

      // If the an intersection is found, makes the union of the two geometries and mark the index.
      if(aggBuffer->intersects(occurrenceAggregation->buffer.get()))
      {
        rtree.remove(*(occurrenceAggregation->buffer->getMBR()), occurrenceIndex);
        occurrenceAggregation->buffer.reset(aggBuffer->Union(occurrenceAggregation->buffer.get()));
        occurrenceAggregation->indexes.push_back(i);
        rtree.insert(*(occurrenceAggregation->buffer->getMBR()), occurrenceIndex);
        aggregated = true;
      }
    }

    if(!aggregated)
    {
      auto occurrenceAggregation = std::make_shared<OccurrenceAggregation>();
      occurrenceAggregation->buffer.reset(aggBuffer.release());
      occurrenceAggregation->indexes.push_back(i);
      occurrenceAggregationVec.push_back(occurrenceAggregation);
      rtree.insert(*(occurrenceAggregation->buffer->getMBR()), occurrenceAggregationVec.size()-1);
    }
  }

  std::vector<size_t> vec;
  rtree.search(*(box.get()), vec);

  int attributeType = -1;
  if(aggregationStatisticOperation != StatisticOperation::COUNT)
  {
    auto property = contextDataSeries->series.teDataSetType->getProperty(attribute);
    if(!property)
    {
      QString errMsg(QObject::tr("Invalid attribute: %1").arg(QString::fromStdString(attribute)));
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    attributeType = property->getType();
  }

  for(size_t i = 0; i < vec.size(); i++)
  {
    auto occurrenceIndex = vec.at(i);
    auto occurrenceAggregation = occurrenceAggregationVec.at(occurrenceIndex);

    OperatorCache cache;
    std::vector<double> values;
    size_t size = occurrenceAggregation->indexes.size();
    values.reserve(size);

    if(aggregationStatisticOperation != StatisticOperation::COUNT)
    {
      if(attribute.empty())
      {
        QString errMsg(QObject::tr("Invalid attribute"));
        throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
      }

      for(size_t j : occurrenceAggregation->indexes)
      {
        double value = getValue(syncDs, attribute, j, attributeType);
        values.push_back(value);
      }
    }

    calculateStatistics(values, cache);
    cache.count = size;

    auto item = new te::mem::DataSetItem(dsOut.get());
    item->setGeometry(0, dynamic_cast<te::gm::Geometry*>(occurrenceAggregation->buffer->clone()));
    item->setDouble(1, getOperationResult(cache, aggregationStatisticOperation));
    dsOut->add(item);
  }

  return dsOut;
}
