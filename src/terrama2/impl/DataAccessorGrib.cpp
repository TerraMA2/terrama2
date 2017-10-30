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
  \file terrama2/impl/DataAccessorGrib.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorGrib.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"

#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/Band.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/memory/DataSetItem.h>

#include <boost/range/algorithm/find_if.hpp>
#include <boost/algorithm/string.hpp>

 terrama2::core::DataAccessorGrib::DataAccessorGrib(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
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

 void terrama2::core::DataAccessorGrib::addToCompleteDataSet(DataSetPtr dataSet,
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

  teDataSet->moveBeforeFirst();
  while(teDataSet->moveNext())
  {
    std::shared_ptr<te::rst::Raster> gribRaster(teDataSet->isNull(rasterColumn) ? nullptr : teDataSet->getRaster(rasterColumn).release());
    auto raster = getRasterBand(dataSet, gribRaster);

    te::mem::DataSetItem* item = new te::mem::DataSetItem(completeDataSet.get());

    item->setRaster(rasterColumn, raster.release());
    if(isValidColumn(timestampColumn ))
      item->setDateTime(timestampColumn, fileTimestamp.get() ? static_cast<te::dt::DateTime*>(fileTimestamp->clone()) : nullptr);

    item->setString("filename", filename);
    completeDataSet->add(item);
  }
}

std::unique_ptr<te::rst::Raster> terrama2::core::DataAccessorGrib::getRasterBand(DataSetPtr dataSet, std::shared_ptr<te::rst::Raster> raster) const
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

std::set<int> terrama2::core::DataAccessorGrib::getBand(DataSetPtr dataSet, std::shared_ptr<te::rst::Raster> raster) const
{
  // get band value
  // if it's a numver, return the value
  // if it's a string find a band with property with that string and return the band index
  auto bandsStr = terrama2::core::getProperty(dataSet, dataSeries_, "bands");
  auto isoSurfaceStr = terrama2::core::getProperty(dataSet, dataSeries_, "iso_surface")+"-ISBL";

  // default band
  if(bandsStr.empty())
    return {0};

  std::vector<std::string> tokens;
  boost::split(tokens, bandsStr, boost::is_any_of(";"));

  std::set<int> bands;
  for(const auto& bandStr : tokens)
  {
    if(boost::range::find_if(bandStr, ::isdigit) != bandStr.end())
    {
      // it's a number, return the number
      bands.insert(std::stoi(bandStr));
    }
    else
    {
      // it's a string,  find a band with property with that string and return the band index
      for(uint bandIdx = 0; bandIdx < raster->getNumberOfBands(); ++bandIdx)
      {
        const te::rst::Band* rasterBand = raster->getBand(bandIdx);
        const auto property = rasterBand->getProperty();

        auto getThisVar = false;
        auto getThisSurface = false;

        const auto& map = property->m_metadata;
        for(uint i = 0; i < map.size(); ++i)
        {
          auto item = map.at(i);
          // check if the key value is GRIB_COMMENT or GRIB_ELEMENT
          // check if the value is the same as bandStr
          if((item.first == "GRIB_COMMENT" || item.first == "GRIB_ELEMENT") && item.second == bandStr)
          {
            getThisVar = true;
          }

          // check if we want this isobaric surface
          if(item.first == "GRIB_SHORT_NAME" && item.second == isoSurfaceStr)
          {
            getThisSurface = true;
          }

          // we found the band, no need to continue
          if(getThisVar && getThisSurface)
            break;
        }

        //add the band index
        if(getThisVar && getThisSurface)
          bands.insert(bandIdx);
      }
    }
  }

  //If no band was found, band 0 will be returned
  if(bands.empty())
  {
    QString errMsg = QObject::tr("No band with defined attributes in raster.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessException() << ErrorDescription(errMsg);
  }

  return bands;
}
