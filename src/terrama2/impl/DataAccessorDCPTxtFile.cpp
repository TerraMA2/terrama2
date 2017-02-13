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
  \file terrama2/impl/DataAccessorTxtFile.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "DataAccessorDCPTxtFile.hpp"
#include "../core/data-model/DataSetDcp.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/Logger.hpp"

// TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/geometry/GeometryProperty.h>

//QT
#include <QUrl>
#include <QDir>
#include <QSet>
#include <QTemporaryFile>
#include <QJsonObject>
#include <QJsonDocument>

// Boost
#include <boost/bind.hpp>

// STL
#include <fstream>


terrama2::core::DataAccessorPtr terrama2::core::DataAccessorDCPTxtFile::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
{
  return std::make_shared<DataAccessorDCPTxtFile>(dataProvider, dataSeries);
}

void terrama2::core::DataAccessorDCPTxtFile::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  bool convertAll = getConvertAll(dataSet);

  QJsonObject fieldGeomObj;

  QJsonDocument doc = QJsonDocument::fromJson(getProperty(dataSet, dataSeries_, "fields").c_str());

  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);

    QJsonObject fieldObj = getFieldObj(doc, property->getName());

    if(fieldObj.empty())
    {
      if(convertAll)
      {
        std::string alias = DataAccessorTxtFile::simplifyString(property->getName());

        std::string defaultType = getProperty(dataSet, dataSeries_, "default_type");

        if(dataTypes.at(defaultType) != te::dt::STRING_TYPE)
        {
          te::dt::SimpleProperty* defaultProperty = new te::dt::SimpleProperty(alias, dataTypes.at(defaultType));

          converter->add(i,defaultProperty);
        }
        else
        {
          te::dt::Property* defaultProperty = property->clone();
          defaultProperty->setName(alias);

          converter->add(i,defaultProperty);
        }
      }

      converter->remove(property->getName());

      continue; // for(size_t i = 0, size = properties.size(); i < size; ++i)
    }

    int type = dataTypes.at(fieldObj.value("type").toString().toStdString());

    if(type == te::dt::GEOMETRY_TYPE)
    {
      fieldGeomObj = fieldObj;
      continue;
    }

    std::string alias = fieldObj.value("alias").toString().toStdString();

    if(alias.empty())
      alias = DataAccessorTxtFile::simplifyString(property->getName());

    switch (type)
    {
      case te::dt::DOUBLE_TYPE:
      {
        te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(alias, type);

        converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
        break;
      }
      case te::dt::INT32_TYPE:
      {
        te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(alias, type);

        converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToInt, this, _1, _2, _3));
        break;
      }
      case te::dt::DATETIME_TYPE:
      {
        std::string format = terramaDateMask2BoostFormat(fieldObj.value("format").toString().toStdString());

        te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(alias, te::dt::TIME_INSTANT_TZ);
        converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorDCPTxtFile::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet), format));

        break;
      }
      default:
      {
        te::dt::Property* defaultProperty = property->clone();
        defaultProperty->setName(alias);

        converter->add(i,defaultProperty);
        break;
      }
    }

    converter->remove(property->getName());
  }

  if(!fieldGeomObj.empty())
  {
    size_t longPos = std::numeric_limits<size_t>::max();
    size_t latPos = std::numeric_limits<size_t>::max();

    longPos = converter->getConvertee()->getPropertyPosition(fieldGeomObj.value("longitude").toString().toStdString());
    latPos = converter->getConvertee()->getPropertyPosition(fieldGeomObj.value("latitude").toString().toStdString());

    if(longPos == std::numeric_limits<size_t>::max() ||
       latPos == std::numeric_limits<size_t>::max())
    {
      QString errMsg = QObject::tr("Could not find the point complete information!");
      TERRAMA2_LOG_WARNING() << errMsg;
      throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
    }

    std::vector<size_t> latLonAttributes;
    latLonAttributes.push_back(longPos);
    latLonAttributes.push_back(latPos);

    Srid srid = getSrid(dataSet);
    std::string alias = fieldGeomObj.value("alias").toString().toStdString();

    te::gm::GeometryProperty* geomProperty = new te::gm::GeometryProperty(alias, srid, te::gm::PointType);

    converter->add(latLonAttributes, geomProperty, boost::bind(&terrama2::core::DataAccessorDCPTxtFile::stringToPoint, this, _1, _2, _3, srid));

    converter->remove(longPos);
    converter->remove(latPos);
  }
}

