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
  \file terrama2/core/impl/DataAccessorWildFireEvent.cpp

  \brief DataAccessor class for static data accessed via OGR driver.

  \author Paulo R. M. Oliveira
 */

//TerraMA2
#include "DataAccessorWildFireEvent.hpp"

#include "../core/Exception.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/DataRetrieverFactory.hpp"

#include <terralib/datatype/DateTimeProperty.h>

// QT
#include <QObject>

#include <boost/algorithm/string/replace.hpp>

terrama2::core::DataAccessorWildFireEvent::DataAccessorWildFireEvent(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
: DataAccessor(dataProvider, dataSeries),
  DataAccessorGeometricObjectOGR(dataProvider, dataSeries, false)
{
 if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
 {
   QString errMsg = QObject::tr("Wrong DataSeries semantics.");
   TERRAMA2_LOG_ERROR() << errMsg;
   throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
 }
}

void terrama2::core::DataAccessorWildFireEvent::adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const
{
  //only one timestamp column
  std::string timestampPropertyName = getTimestampPropertyName(dataSet);
  std::string outputTimestampPropertyName = getOutputTimestampPropertyName(dataSet);

  std::string geometryPropertyName = getGeometryPropertyName(dataSet);
  std::string outputGeometryPropertyName = getOutputGeometryPropertyName(dataSet);

  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty(outputTimestampPropertyName, te::dt::TIME_INSTANT_TZ);
  auto timezone = DataAccessorFile::getTimeZone(dataSet);
  auto timestampMask = getTimestampMask(dataSet);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == timestampPropertyName)
    {
      // datetime column found
      converter->add(i, dtProperty, [timezone, timestampMask](te::da::DataSet* teDataset, const std::vector<std::size_t>& indexes, int dstType)
      {
        return terrama2::core::DataAccessorGeometricObjectOGR::numberToTimestamp(teDataset, indexes, dstType, timezone, timestampMask);
      });
    }
    else if(property->getName() == geometryPropertyName)
    {
      auto geomProperty = property->clone();
      geomProperty->setName(outputGeometryPropertyName);
      converter->add(i, geomProperty);
    }
    else
    {
      // future date field, not currently used
      if(property->getName() == "dt")
        continue;

      converter->add(i,property->clone());
    }
  }
}

void terrama2::core::DataAccessorWildFireEvent::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> /*converter*/, const std::shared_ptr<te::da::DataSetType>& /*datasetType*/) const
{
  //columns add by the adapt method
}
