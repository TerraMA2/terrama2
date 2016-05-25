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
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/Logger.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/geometry/GeometryProperty.h>

//QT
#include <QObject>
#include <QString>

std::shared_ptr<te::gm::Geometry> terrama2::services::analysis::core::createBuffer(Buffer buffer,
                                                                                   std::shared_ptr<te::gm::Geometry> geometry)
{
  // Converts the data to UTM
  int utmSrid = terrama2::core::getUTMSrid(geometry.get());
  geometry->transform(utmSrid);

  if(buffer.bufferType == NONE)
  {
    return geometry;
  }

  std::shared_ptr<te::gm::Geometry> geomResult;
  std::shared_ptr<te::gm::Geometry> geomTemp;

  double distance = terrama2::core::convertDistanceUnit(buffer.distance, buffer.unit, "METER");


  switch(buffer.bufferType)
  {
    case ONLY_BUFFER:
    {
      geomTemp.reset(geometry->buffer(distance, 16, te::gm::CapButtType));
      if(distance > 0)
        geomResult.reset(geomTemp->difference(geometry.get()));
      else
        geomResult.reset(geometry->difference(geomTemp.get()));
      break;
    }
    case OUTSIDE_PLUS_INSIDE:
    {
      geomTemp.reset(geometry->buffer(distance, 16, te::gm::CapButtType));

      double distance2 = terrama2::core::convertDistanceUnit(buffer.distance2, buffer.unit2, "METER");
      std::shared_ptr<te::gm::Geometry> auxGeom(geometry->buffer(distance2, 16, te::gm::CapButtType));
      geomResult.reset(geomTemp->difference(auxGeom.get()));
      break;
    }
    case OBJECT_PLUS_BUFFER:
    {
      if(buffer.distance < 0)
      {
        QString errMsg(QObject::tr(
                "The distance must be positive for the buffer type OBJECT_PLUS_BUFFER, given value: %1.").arg(
                buffer.distance));
        TERRAMA2_LOG_ERROR() << errMsg;
        throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
      }
      geomResult.reset(geometry->buffer(distance, 16, te::gm::CapButtType));
      break;
    }
    case OBJECT_MINUS_BUFFER:
    {
      if(buffer.distance > 0)
      {
        QString errMsg(QObject::tr(
                "The distance must be negative for the buffer type OBJECT_MINUS_BUFFER, given value: %1.").arg(
                buffer.distance));
        TERRAMA2_LOG_ERROR() << errMsg;
        throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
      }
      geomResult.reset(geometry->buffer(distance, 16, te::gm::CapButtType));
      break;
    }
    case DISTANCE_ZONE:
    {
      geomTemp.reset(geometry->buffer(distance, 16, te::gm::CapButtType));

      double distance2 = terrama2::core::convertDistanceUnit(buffer.distance2, buffer.unit2, "METER");
      geomResult.reset(geomTemp->difference(geometry->buffer(distance2, 16, te::gm::CapButtType)));
      break;
    }
    default:
      break;
  }

  geomResult->setSRID(utmSrid);
  return geomResult;
}

std::shared_ptr<te::mem::DataSet> terrama2::services::analysis::core::createAggregationBuffer(
        std::vector<size_t>& geometries, std::shared_ptr<te::gm::Envelope>& box,
        Buffer buffer)
{
  std::shared_ptr<te::mem::DataSet> dsOut;
  if(geometries.empty())
    return dsOut;

/*
  // Creates memory dataset for buffer
  te::da::DataSetType* dt = new te::da::DataSetType("buffer");

  auto geomSample = geometries[0];
  int geomSampleSrid = geomSample->getSRID();

  te::gm::GeometryProperty* prop = new te::gm::GeometryProperty("geom", 0, te::gm::MultiPolygonType, true);
  prop->setSRID(geomSampleSrid);
  dt->add(prop);


  te::dt::SimpleProperty* prop02 = new te::dt::SimpleProperty("attribute", te::dt::DOUBLE_TYPE, true);
  dt->add(prop02);

  dsOut.reset(new te::mem::DataSet(dt));


  // Inserts each geometry in the rtree, if there is a conflict, it makes the union of the two geometries
  te::sam::rtree::Index<OccurrenceAggregation*, 4> rtree;

  for(size_t i = 0; i < geometries.size(); ++i)
  {
    auto geom = geometries[i];

    double distance = terrama2::core::convertDistanceUnit(buffer.distance, buffer.unit, "METER");

    std::unique_ptr<te::gm::Geometry> tempGeom(dynamic_cast<te::gm::Geometry*>(geom.get()->clone()));
    int utmSrid = terrama2::core::getUTMSrid(tempGeom.get());

    // Converts to UTM in order to create buffer in meters
    if(tempGeom->getSRID() != utmSrid)
    {
      tempGeom->transform(utmSrid);
    }
    std::unique_ptr<te::gm::Geometry> aggBuffer(tempGeom->buffer(distance, 16, te::gm::CapButtType));
    aggBuffer->setSRID(utmSrid);

    // Converts buffer to DataSet SRID in order to compare with the occurrences in the rtree
    aggBuffer->transform(geomSampleSrid);


    std::vector<OccurrenceAggregation*> vec;
    bool aggregated = false;

    // Search for occurrences in the same area
    rtree.search(*(aggBuffer->getMBR()), vec);
    for(std::size_t t = 0; t < vec.size(); ++t)
    {
      OccurrenceAggregation* occurrenceAggregation = vec[t];

      // If the an intersection is found, makes the union of the two geometries and mark the index.
      if(aggBuffer->intersects(occurrenceAggregation->buffer.get()))
      {
        rtree.remove(*(occurrenceAggregation->buffer->getMBR()), occurrenceAggregation);
        occurrenceAggregation->buffer.reset(aggBuffer->Union(occurrenceAggregation->buffer.get()));
        occurrenceAggregation->indexes.push_back(i);
        rtree.insert(*(occurrenceAggregation->buffer->getMBR()), occurrenceAggregation);
        aggregated = true;
      }
    }

    if(!aggregated)
    {
      OccurrenceAggregation* occurrenceAggregation = new OccurrenceAggregation();
      occurrenceAggregation->buffer.reset(aggBuffer.release());
      occurrenceAggregation->indexes.push_back(i);
      rtree.insert(*(aggBuffer->getMBR()), occurrenceAggregation);
    }
  }

  // Fills the memory dataset with the geometries
  std::vector<OccurrenceAggregation*> geomVec;

  rtree.search(*(box.get()), geomVec);

  for(size_t i = 0; i < geomVec.size(); i++)
  {
    auto item = new te::mem::DataSetItem(dsOut.get());
    item->setGeometry(0, dynamic_cast<te::gm::Geometry*>(geomVec[i]->buffer->clone()));
    dsOut->add(item);
  }

*/
  return dsOut;

}
