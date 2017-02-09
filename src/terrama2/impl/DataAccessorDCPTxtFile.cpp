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
  std::vector<std::tuple< std::vector<std::string>, std::string, int>> fields;

  fields = getFields(dataSet);
  bool convertAll = getConvertAll(dataSet);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);

    if(property->getName() == getProperty(dataSet, dataSeries_, "timestamp_property"))
    {
      std::string alias;

      try
      {
          getProperty(dataSet, dataSeries_, "timestamp_property_alias");
      }
      catch(UndefinedTagException /*e*/)
      {
        // Do nothing
      }

      if(alias.empty())
        alias = DataAccessorTxtFile::simplifyString(property->getName());

      te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(alias, te::dt::TIME_INSTANT_TZ);
      converter->add(i, dtProperty, boost::bind(&terrama2::core::DataAccessorDCPTxtFile::stringToTimestamp, this, _1, _2, _3, getTimeZone(dataSet), getTimestampPropertyName(dataSet)));

      converter->remove(property->getName());

      continue;
    }

    bool converted = false;

    for(auto& field : fields)
    {
      std::string& propertyName = std::get<0>(field).at(0);

      if(propertyName == property->getName())
      {
        std::string alias = std::get<1>(field);
        int type = std::get<2>(field);

        if(alias.empty())
          alias = DataAccessorTxtFile::simplifyString(property->getName());

        te::dt::SimpleProperty* newProperty = new te::dt::SimpleProperty(alias, type);

        switch (type)
        {
          case te::dt::DOUBLE_TYPE:
          {
            converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToDouble, this, _1, _2, _3));
            break;
          }
          case te::dt::UINT32_TYPE:
          {
            converter->add(i, newProperty, boost::bind(&terrama2::core::DataAccessor::stringToInt, this, _1, _2, _3));
            break;
          }
          default:
          {
            delete newProperty;

            te::dt::Property* defaultProperty = property->clone();
            defaultProperty->setName(alias);

            converter->add(i,defaultProperty);
            break;
          }
        }

        converted = true;
        break; // for(auto& field : fields)
      }
    }

    if(converted)
    {
      converter->remove(property->getName());
      continue;
    }

    if(convertAll)
    {
      std::string alias = DataAccessorTxtFile::simplifyString(property->getName());
      te::dt::Property* defaultProperty = property->clone();
      defaultProperty->setName(alias);

      converter->add(i,defaultProperty);
    }
    converter->remove(property->getName());
  }
}

