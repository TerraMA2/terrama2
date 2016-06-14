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
  \file terrama2/service/collector/core/IntersectionOperation.cpp

  \brief Instersects the collected data with the DataSeries configured in the intersection, in order to add more
  information to the collected data.

  \author Paulo R. M. Oliveira
*/


#include "IntersectionOperation.hpp"
#include "DataManager.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../Exception.hpp"

// QT
#include <QString>
#include <QObject>

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

terrama2::core::DataSetSeries terrama2::services::collector::core::processIntersection(DataManagerPtr dataManager, core::IntersectionPtr intersection, terrama2::core::DataSetSeries collectedDataSetSeries)
{
  auto collectedData = collectedDataSetSeries.syncDataSet;
  auto collectedDataSetType = collectedDataSetSeries.teDataSetType;

  if(!collectedData)
  {
    QString errMsg(QObject::tr("Invalid TerraLib dataset"));
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }

  if(!collectedDataSetType)
  {
    QString errMsg(QObject::tr("Invalid dataset type"));
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }


  if(!collectedDataSetSeries.dataSet)
  {
    QString errMsg(QObject::tr("Invalid dataset"));
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }

  std::shared_ptr<te::mem::DataSet> outputDs;
  std::shared_ptr<te::da::DataSetType> outputDt;

  std::vector<te::dt::Property*> collectedProperties = collectedDataSetType->getProperties();

  // Reads the intersection configuration
  auto attrMap = intersection->attributeMap;
  for(auto it = attrMap.begin(); it != attrMap.end(); ++it)
  {
    DataSeriesId dataSeriesId = it->first;
    std::vector<std::string> vecAttr = it->second;

    auto dataSeries = dataManager->findDataSeries(dataSeriesId);
    auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

    if(dataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
    {
      if(vecAttr.empty())
      {
        QString errMsg(QObject::tr("Empty attribute list in intersection configuration for data series: ").arg(dataSeriesId));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }
    }

    //accessing data
    terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries);

    terrama2::core::Filter filter;

    // In case the DataSeries is GRID we have to use only the last data
    if(dataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
    {
      filter.lastValue = true;
    }

    // Reads data
    auto seriesMap = accessor->getSeries(filter);

    for(auto it = seriesMap.begin(); it != seriesMap.end(); ++it)
    {
      auto interDsType = it->second.teDataSetType;
      auto interDs = it->second.syncDataSet;
      std::vector<te::dt::Property*> interProperties;
      for(std::string attr : vecAttr)
      {
        std::string name = attr;

        if(!it->second.teDataSetType)
          name = it->second.teDataSetType->getDatasetName() + "_" + name;


        interProperties.push_back(it->second.teDataSetType->getProperty(attr));

      }

      te::sam::rtree::Index<size_t, 8>* rtree(new te::sam::rtree::Index<size_t, 8>);
      te::dt::Property* geomProperty = interDsType->findFirstPropertyOfType(te::dt::GEOMETRY_TYPE);
      size_t secGeomPropPos = interDsType->getPropertyPosition(geomProperty);
      te::gm::GeometryProperty* geomProp = te::da::GetFirstGeomProperty(interDsType.get());


      // Create the DataSetType and DataSet

      outputDt.reset(createDataSetType(collectedDataSetType.get(), interDsType.get(), interProperties));
      outputDs.reset(new te::mem::DataSet(outputDt.get()));

      // Creates a rtree with all geometries in the intersection dataset
      size_t secondDsCount = 0;
      int sridSecond = geomProp->getSRID();

      for(unsigned int i = 0; i < interDs->size(); ++i)
      {
        auto geometry = interDs->getGeometry(i, secGeomPropPos);

        rtree->insert(*geometry->getMBR(), secondDsCount);

        ++secondDsCount;
      }


      te::gm::GeometryProperty* fiGeomProp = te::da::GetFirstGeomProperty(collectedDataSetType.get());
      size_t fiGeomPropPos = collectedDataSetType->getPropertyPosition(fiGeomProp);


      for(unsigned int i = 0; i < collectedData->size(); ++i)
      {
        auto currGeom = collectedData->getGeometry(i, fiGeomPropPos);
        if(!currGeom.get())
          continue;

        if(currGeom->getSRID() != sridSecond)
          currGeom->transform(sridSecond);

        // Recovers all geometries that intersects the current geometry envelope
        std::vector<size_t> report;
        rtree->search(*currGeom->getMBR(), report);

        if(!report.empty())
          currGeom->transform(fiGeomProp->getSRID());

        for(size_t k = 0; k < report.size(); ++k)
        {

          te::mem::DataSetItem* item = new te::mem::DataSetItem(outputDs.get());

          item->setGeometry(fiGeomProp->getName(), dynamic_cast<te::gm::Geometry*>(currGeom.get()->clone()));

          // copies all attributes from the collected dataset
          for(size_t j = 0; j < collectedProperties.size(); ++j)
          {
            std::string name = collectedProperties[j]->getName();

            if(!collectedData->isNull(i, collectedProperties[j]->getName()))
            {
              auto ad = collectedData->getValue(i, collectedProperties[j]->getName());

              if(!ad)
                continue;

              item->setValue(name, dynamic_cast<te::dt::AbstractData*>(ad->clone()));
            }
          }

          auto secGeom = interDs->getGeometry(report[k], secGeomPropPos);
          secGeom->setSRID(sridSecond);

          if(secGeom->getSRID() != fiGeomProp->getSRID())
            secGeom->transform(fiGeomProp->getSRID());

          // for those geometries that has intersection, copies the selected attributes of the intersection dataset
          if(currGeom->intersects(secGeom.get()))
          {
            for(size_t j = 0; j < interProperties.size(); ++j)
            {
              std::string name = interProperties[j]->getName();

              if(!interDsType->getName().empty())
                name = te::vp::GetSimpleTableName(interDsType->getName()) + "_" + name;

              if(!interDs->isNull(report[k], interProperties[j]->getName()))
              {
                auto ad = interDs->getValue(report[k], interProperties[j]->getName());
                item->setValue(name, dynamic_cast<te::dt::AbstractData*>(ad.get()->clone()));
              }
            }
          }

          outputDs->moveNext();

          std::size_t aux = te::da::GetFirstSpatialPropertyPos(outputDs.get());

          if(!item->isNull(aux))
            outputDs->add(item);

          break;
        }
      }
    }
  }


  terrama2::core::SynchronizedDataSetPtr syncDs(new terrama2::core::SynchronizedDataSet(outputDs));
  terrama2::core::DataSetSeries outputDataSeries;
  outputDataSeries.syncDataSet = syncDs;
  outputDataSeries.dataSet = collectedDataSetSeries.dataSet;
  outputDataSeries.teDataSetType = outputDt;

  return outputDataSeries;
}


te::da::DataSetType* terrama2::services::collector::core::createDataSetType(te::da::DataSetType* collectedDST,
                                                                            te::da::DataSetType* intersectionDST,
                                                                            std::vector<te::dt::Property*> intersectionDSProperties)
{
  te::da::DataSetType* outputDt = new te::da::DataSetType(collectedDST->getName());


  std::vector<te::dt::Property*> collectedDSProperties = collectedDST->getProperties();

  for(size_t i = 0; i < collectedDSProperties.size(); ++i)
  {
    te::dt::Property* prop = collectedDSProperties[i]->clone();
    if(!collectedDST->getName().empty())
      prop->setName(prop->getName());
    outputDt->add(prop);
  }

  for(size_t i = 0; i < intersectionDSProperties.size(); ++i)
  {
    te::dt::Property* prop = intersectionDSProperties[i]->clone();
    if(!intersectionDST->getName().empty())
      prop->setName(te::vp::GetSimpleTableName(intersectionDST->getName()) + "_" + prop->getName());
    outputDt->add(prop);
  }

  return outputDt;
}
