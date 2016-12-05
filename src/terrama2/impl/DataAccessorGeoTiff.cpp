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
  \file terrama2/core/data-access/DataAccessorGeoTiff.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorGeoTiff.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"

//TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>

//QT
#include <QString>
#include <QObject>
#include <QFileInfo>

terrama2::core::DataAccessorGeoTiff::DataAccessorGeoTiff(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
 : DataAccessor(dataProvider, dataSeries, false),
   DataAccessorGrid(dataProvider, dataSeries, false),
   DataAccessorFile(dataProvider, dataSeries, false)
{
  if(checkSemantics && dataSeries->semantics.code != dataAccessorType())
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorGeoTiff::dataSourceType() const { return "GDAL"; }

std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorGeoTiff::createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const
{
  return DataAccessorFile::internalCreateCompleteDataSet(dataSetType, true, true);
}

void terrama2::core::DataAccessorGeoTiff::addToCompleteDataSet(std::shared_ptr<te::mem::DataSet> completeDataSet,
                                                               std::shared_ptr<te::da::DataSet> dataSet,
                                                               std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp,
                                                               const std::string& filename) const
{
  completeDataSet->moveLast();

  size_t rasterColumn = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::RASTER_TYPE);
  if(!isValidColumn(rasterColumn))
  {
    QString errMsg = QObject::tr("No raster attribute.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  size_t timestampColumn = te::da::GetFirstPropertyPos(completeDataSet.get(), te::dt::DATETIME_TYPE);

  dataSet->moveBeforeFirst();
  while(dataSet->moveNext())
  {
    std::unique_ptr<te::rst::Raster> raster(dataSet->isNull(rasterColumn) ? nullptr : dataSet->getRaster(rasterColumn).release());

    te::mem::DataSetItem* item = new te::mem::DataSetItem(completeDataSet.get());

    item->setRaster(rasterColumn, raster.release());
    if(isValidColumn(timestampColumn ))
      item->setDateTime(timestampColumn, fileTimestamp.get() ? static_cast<te::dt::DateTime*>(fileTimestamp->clone()) : nullptr);

    item->setString("filename", filename);
    completeDataSet->add(item);
  }
}
