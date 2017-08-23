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
  \file interpolator/core/Interpolator.cpp

  \author Frederico Augusto Bedê
*/

#include "Interpolator.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/Typedef.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../core/utility/Logger.hpp"

#include "DataManager.hpp"
#include "Typedef.hpp"

// TerraLib
#include <terralib/geometry/Point.h>
#include <terralib/geometry/MultiPoint.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/sam/kdtree.h>

// STL
#include <utility> //std::pair, std::make_pair


#define BICUBIC_MODULE( x ) ( ( x < 0 ) ? ( -1 * x ) : x )
#define BICUBIC_K1( x , a ) ( ( ( a + 2 ) * x * x * x ) - \
  ( ( a + 3 ) * x * x ) + 1 )
#define BICUBIC_K2( x , a ) ( ( a * x * x * x ) - ( 5 * a * x * x ) + \
  ( 8 * a * x ) - ( 4 * a ) )
#define BICUBIC_RANGES(x,a) \
  ( ( ( 0 <= x ) && ( x <= 1 ) ) ? \
  BICUBIC_K1(x,a) \
  : ( ( ( 1 < x ) && ( x <= 2 ) ) ? \
  BICUBIC_K2(x,a) \
  : 0 ) )
#define BICUBIC_KERNEL( x , a ) BICUBIC_RANGES( BICUBIC_MODULE(x) , a )


void FillEmptyData(std::vector<terrama2::services::interpolator::core::InterpolatorData>& data,
                   std::vector<double>& dists, const unsigned int& nneighbors)
{
  data.clear();
  dists.clear();

  terrama2::services::interpolator::core::InterpolatorData d;
  te::gm::Point pt(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
  terrama2::core::DataSetSeries dset;

  d.pt_ = &pt;
  d.series_ = dset;

  data.resize(1, d);
  dists.resize(1, std::numeric_limits<double>::max());
}


terrama2::services::interpolator::core::Interpolator::Interpolator(terrama2::services::interpolator::core::InterpolatorParamsPtr params) :
  Process(),
  interpolationParams_(params)
{
  te::gm::Envelope env;
  tree_.reset(new InterpolatorTree(env));
  tree_->setBucketSize(1);

  this->id = interpolationParams_->id_;
  this->serviceInstanceId = interpolationParams_->serviceInstanceId_;
  this->active = true;
}

void terrama2::services::interpolator::core::Interpolator::fillTree()
{
  DataSeriesId dId = interpolationParams_->series_;

  try
  {
    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto dataManager = interpolationParams_->dataManager_;
    auto lock = dataManager->getLock();

    // input data
    auto inputDataSeries = dataManager->findDataSeries(dId);

    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting creating kd-tree for '%1'").arg(inputDataSeries->name.c_str());

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    //  recovering data
    auto provider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

    auto remover = std::make_shared<terrama2::core::FileRemover>();

    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(provider, inputDataSeries);

    std::unique_ptr<terrama2::core::Filter> filter(interpolationParams_->filter_.release());

    auto uriMap = dataAccessor->getFiles(*filter.get(), remover);

    auto dataMap = dataAccessor->getSeries(uriMap, *filter.get(), remover);

    if(dataMap.empty())
    {
      QString errMsg = QObject::tr("No data to fullfill the tree.");
      TERRAMA2_LOG_WARNING() << errMsg;
      return;
    }

    /////////////////////////////////////////////////////////////////////////
    //  building the kd-tree

    te::gm::MultiPoint pts(0, te::gm::MultiPointType, interpolationParams_->srid_);

    std::vector< std::pair<te::gm::Point, InterpolatorData> > dataSetVec;

    for (auto it : dataMap)
    {
      auto dataSet = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(it.first);
      auto dataSeries = it.second;
      auto pt1 = *(dataSet->position.get());
      InterpolatorData node;
      node.pt_ = &pt1;
      node.series_ = dataSeries;
      auto pair = std::make_pair(pt1, node);
      dataSetVec.push_back(pair);

      pts.add(new te::gm::Point(pt1));
    }

    tree_->setMBR(*pts.getMBR());
    tree_->build(dataSetVec);

    interpolationParams_->bRect_ = tree_->getMBR();

    /////////////////////////////////////////////////////////////////////////
    TERRAMA2_LOG_INFO() << QObject::tr("Tree filled successfully.");
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << QObject::tr("Fail to build the kd-tree!");

    //    if(executionPackage.registerId != 0)
    //    {
    //      logger->log(CollectorLogger::ERROR_MESSAGE, e.what(), executionPackage.registerId);
    //      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
    //    }
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Unknown error.");
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << QObject::tr("Fail to build the kd-tree!");

    //    if(executionPackage.registerId != 0)
    //    {
    //      logger->log(CollectorLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    //      logger->result(CollectorLogger::ERROR, nullptr, executionPackage.registerId);
    //    }

  }
}

terrama2::services::interpolator::core::NNInterpolator::NNInterpolator(terrama2::services::interpolator::core::InterpolatorParamsPtr params) :
  Interpolator(params)
{

}

terrama2::services::interpolator::core::RasterPtr terrama2::services::interpolator::core::NNInterpolator::makeInterpolation()
{
  RasterPtr r;

  // Reseting the kd-tree
  /////////////////////////////////////////////////////////////////////////
  if(!tree_->isEmpty())
    tree_->clear();

  fillTree();
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //  Creating and configuring the output raster.

  unsigned int resolutionX = interpolationParams_->resolutionX_;
  unsigned int resolutionY = interpolationParams_->resolutionY_;
  int srid = interpolationParams_->srid_;

  // For testing assume the bounding rect calculated by kd-tree
  //  te::gm::Envelope* env = new te::gm::Envelope(interpolationParams_->bRect_);
  te::gm::Envelope* env = new te::gm::Envelope(interpolationParams_->bRect_);

  te::rst::Grid* grid = new te::rst::Grid(resolutionX, resolutionY, env, srid);

  // Creating bands
  te::rst::BandProperty* bProp = new te::rst::BandProperty(0, te::dt::DOUBLE_TYPE, "");
  std::vector<te::rst::BandProperty*> vecBandProp(1, bProp);

  // Creating raster info
  std::map<std::string, std::string> conInfo;
  conInfo["URI"] = interpolationParams_->fileName_;

  // create raster
  r.reset(te::rst::RasterFactory::make("GDAL", grid, vecBandProp, conInfo));
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //  Making the interpolation using the nearest neighbor.
  te::gm::Point pt1;
  std::vector<InterpolatorData> res;
  std::vector<double> dist;

  for(unsigned int row = 0; row < resolutionY; row++)
    for(unsigned int col = 0; col < resolutionX; col++)
    {
      double x,
          y;

      res.clear();
      dist.clear();

      r->getGrid()->gridToGeo((double) col, (double)row, x, y);

      pt1.setX(x);
      pt1.setY(y);

      // Finding the nearest neighbor.
      FillEmptyData(res, dist, 1);
      tree_->nearestNeighborSearch(pt1, res, dist, 1);

      // Getting the attribute and setting the interpolation value
      terrama2::core::DataSetSeries ds = (res.begin())->series_;
      terrama2::core::SynchronizedDataSetPtr dSet = ds.syncDataSet;

      double value = dSet->getDouble(0, interpolationParams_->attributeName_);

      r->setValue(col, row, value, 0);
    }
  /////////////////////////////////////////////////////////////////////////

  return r;
}

terrama2::services::interpolator::core::BLInterpolator::BLInterpolator(terrama2::services::interpolator::core::InterpolatorParamsPtr params) :
  Interpolator(params)
{

}

terrama2::services::interpolator::core::RasterPtr terrama2::services::interpolator::core::BLInterpolator::makeInterpolation()
{
  return RasterPtr();
}


terrama2::services::interpolator::core::BCInterpolator::BCInterpolator(terrama2::services::interpolator::core::InterpolatorParamsPtr params) :
  Interpolator(params)
{

}

terrama2::services::interpolator::core::RasterPtr terrama2::services::interpolator::core::BCInterpolator::makeInterpolation()
{
  return RasterPtr();
}
