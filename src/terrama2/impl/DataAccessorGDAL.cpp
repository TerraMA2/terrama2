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
  \file terrama2/core/data-access/DataAccessorGDAL.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorGDAL.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/Verify.hpp"

//TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/raster/Grid.h>

//QT
#include <QString>
#include <QObject>
#include <QFileInfo>

terrama2::core::DataAccessorGDAL::DataAccessorGDAL(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
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

std::string terrama2::core::DataAccessorGDAL::dataSourceType() const { return "GDAL"; }

std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorGDAL::createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const
{
  return DataAccessorFile::internalCreateCompleteDataSet(dataSetType, true, true);
}

void terrama2::core::DataAccessorGDAL::addToCompleteDataSet(terrama2::core::DataSetPtr dataSet,
                                                               std::shared_ptr<te::mem::DataSet> completeDataSet,
                                                               std::shared_ptr<te::da::DataSet> teDataSet,
                                                               std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp,
                                                               const std::string& filename) const
{
  completeDataSet->moveLast();

  size_t rasterColumn = te::da::GetFirstPropertyPos(teDataSet.get(), te::dt::RASTER_TYPE);
  if(!isValidColumn(rasterColumn))
  {
    QString errMsg = QObject::tr("No raster attribute.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  size_t timestampColumn = te::da::GetFirstPropertyPos(completeDataSet.get(), te::dt::DATETIME_TYPE);

  int outputSrid = -1;
  try
  {
    outputSrid = getSrid(dataSet, false);
    verify::srid(outputSrid);
  }
  catch (const UndefinedTagException&)
  {
    //SRID is an optional parameter
  }

  teDataSet->moveBeforeFirst();
  while(teDataSet->moveNext())
  {
    std::unique_ptr<te::rst::Raster> raster(teDataSet->isNull(rasterColumn) ? nullptr : teDataSet->getRaster(rasterColumn).release());

    if(outputSrid > 0 && outputSrid != raster->getSRID())
    {
      try
      {
        verify::srid(raster->getSRID(), false);
        std::map<std::string, std::string> map{{"FORCE_MEM_DRIVER", "TRUE"}};
        auto temp = raster->transform(outputSrid, map);
        if(!temp)
        {
          QString errMsg = QObject::tr("Null raster found.\nError during transform.");
          TERRAMA2_LOG_ERROR() << errMsg;
          continue;
        }
        else
          raster.reset(temp);
      }
      catch (...)
      {
        auto grid = raster->getGrid();
        grid->setSRID(outputSrid);
      }
    }

    te::mem::DataSetItem* item = new te::mem::DataSetItem(completeDataSet.get());

    item->setRaster(rasterColumn, raster.release());
    if(isValidColumn(timestampColumn ))
      item->setDateTime(timestampColumn, fileTimestamp.get() ? static_cast<te::dt::DateTime*>(fileTimestamp->clone()) : nullptr);

    item->setString("filename", filename);
    completeDataSet->add(item);
  }
}
