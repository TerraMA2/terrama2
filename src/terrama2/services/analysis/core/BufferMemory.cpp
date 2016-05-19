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

// TerraLib
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/GeometryProperty.h>

std::unique_ptr<te::gm::Geometry> terrama2::services::analysis::core::createBuffer(BufferType bufferType, std::shared_ptr<te::gm::Geometry> geometry, double distance)
{
  // Converts the data to UTM
  int utmSrid = terrama2::core::getUTMSrid(geometry.get());

  std::unique_ptr<te::gm::Geometry> utmGeom(dynamic_cast<te::gm::Geometry*>(geometry.get()->clone()));
  utmGeom->transform(utmSrid);

  std::unique_ptr<te::gm::Geometry> geomResult;
  std::unique_ptr<te::gm::Geometry> tempGeom;

  switch (bufferType)
  {
    case NONE:
    {
      geomResult = std::move(utmGeom);
      break;
    }
    case EXTERN:
    {
      tempGeom.reset(utmGeom->buffer(distance, 16, te::gm::CapButtType));
      geomResult.reset(tempGeom->difference(utmGeom.get()));
      break;
    }
    case INTERN:
    {
      tempGeom.reset(utmGeom->buffer(-distance, 16, te::gm::CapButtType));
      geomResult.reset(tempGeom->difference(utmGeom.get()));
      break;
    }
    case INTERN_PLUS_EXTERN:
    {
      tempGeom.reset(utmGeom->buffer(distance, 16, te::gm::CapButtType));
      std::unique_ptr<te::gm::Geometry> auxGeom(utmGeom->buffer(-distance, 16, te::gm::CapButtType));
      geomResult.reset(tempGeom->difference(auxGeom.get()));
      break;
    }
    case OBJECT_PLUS_EXTERN:
    {
      geomResult.reset(utmGeom->buffer(distance, 16, te::gm::CapButtType));
      break;
    }
    case OBJECT_WITHOUT_INTERN:
    {
      tempGeom.reset(utmGeom->buffer(-distance, 16, te::gm::CapButtType));
      geomResult.reset(tempGeom->difference(utmGeom.get()));
      break;
    }
  }

  return geomResult;
}

std::shared_ptr<te::mem::DataSet> terrama2::services::analysis::core::createAggregationBuffer(std::vector<std::shared_ptr<te::gm::Geometry> >& geometries, std::shared_ptr<te::gm::Envelope>& box, double distance, BufferType bufferType)
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
    auto buffer = geom->buffer(distance, 16, te::gm::CapButtType);

    std::vector<te::gm::Geometry*> vec;

    rtree.search(*(buffer->getMBR()), vec);

    for(std::size_t t = 0; t < vec.size(); ++t)
    {
      if(buffer->intersects(vec[t]))
      {
        buffer = buffer->Union(vec[t]);
        rtree.remove(*(vec[t]->getMBR()), vec[t]);
      }
    }

    rtree.insert(*(buffer->getMBR()), buffer);

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
