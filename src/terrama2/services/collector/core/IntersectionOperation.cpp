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
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/Verify.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataAccessorGrid.hpp"
#include "../../../core/data-access/GridSeries.hpp"
#include "../../../Exception.hpp"

// QT
#include <QString>
#include <QObject>

//STL
#include <algorithm>

// TerraLib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/memory/DataSet.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/geometry/Utils.h>
#include <terralib/vp/IntersectionOp.h>
#include <terralib/vp/IntersectionMemory.h>
#include <terralib/vp/Utils.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/raster/Grid.h>
#include <terralib/sa/core/Utils.h>

terrama2::core::DataSetSeries terrama2::services::collector::core::processIntersection(DataManagerPtr dataManager,
                                                                                       IntersectionPtr intersection,
                                                                                       terrama2::core::DataSetSeries collectedDataSetSeries,
                                                                                       std::shared_ptr<te::dt::TimeInstantTZ> executionDate)
{
  if(!intersection)
    return collectedDataSetSeries;

  // Reads the intersection configuration
  auto attrMap = intersection->attributeMap;
  for(auto it = attrMap.begin(); it != attrMap.end(); ++it)
  {
    DataSeriesId dataSeriesId = it->first;
    std::vector<IntersectionAttribute> vecAttributes = it->second;

    auto intersectionDataSeries = dataManager->findDataSeries(dataSeriesId);

    if(intersectionDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GEOMETRIC_OBJECT)
    {
      collectedDataSetSeries = processVectorIntersection(dataManager, collectedDataSetSeries, vecAttributes, intersectionDataSeries, executionDate);
    }
    else if(intersectionDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
    {
      collectedDataSetSeries = processGridIntersection(dataManager, collectedDataSetSeries, vecAttributes, intersectionDataSeries, executionDate);
    }

  }

  return collectedDataSetSeries;

}

terrama2::core::DataSetSeries terrama2::services::collector::core::processVectorIntersection(DataManagerPtr dataManager,
    terrama2::core::DataSetSeries collectedDataSetSeries,
    std::vector<IntersectionAttribute>& vecAttributes,
    terrama2::core::DataSeriesPtr intersectionDataSeries,
    std::shared_ptr<te::dt::TimeInstantTZ> executionDate)
{


  auto collectedData = collectedDataSetSeries.syncDataSet;
  auto collectedDataSetType = collectedDataSetSeries.teDataSetType;

  if(!collectedData)
  {
    QString errMsg(QObject::tr("Invalid TerraLib dataset"));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }

  if(!collectedDataSetType)
  {
    QString errMsg(QObject::tr("Invalid dataset type"));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }


  if(!collectedDataSetSeries.dataSet)
  {
    QString errMsg(QObject::tr("Invalid TerraLib dataset for the collected data"));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }

  std::shared_ptr<te::mem::DataSet> outputDs;
  std::shared_ptr<te::da::DataSetType> outputDt;

  std::vector<te::dt::Property*> collectedProperties = collectedDataSetType->getProperties();


  auto dataProvider = dataManager->findDataProvider(intersectionDataSeries->dataProviderId);

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, intersectionDataSeries);

  terrama2::core::Filter filter;
  filter.discardAfter = executionDate;
  // If dynamic geometric object filter by the last date, doesn't affect static geometric object
  if(intersectionDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GEOMETRIC_OBJECT)
    filter.lastValues = std::make_shared<size_t>(1);

  auto remover = std::make_shared<terrama2::core::FileRemover>();
  // Reads data
  auto seriesMap = accessor->getSeries(filter, remover);

  std::map<std::string, std::string> mapAlias;

  for(auto it = seriesMap.begin(); it != seriesMap.end(); ++it)
  {
    auto interDsType = it->second.teDataSetType;
    auto interDs = it->second.syncDataSet;


    std::vector<te::dt::Property*> interProperties;

    for(const auto& intersectionAttribute : vecAttributes)
    {
      std::string name = intersectionAttribute.attribute;
      mapAlias[intersectionAttribute.alias] = name;

      if(!it->second.teDataSetType)
      {
        name = it->second.teDataSetType->getDatasetName() + "_" + name;
      }


      auto property = it->second.teDataSetType->getProperty(name);
      if(!property)
      {
        QString errMsg(QObject::tr("Invalid attribute name (%1) for data series: %2").arg(name.c_str()).arg(it->first->dataSeriesId));
        TERRAMA2_LOG_ERROR() << errMsg;
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      property->setName(intersectionAttribute.alias);
      interProperties.push_back(property);

    }

    te::sam::rtree::Index<size_t, 8>* rtree(new te::sam::rtree::Index<size_t, 8>);


    auto collectedGeomProperty = te::da::GetFirstGeomProperty(collectedDataSetType.get());
    if(!collectedGeomProperty)
    {
      QString errMsg(QObject::tr("Could not find a geometry property in the collected dataset"));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }
    size_t collectedGeomPropertyPos = collectedDataSetType->getPropertyPosition(collectedGeomProperty);
    int occurrenceSRID = collectedGeomProperty->getSRID();

    auto intersectionGeomProperty = te::da::GetFirstGeomProperty(interDsType.get());

    if(!intersectionGeomProperty)
    {
      QString errMsg(QObject::tr("Could not find a geometry property in the intersection dataset"));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }
    size_t intersectionGeomPos = interDsType->getPropertyPosition(intersectionGeomProperty);

    // Create the DataSetType and DataSet

    outputDt.reset(createDataSetType(collectedDataSetType.get(), interProperties));
    outputDs.reset(new te::mem::DataSet(outputDt.get()));

    // Creates a rtree with all occurrences
    for(unsigned int i = 0; i < collectedData->size(); ++i)
    {
      auto geometry = collectedData->getGeometry(i, collectedGeomPropertyPos);
      rtree->insert(*geometry->getMBR(), i);

      te::mem::DataSetItem* item = new te::mem::DataSetItem(outputDs.get());

      te::gm::Geometry* occurrenceGeom(dynamic_cast<te::gm::Geometry*>(geometry->clone()));
      item->setGeometry(collectedGeomProperty->getName(), occurrenceGeom);

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

      outputDs->add(item);

    }



    for(unsigned int i = 0; i < interDs->size(); ++i)
    {
      auto currGeom = interDs->getGeometry(i, intersectionGeomPos);
      if(!currGeom.get())
        continue;

      terrama2::core::verify::srid(currGeom->getSRID());
      terrama2::core::verify::srid(occurrenceSRID);

      if(currGeom->getSRID() != occurrenceSRID)
        currGeom->transform(occurrenceSRID);

      // Recovers all occurrences that intersects the current geometry envelope
      std::vector<size_t> report;
      rtree->search(*currGeom->getMBR(), report);

      for(size_t k = 0; k < report.size(); ++k)
      {
        std::shared_ptr<te::gm::Geometry> occurrence = collectedData->getGeometry(report[k], collectedGeomPropertyPos);

        // for those geometries that has intersection, copies the selected attributes of the intersection dataset
        if(occurrence->intersects(currGeom.get()))
        {
          for(size_t j = 0; j < interProperties.size(); ++j)
          {
            std::string name = interProperties[j]->getName();

            std::string propName = mapAlias[name];
            if(!interDs->isNull(i, propName))
            {
              auto ad = interDs->getValue(i, propName);
              if(!ad)
                continue;
              outputDs->move(report[k]);
              outputDs->setValue(name, dynamic_cast<te::dt::AbstractData*>(ad.get()->clone()));
            }
          }
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
    std::vector<te::dt::Property*> intersectionDSProperties)
{
  te::da::DataSetType* outputDt = new te::da::DataSetType(collectedDST->getName());


  std::vector<te::dt::Property*> collectedDSProperties = collectedDST->getProperties();

  for(size_t i = 0; i < collectedDSProperties.size(); ++i)
  {
    te::dt::Property* prop = collectedDSProperties[i]->clone();
    outputDt->add(prop);
  }

  for(size_t i = 0; i < intersectionDSProperties.size(); ++i)
  {
    te::dt::Property* prop = intersectionDSProperties[i]->clone();
    outputDt->add(prop);
  }

  return outputDt;
}

std::vector<int> terrama2::services::collector::core::getBands(std::vector<IntersectionAttribute>& vecAttributes)
{
  std::vector<int> bands;
  try
  {
    for(auto& attr : vecAttributes)
    {
      bands.push_back(std::stoi(attr.attribute));
    }
  }
  catch (const std::invalid_argument&)
  {
    QString errMsg(QObject::tr("Invalid value for band intersection.\nNot a number."));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }

  return bands;
}

terrama2::core::DataSetSeries terrama2::services::collector::core::processGridIntersection(DataManagerPtr dataManager,
    terrama2::core::DataSetSeries collectedDataSetSeries,
    std::vector<IntersectionAttribute>& vecAttributes,
    terrama2::core::DataSeriesPtr intersectionDataSeries,
    std::shared_ptr<te::dt::TimeInstantTZ> executionDate)
{
  if(intersectionDataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg(QObject::tr("Invalid DataSeries type for intersection"));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }

  auto collectedData = collectedDataSetSeries.syncDataSet;
  auto collectedDataSetType = collectedDataSetSeries.teDataSetType;

  auto dataProvider = dataManager->findDataProvider(intersectionDataSeries->dataProviderId);

  //accessing data
  terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, intersectionDataSeries);
  auto accessorGrid = std::dynamic_pointer_cast<terrama2::core::DataAccessorGrid>(accessor);

  if(!accessorGrid)
  {
    QString errMsg(QObject::tr("Data accessor invalid for the data series: %1").arg(intersectionDataSeries->id));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
  }

  terrama2::core::Filter filter;
  filter.lastValues = std::make_shared<size_t>(1);
  filter.discardAfter = executionDate;

  auto remover = std::make_shared<terrama2::core::FileRemover>();
  auto gridSeries = accessorGrid->getGridSeries(filter, remover);
  auto gridMap = gridSeries->gridMap();

  std::vector<int> bands = getBands(vecAttributes);

  std::vector<te::dt::Property*> collectedProperties = collectedDataSetType->getProperties();
  std::shared_ptr<te::da::DataSetType> outputDt{dynamic_cast<te::da::DataSetType*>(collectedDataSetType.get()->clone())};

  auto tableNameStr = terrama2::core::simplifyString(intersectionDataSeries->name) + "_band_%1";
  auto propertyName = QString::fromStdString(tableNameStr);
  // Creates one property for each raster band
  for(const auto& band : bands)
  {
    te::dt::Property* property = new te::dt::SimpleProperty(propertyName.arg(band).toStdString() , te::dt::DOUBLE_TYPE);
    outputDt->add(property);
  }

  std::shared_ptr<te::mem::DataSet> outputDs = std::make_shared<te::mem::DataSet>(outputDt.get());

  auto geomProperty = te::da::GetFirstGeomProperty(collectedDataSetType.get());
  auto geomPropertyPos = collectedDataSetType->getPropertyPosition(geomProperty);
  for(auto it = gridMap.begin(); it != gridMap.end(); ++it)
  {
    auto raster = it->second;
    if(!raster)
    {
      QString errMsg(QObject::tr("Invalid raster for intersection data series"));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    auto grid = raster->getGrid();
    if(!grid)
    {
      QString errMsg(QObject::tr("Invalid raster grid for intersection data series"));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }

    for(unsigned int i = 0; i < collectedData->size(); ++i)
    {
      auto currGeom = collectedData->getGeometry(i, geomPropertyPos);

      std::unique_ptr<te::mem::DataSetItem> item(new te::mem::DataSetItem(outputDs.get()));
      // copies all attributes from the collected dataset
      for(size_t j = 0; j < collectedProperties.size(); ++j)
      {
        std::string name = collectedProperties[j]->getName();

        if(!collectedData->isNull(i, name))
        {
          auto ad = collectedData->getValue(i, name);
          if(!ad)
            continue;

          item->setValue(name, dynamic_cast<te::dt::AbstractData*>(ad->clone()));
        }
      }

      terrama2::core::verify::srid(raster->getSRID());

      // Transform the occurrence to the raster projection
      currGeom->transform(raster->getSRID());

      // Gets the respective row and column for the occurrence coordinate
      double row, col;
      te::gm::Coord2D coord = currGeom->getCentroid();
      double x = coord.getX();
      double y = coord.getY();

      grid->geoToGrid(x, y, col, row);

      // Reads the value for each band and sets it in the DataSetItem.
      if(row < raster->getNumberOfRows() && col < raster->getNumberOfColumns() && row >= 0 && col >= 0)
      {
        for(const auto& band : bands)
        {
          double value;
          raster->getValue(col, row, value, band);
          item->setDouble(propertyName.arg(band).toStdString() , value);
        }
      }

      outputDs->moveNext();
      std::size_t aux = te::da::GetFirstSpatialPropertyPos(outputDs.get());

      if(!item->isNull(aux))
        outputDs->add(item.release());
    }
  }

  terrama2::core::SynchronizedDataSetPtr syncDs(new terrama2::core::SynchronizedDataSet(outputDs));
  terrama2::core::DataSetSeries outputDataSeries;
  outputDataSeries.syncDataSet = syncDs;
  outputDataSeries.dataSet = collectedDataSetSeries.dataSet;
  outputDataSeries.teDataSetType = outputDt;

  return outputDataSeries;
}
