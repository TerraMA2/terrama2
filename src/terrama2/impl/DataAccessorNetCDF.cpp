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
  \file terrama2/impl/DataAccessorNetCDF.cpp

 */

#include "DataAccessorNetCDF.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/Verify.hpp"

#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/Band.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/memory/DataSetItem.h>

#include <boost/range/algorithm/find_if.hpp>
#include <boost/algorithm/string.hpp>

#include <memory>
#include <QDebug>

 terrama2::core::DataAccessorNetCDF::DataAccessorNetCDF(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
  : DataAccessor(dataProvider, dataSeries),
    DataAccessorGDAL(dataProvider, dataSeries, false)
 {
   if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
   {
     QString errMsg = QObject::tr("Wrong DataSeries semantics.");
     TERRAMA2_LOG_ERROR() << errMsg;
     throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
   }
 }

void terrama2::core::DataAccessorNetCDF::addToCompleteDataSet(DataSetPtr dataSet,
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
    throw DataAccessException() << ErrorDescription(errMsg);
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
    std::shared_ptr<te::rst::Raster> ncRaster(teDataSet->isNull(rasterColumn) ? nullptr : teDataSet->getRaster(rasterColumn).release());

    auto raster = getRasterBand(dataSet, ncRaster);

    std::unique_ptr<te::mem::DataSetItem> item (new te::mem::DataSetItem(completeDataSet.get()));

    if(outputSrid > 0 && outputSrid != ncRaster->getSRID())
    {
      try
      {
        verify::srid(raster->getSRID(), false);
        std::map<std::string, std::string> map{{"RTYPE", "EXPANSIBLE"}};
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

    item->setRaster(rasterColumn, raster.release());

    if(isValidColumn(timestampColumn))
      item->setDateTime(timestampColumn, fileTimestamp.get() ? static_cast<te::dt::DateTime*>(fileTimestamp->clone()) : nullptr);

    item->setString("filename", filename);
    completeDataSet->add(item.release());
  }
}

std::unique_ptr<te::rst::Raster> terrama2::core::DataAccessorNetCDF::getRasterBand(DataSetPtr dataSet, std::shared_ptr<te::rst::Raster> raster) const
{
  auto bandList = getBand(dataSet, raster);
  std::vector<te::rst::BandProperty*> bands;
  for(const auto& bandIdx : bandList)
    bands.push_back(new te::rst::BandProperty(*raster->getBand(bandIdx)->getProperty()));

  auto grid = new te::rst::Grid(raster->getNumberOfColumns(), raster->getNumberOfRows(), new te::gm::Envelope(*raster->getExtent()), raster->getSRID());
  std::unique_ptr<te::rst::Raster> expansible(te::rst::RasterFactory::make("EXPANSIBLE", grid, bands, {}));

  uint newBand = 0;
  for(const auto& bandIdx : bandList)
  {
    const te::rst::Band* rasterBand = raster->getBand(bandIdx);
    te::rst::Band* expansibleBand = expansible->getBand(newBand);

    const int nblocksX = rasterBand->getProperty()->m_nblocksx;
    const int nblocksY = rasterBand->getProperty()->m_nblocksy;
    int blkYIdx = 0;

    for( int blkXIdx = 0 ; blkXIdx < nblocksX ; ++blkXIdx )
    {
      for( blkYIdx = 0 ; blkYIdx < nblocksY ; ++blkYIdx )
      {
        std::unique_ptr<unsigned char[]> buffer(new unsigned char[rasterBand->getBlockSize()]);
        rasterBand->read( blkXIdx, blkYIdx, buffer.get());
        expansibleBand->write( blkXIdx, blkYIdx, buffer.get());
      }
    }

    ++newBand;
  }
  return expansible;
}

std::set<int> terrama2::core::DataAccessorNetCDF::getBand(DataSetPtr dataSet, std::shared_ptr<te::rst::Raster> raster) const
{
  return {0};
}
