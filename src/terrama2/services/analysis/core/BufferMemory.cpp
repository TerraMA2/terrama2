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
#include "../../../core/data-access/SyncronizedDataSet.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/Logger.hpp"

// TerraLib
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/GeometryProperty.h>

//QT
#include <QObject>
#include <QString>

std::shared_ptr<te::gm::Geometry> terrama2::services::analysis::core::createBuffer(Buffer buffer, std::shared_ptr<te::gm::Geometry> geometry)
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


  switch (buffer.bufferType)
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
    case INTERN_PLUS_EXTERN:
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
        QString errMsg(QObject::tr("The distance must be positive for the buffer type OBJECT_PLUS_BUFFER, given value: %1.").arg(buffer.distance));
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
        QString errMsg(QObject::tr("The distance must be negative for the buffer type OBJECT_MINUS_BUFFER, given value: %1.").arg(buffer.distance));
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

std::shared_ptr<te::mem::DataSet> terrama2::services::analysis::core::createAggregationBuffer(std::vector<std::shared_ptr<te::gm::Geometry> >& geometries, std::shared_ptr<te::gm::Envelope>& box, Buffer buffer)
{

  // Creates memory dataset for buffer
  te::da::DataSetType* dt = new te::da::DataSetType("buffer");

  assert(geometries.size() > 0);
  auto geomSample = geometries[0];

  te::gm::GeometryProperty* prop = new te::gm::GeometryProperty("geom", 0, te::gm::MultiPolygonType, true);
  prop->setSRID(geomSample->getSRID());
  dt->add(prop);

  std::shared_ptr<te::mem::DataSet> dsOut(new te::mem::DataSet(dt));



  // Inserts each geometry in the rtree, if there is a conflict, it makes the union of the two geoemtries
  te::sam::rtree::Index<te::gm::Geometry*, 4> rtree;

  for(size_t i = 0; i < geometries.size(); ++i)
  {
    auto geom = geometries[i];

    double distance = terrama2::core::convertDistanceUnit(buffer.distance, buffer.unit, "METER");

    auto aggBuffer = geom->buffer(distance, 16, te::gm::CapButtType);

    std::vector<te::gm::Geometry*> vec;

    rtree.search(*(aggBuffer->getMBR()), vec);

    for(std::size_t t = 0; t < vec.size(); ++t)
    {
      if(aggBuffer->intersects(vec[t]))
      {
        aggBuffer = aggBuffer->Union(vec[t]);
        rtree.remove(*(vec[t]->getMBR()), vec[t]);
      }
    }

    rtree.insert(*(aggBuffer->getMBR()), aggBuffer);

  }

  // Fills the memory dataset with the geometries
  std::vector<te::gm::Geometry*> geomVec;

  rtree.search(*(box.get()), geomVec);

  for (size_t i = 0; i < geomVec.size(); i++)
  {
    auto item = new te::mem::DataSetItem(dsOut.get());
    item->setGeometry(0, geomVec[i]);
    dsOut->add(item);
  }

  return dsOut;

}
