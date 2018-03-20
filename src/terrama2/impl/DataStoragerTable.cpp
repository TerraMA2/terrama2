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
  \file terrama2/core/data-access/DataStoragerTable.cpp

  \brief

  \author Jano Simas
 */

#include "DataStoragerTable.hpp"

#include "../core/data-model/DataProvider.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Utils.hpp"

//terralib
#include <terralib/dataaccess/datasource/DataSourceCapabilities.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/NumericProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

//Qt
#include <QUrl>

//STL
#include <algorithm>

//Boost
#include <boost/algorithm/string.hpp>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

std::unique_ptr<te::dt::Property> terrama2::core::DataStoragerTable::copyProperty(te::dt::Property* property) const
{
  auto name = property->getName();
  auto type = property->getType();
  switch (type)
  {
    case te::dt::INT16_TYPE:
    case te::dt::UINT16_TYPE:
    case te::dt::INT32_TYPE:
    case te::dt::UINT32_TYPE:
    case te::dt::INT64_TYPE:
    case te::dt::UINT64_TYPE:
    case te::dt::BOOLEAN_TYPE:
    case te::dt::FLOAT_TYPE:
    case te::dt::DOUBLE_TYPE:
      return std::unique_ptr<te::dt::Property>(new te::dt::SimpleProperty(name, type));
    case te::dt::STRING_TYPE:
      {
        auto stringProperty = dynamic_cast<te::dt::StringProperty*>(property);
        if(!stringProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::dt::StringProperty(name,
                                                                            stringProperty->getSubType(),
                                                                            stringProperty->size()));
      }
    case te::dt::DATETIME_TYPE:
      {
        auto dateTime = dynamic_cast<te::dt::DateTimeProperty*>(property);
        if(!dateTime)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::dt::DateTimeProperty(name,
                                                                              dateTime->getSubType(),
                                                                              dateTime->getPrecision()));
      }
    case te::dt::NUMERIC_TYPE:
      {
        auto numericProperty = dynamic_cast<te::dt::NumericProperty*>(property);
        if(!numericProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::dt::NumericProperty(name,
                                                                             numericProperty->getPrecision(),
                                                                             numericProperty->getScale()));
      }
    case te::dt::GEOMETRY_TYPE:
      {
        auto geomProperty = dynamic_cast<te::gm::GeometryProperty*>(property);
        if(!geomProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::gm::GeometryProperty(name,
                                                                              geomProperty->getSRID(),
                                                                              geomProperty->getGeometryType()));
      }
    case te::dt::RASTER_TYPE:
      {
        auto rasterProperty = dynamic_cast<te::rst::RasterProperty*>(property);
        if(!rasterProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        auto newProperty =  std::unique_ptr<te::dt::Property>(new te::rst::RasterProperty(rasterProperty->getGrid(),
                                                                                          rasterProperty->getBandProperties(),
                                                                                          rasterProperty->getInfo()));
        newProperty->setName(name);
        return newProperty;
      }
    default:
      {
        QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
        TERRAMA2_LOG_ERROR() << errMsg;
        throw DataStoragerException() << ErrorDescription(errMsg);
      }

  }
}

std::shared_ptr<te::da::DataSetType> terrama2::core::DataStoragerTable::copyDataSetType(std::shared_ptr<te::da::DataSetType> dataSetType, const std::string& newDataSetName) const
{
  std::shared_ptr< te::da::DataSetType > newDatasetType = std::make_shared<te::da::DataSetType>(newDataSetName);

  for(const auto& property : dataSetType->getProperties())
  {
    auto newProperty = copyProperty(property);
    newDatasetType->add(newProperty.release());
  }

  return newDatasetType;
}

void terrama2::core::DataStoragerTable::updateAttributeNames(std::shared_ptr<te::mem::DataSet> teDataSet, std::shared_ptr<te::da::DataSetType> dataSetType, terrama2::core::DataSetPtr dataset) const
{
  try
  {
    auto attributeMapJson = dataset->format.at("inout_attribute_map");

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(attributeMapJson.c_str(), &error);

    if(error.error != QJsonParseError::NoError)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson parse error: %1\n").arg(error.errorString());
      return;
    }
    auto jsonObject = jsonDoc.object();

    for(auto& property : dataSetType->getProperties())
    {
      auto oldName = QString::fromStdString(property->getName());
      if(jsonObject.contains(oldName))
      {
        auto newName = jsonObject[oldName].toString().toStdString();
        property->setName(newName);
      }
    }

    const std::size_t np = teDataSet->getNumProperties();
    for(std::size_t i = 0; i != np; ++i)
    {
      auto oldName = QString::fromStdString(teDataSet->getPropertyName(i));
      if(jsonObject.contains(oldName))
      {
        auto newName = jsonObject[oldName].toString().toStdString();
        teDataSet->setPropertyName(newName, i);
      }
    }
  }
  catch (std::out_of_range)
  {
    // no inout_attribute_map
    // no need to update names
  }
}

void terrama2::core::DataStoragerTable::store(DataSetSeries series, DataSetPtr outputDataSet) const
{
  if(!dataProvider_)
  {
    QString errMsg = QObject::tr("Invalid data provider");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  te::core::URI uri(getCompleteURI(outputDataSet));
  std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make(driver(), uri));

  OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);
  // if(!datasourceDestination->isOpened())
  // {
  //   QString errMsg = QObject::tr("Could not connect to database");
  //   TERRAMA2_LOG_ERROR() << errMsg;
  //   throw DataProviderException() << ErrorDescription(errMsg);
  // }

  // FIXME: verify if it's open
  // there is a bug is the method for CSV that returns false, check if it's fixed

  auto capabilities = datasourceDestination->getCapabilities();
  auto typeCapabilities = capabilities.getDataSetTypeCapabilities();

  std::string destinationDataSetName = boost::to_lower_copy(getDataSetName(outputDataSet));

  std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
  te::da::ScopedTransaction scopedTransaction(*transactorDestination);

  std::shared_ptr<te::da::DataSetType> datasetType = series.teDataSetType;
  updateAttributeNames(std::static_pointer_cast<te::mem::DataSet>(series.syncDataSet->dataset()), datasetType, outputDataSet);

  std::shared_ptr<te::da::DataSetType> newDataSetType;
  if (!transactorDestination->dataSetExists(destinationDataSetName))
  {
    // create and save datasettype in the datasource destination
    // newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(datasetType->clone()));
    newDataSetType = copyDataSetType(datasetType, destinationDataSetName);

    if(typeCapabilities.supportsPrimaryKey())
    {
      std::string pkName = "\""+destinationDataSetName+"_pk\"";
      std::unique_ptr<te::da::PrimaryKey> pk(new te::da::PrimaryKey(pkName));

      std::unique_ptr<te::dt::SimpleProperty> serialPk(new te::dt::SimpleProperty ("pid_"+destinationDataSetName, te::dt::INT32_TYPE, true));
      serialPk->setAutoNumber(true);
      pk->add(serialPk.get());
      newDataSetType->add(pk.release());
      newDataSetType->add(serialPk.release());
    }

    //Get original geometry to get srid
    te::gm::GeometryProperty* geom = GetFirstGeomProperty(datasetType.get());
    //configure if there is a geometry property
    if(geom)
    {
      auto geomPropertyName = getGeometryPropertyName(outputDataSet);
      te::gm::GeometryProperty* geomProperty = dynamic_cast<te::gm::GeometryProperty*>(newDataSetType->getProperty(geomPropertyName));
      if(!geomProperty)
      {
        QString errMsg = QObject::tr("Unable to find geometric property %1").arg(QString::fromStdString(geomPropertyName));
        TERRAMA2_LOG_ERROR() << errMsg;
        throw DataStoragerException() << ErrorDescription(errMsg);
      }
      geomProperty->setSRID(geom->getSRID());
      geomProperty->setGeometryType(geom->getGeometryType());

      if(typeCapabilities.supportsRTreeIndex())
      {
        // the newDataSetType takes ownership of the pointer
        auto spatialIndex = new te::da::Index("spatial_index_" + destinationDataSetName, te::da::R_TREE_TYPE, {geomProperty});
        newDataSetType->add(spatialIndex);
      }
    }

    transactorDestination->createDataSet(newDataSetType.get(), {});
  }
  else
  {
    newDataSetType = transactorDestination->getDataSetType(destinationDataSetName);
  }

  adapt(series);

  const auto& oldPropertiesList = newDataSetType->getProperties();
  for(const auto & property : datasetType->getProperties())
  {
    auto it = std::find_if(oldPropertiesList.cbegin(), oldPropertiesList.cend(), std::bind(&terrama2::core::DataStoragerTable::isPropertyEqual, this, property, std::placeholders::_1));
    if(it == oldPropertiesList.cend())
      transactorDestination->addProperty(newDataSetType->getName(), copyProperty(property).get());
  }

  series.syncDataSet->dataset()->moveBeforeFirst();
  transactorDestination->add(newDataSetType->getName(), series.syncDataSet->dataset().get(), {});

  scopedTransaction.commit();
}

std::string terrama2::core::DataStoragerTable::getGeometryPropertyName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "geometry_property");
}

bool terrama2::core::DataStoragerTable::isPropertyEqual(te::dt::Property* newProperty, te::dt::Property* oldMember) const
{
  std::string newPropertyName = newProperty->getName();
  std::transform(newPropertyName.begin(), newPropertyName.end(), newPropertyName.begin(), ::tolower);

  std::string oldPropertyName = oldMember->getName();
  std::transform(oldPropertyName.begin(), oldPropertyName.end(), oldPropertyName.begin(), ::tolower);

  bool noEqual = newPropertyName == oldPropertyName;
  if(!noEqual)
    return false;

  if(newProperty->getType() != oldMember->getType())
  {
    QString errMsg = QObject::tr("Wrong column type: %1").arg(QString::fromStdString(newProperty->getName()));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  return true;
}
