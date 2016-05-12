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
  \file terrama2/core/data-access/DataAccessorGeoTif.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorGeoTif.hpp"
#include "../core/utility/Logger.hpp"

//TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/memory/DataSetItem.h>

//QT
#include <QString>
#include <QObject>
#include <QFileInfo>

terrama2::core::DataAccessorGeoTif::DataAccessorGeoTif(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const Filter& filter)
 : DataAccessor(dataProvider, dataSeries, filter),
   DataAccessorGrid(dataProvider, dataSeries, filter),
   DataAccessorFile(dataProvider, dataSeries, filter)
{
  if(dataSeries->semantics.code != "GRID-geotif")
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);;
  }
}

std::string terrama2::core::DataAccessorGeoTif::dataSourceType() const { return "GDAL"; }

std::shared_ptr<te::da::DataSet> terrama2::core::DataAccessorGeoTif::createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const
{
  te::dt::Property* timestamp = new te::dt::DateTimeProperty("file_timestamp", te::dt::TIME_INSTANT_TZ);
  dataSetType->add(timestamp);
  return std::make_shared<te::mem::DataSet>(dataSetType.get());
}

void terrama2::core::DataAccessorGeoTif::addToCompleteDataSet(std::shared_ptr<te::da::DataSet> completeDataSet,
                                                               std::shared_ptr<te::da::DataSet> dataSet,
                                                               std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp) const
{
  auto complete = std::dynamic_pointer_cast<te::mem::DataSet>(completeDataSet);
  complete->moveLast();

  int rasterColumn = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::RASTER_TYPE);
  if(rasterColumn == -1)
  {
    QString errMsg = QObject::tr("No raster attribute.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  int timestampColumn = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::DATETIME_TYPE);

  dataSet->moveBeforeFirst();
  while(dataSet->moveNext())
  {
    std::unique_ptr<te::rst::Raster> raster(dataSet->isNull(rasterColumn) ? nullptr : dataSet->getRaster(rasterColumn).release());
    auto info = raster->getInfo();
    QFileInfo fileInfo(QString::fromStdString(info["URI"]));
    raster->setName(fileInfo.baseName().toStdString());

    te::mem::DataSetItem* item = new te::mem::DataSetItem(complete.get());

    item->setRaster(rasterColumn, raster.release());
    if(timestampColumn != -1)
      item->setDateTime(timestampColumn, fileTimestamp.get() ? static_cast<te::dt::DateTime*>(fileTimestamp->clone()) : nullptr);
      
    complete->add(item);
  }
}
