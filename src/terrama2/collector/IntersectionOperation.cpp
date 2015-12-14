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
  \file terrama2/collector/IntersectionOperation.cpp

  \brief Instersects the collected data with the intersection data configured in the dataset, in order to add more information to the collected data.

  \author Paulo R. M. Oliveira
*/


#include "IntersectionOperation.hpp"


// TerraMA2
#include "../core/DataSet.hpp"
#include "../core/Intersection.hpp"
#include "../core/ApplicationController.hpp"
#include "../core/DataManager.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/memory/DataSet.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/vp/IntersectionOp.h>
#include <terralib/vp/IntersectionMemory.h>
#include <terralib/vp/Utils.h>

std::shared_ptr<te::da::DataSet> terrama2::collector::processIntersection(const terrama2::core::DataSet& dataSet, std::shared_ptr<te::da::DataSet> collectedData, std::shared_ptr<te::da::DataSetType>& collectedDataSetType)
{
  assert(collectedData.get());

  std::shared_ptr<te::mem::DataSet> outputDs;
  std::shared_ptr<te::da::DataSetType> outputDt;

  auto transactor = terrama2::core::ApplicationController::getInstance().getTransactor();

  // Reads the vectorial intersection configuration
  terrama2::core::Intersection intersection = dataSet.intersection();
  auto attrMap = intersection.attributeMap();
  for(auto it = attrMap.begin(); it != attrMap.end(); ++it)
  {
    std::string interDsName = it->first;
    std::vector<std::string> vecAttr = it->second;

    // Reads the intersection data
    std::shared_ptr<te::da::DataSource> srcDs2 = terrama2::core::ApplicationController::getInstance().getDataSource();
    auto interDsType = srcDs2->getDataSetType(interDsName);
    auto interDs = srcDs2->getDataSet(interDsName);
    if(interDs.get())
    {

      std::vector<te::dt::Property*> collectedProperties = collectedDataSetType->getProperties();
      std::vector<te::dt::Property*> interProperties;
      for(std::string attr : vecAttr)
      {
        std::string name = attr;

        if (!interDsName.empty())
          name = te::vp::GetSimpleTableName(interDsName) + "_" + name;


        interProperties.push_back(interDsType->getProperty(attr));

      }

      te::sam::rtree::Index<size_t, 8>* rtree(new te::sam::rtree::Index<size_t, 8>);
      te::dt::Property* geomProperty = interDsType->findFirstPropertyOfType(te::dt::GEOMETRY_TYPE);
      size_t secGeomPropPos = interDsType->getPropertyPosition(geomProperty);
      te::gm::GeometryProperty* geomProp = te::da::GetFirstGeomProperty(interDsType.get());


      // Create the DataSetType and DataSet

      outputDt.reset(createDataSetType(collectedDataSetType->getName() + "int", collectedDataSetType.get(), collectedProperties, interDsType.get(), interProperties));
      outputDs.reset(new te::mem::DataSet(outputDt.get()));

      size_t secondDsCount = 0;
      int sridSecond = geomProp->getSRID();

      interDs->moveBeforeFirst();
      while(interDs->moveNext())
      {
        std::auto_ptr<te::gm::Geometry> g = interDs->getGeometry(secGeomPropPos);

        rtree->insert(*g->getMBR(), secondDsCount);

        ++secondDsCount;
      }

      collectedData->moveBeforeFirst();

      te::gm::GeometryProperty* fiGeomProp = te::da::GetFirstGeomProperty(collectedDataSetType.get());
      size_t fiGeomPropPos = collectedDataSetType->getPropertyPosition(fiGeomProp);

      int pk = 0;

      while(collectedData->moveNext())
      {
        std::auto_ptr<te::gm::Geometry> currGeom = collectedData->getGeometry(fiGeomPropPos);
        if(!currGeom.get())
          continue;

        if(currGeom->getSRID() != sridSecond)
          currGeom->transform(sridSecond);

        std::vector<size_t> report;
        rtree->search(*currGeom->getMBR(), report);

        if(!report.empty())
          currGeom->transform(fiGeomProp->getSRID());

        for(size_t i = 0; i < report.size(); ++i)
        {
          interDs->move(report[i]);
          std::auto_ptr<te::gm::Geometry> secGeom = interDs->getGeometry(secGeomPropPos);
          secGeom->setSRID(sridSecond);

          if(secGeom->getSRID() != fiGeomProp->getSRID())
            secGeom->transform(fiGeomProp->getSRID());

          if(!currGeom->intersects(secGeom.get()))
            continue;

          te::mem::DataSetItem* item = new te::mem::DataSetItem(outputDs.get());

          item->setGeometry("geom", currGeom.release());

          for(size_t j = 0; j < collectedProperties.size(); ++j)
          {
            std::string name = collectedProperties[j]->getName();

            te::dt::AbstractData* ad = collectedData->getValue(collectedProperties[j]->getName()).release();

            item->setValue(name, ad);
          }

          for(size_t j = 0; j < interProperties.size(); ++j)
          {
            std::string name = interProperties[j]->getName();

            if (!interDsType->getName().empty())
              name = te::vp::GetSimpleTableName(interDsType->getName()) + "_" + name;

            te::dt::AbstractData* ad = interDs->getValue(interProperties[j]->getName()).release();

            item->setValue(name, ad);
          }

          item->setInt32(collectedDataSetType->getName() + "int" + "_id", pk);
          ++pk;

          outputDs->moveNext();

          std::size_t aux = te::da::GetFirstSpatialPropertyPos(outputDs.get());

          if(!item->isNull(aux))
            outputDs->add(item);

          break;
        }
      }

      collectedData = outputDs;
      outputDs.reset();
      collectedDataSetType = outputDt;
      outputDt.reset();



    }
  }

  return collectedData;
}



te::da::DataSetType* terrama2::collector::createDataSetType(std::string newName,
                                                                   te::da::DataSetType* firstDt,
                                                                   std::vector<te::dt::Property*> firstProps,
                                                                   te::da::DataSetType* secondDt,
                                                                   std::vector<te::dt::Property*> secondProps)
{
  te::da::DataSetType* outputDt = new te::da::DataSetType(newName);



  te::dt::SimpleProperty* pkProperty = new te::dt::SimpleProperty(newName + "_id", te::dt::INT32_TYPE);
  pkProperty->setAutoNumber(true);
  outputDt->add(pkProperty);

  te::da::PrimaryKey* pk = new te::da::PrimaryKey(newName + "_pk", outputDt);
  pk->add(pkProperty);
  outputDt->setPrimaryKey(pk);

  for(size_t i = 0; i < firstProps.size(); ++i)
  {
    te::dt::Property* prop = firstProps[i]->clone();
    if (!firstDt->getName().empty())
      prop->setName(prop->getName());
    outputDt->add(prop);
  }

  for(size_t i = 0; i < secondProps.size(); ++i)
  {
    te::dt::Property* prop = secondProps[i]->clone();
    if (!secondDt->getName().empty())
      prop->setName(te::vp::GetSimpleTableName(secondDt->getName()) + "_" + prop->getName());
    outputDt->add(prop);
  }

  te::gm::GeometryProperty* newGeomProp = new te::gm::GeometryProperty("geom");
  newGeomProp->setGeometryType(te::da::GetFirstGeomProperty(firstDt)->getGeometryType());
  newGeomProp->setSRID(te::da::GetFirstGeomProperty(firstDt)->getSRID());

  outputDt->add(newGeomProp);

  return outputDt;
}