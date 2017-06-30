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
  \file terrama2/services/view/core/MemoryDataSetLayer.cpp

  \brief DataSet layer to deal with an dataSet in memory(a dataSet without a dataSource).

  \author Vinicius Campanha
*/

// TerraMA2
#include "MemoryDataSetLayer.hpp"


te::map::MemoryDataSetLayer::MemoryDataSetLayer(const std::string& id,
                   const std::string& title,
                   std::shared_ptr< te::da::DataSet > dataSet,
                   std::shared_ptr< LayerSchema > memoryLayerSchema)
  : DataSetLayer(id, title),
    dataSet_(dataSet),
    memoryLayerSchema_(memoryLayerSchema)
{

}

void te::map::MemoryDataSetLayer::setSRID(int srid)
{
  // set the srid associated to the parent AbstractLayer
  m_srid=srid;

  // propagate it to my cached dataset schema
  if(memoryLayerSchema_)
  {
    if (memoryLayerSchema_->hasGeom())
    {
      gm::GeometryProperty* myGeom = te::da::GetFirstGeomProperty(memoryLayerSchema_.get());
      myGeom->setSRID(srid);
    }
    else if (memoryLayerSchema_->hasRaster())
    {
      rst::RasterProperty* rstProp = te::da::GetFirstRasterProperty(memoryLayerSchema_.get());
      rstProp->getGrid()->setSRID(srid);
    }
  }
}


std::auto_ptr<te::map::LayerSchema> te::map::MemoryDataSetLayer::getSchema() const
{
  return std::auto_ptr< LayerSchema >(dynamic_cast<LayerSchema*>(memoryLayerSchema_->clone()));
}


std::auto_ptr<te::da::DataSet> te::map::MemoryDataSetLayer::getData(te::common::TraverseType /*travType*/,
                                       const te::common::AccessPolicy /*accessPolicy*/) const
{
  throw Exception((TE_TR("This method was not implemented.!")));
}


std::auto_ptr<te::da::DataSet> te::map::MemoryDataSetLayer::getData(const std::string& /*propertyName*/,
                                       const te::gm::Envelope* /*e*/,
                                       te::gm::SpatialRelation /*r*/,
                                       te::common::TraverseType /*travType*/,
                                       const te::common::AccessPolicy /*accessPolicy*/) const
{
  throw Exception((TE_TR("This method was not implemented.!")));
}


std::auto_ptr<te::da::DataSet> te::map::MemoryDataSetLayer::getData(const std::string& /*propertyName*/,
                                       const te::gm::Geometry* /*g*/,
                                       te::gm::SpatialRelation /*r*/,
                                       te::common::TraverseType /*travType*/,
                                       const te::common::AccessPolicy /*accessPolicy*/) const
{
  throw Exception((TE_TR("This method was not implemented.!")));
}


std::auto_ptr<te::da::DataSet> te::map::MemoryDataSetLayer::getData(te::da::Expression* /*restriction*/,
                                       te::common::TraverseType /*travType*/,
                                       const te::common::AccessPolicy /*accessPolicy*/) const
{
  throw Exception((TE_TR("This method was not implemented.!")));
}


std::auto_ptr<te::da::DataSet> te::map::MemoryDataSetLayer::getData(const te::da::ObjectIdSet* /*oids*/,
                                       te::common::TraverseType /*travType*/,
                                       const te::common::AccessPolicy /*accessPolicy*/) const
{
  throw Exception((TE_TR("This method was not implemented.!")));
}


bool te::map::MemoryDataSetLayer::isValid() const
{
  if(m_datasetName.empty())
    return false;

  if(!dataSet_)
    return false;

  if(!memoryLayerSchema_)
    return false;

  return true;
}


void te::map::MemoryDataSetLayer::draw(Canvas* canvas, const te::gm::Envelope& bbox, int srid, const double& scale, bool* cancel)
{
  // Try get the defined renderer
  std::shared_ptr<MemoryDataSetRenderer> renderer(new MemoryDataSetRenderer(dataSet_, memoryLayerSchema_));
  if(renderer.get() == 0)
    throw Exception((boost::format(TE_TR("Could not draw the memory data set layer %1%. The memory data set renderer could not be created!")) % getTitle()).str());

  renderer->draw(this, canvas, bbox, srid, scale, cancel);
}
