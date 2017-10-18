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
 * \file interpolator/core/Interpolator.cpp
 *
 * \author Frederico Augusto Bedê
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


/*!
 * \brief Fills the vector with empty data \a nneighbors times.
 *
 * \param[in,out] data The vector containing data.
 *
 * \param[in,out] dists The vector containing distances between the sample and the neighbor.
 *
 * \param nneighbors Number of neighbors to be used.
 */
void FillEmptyData(std::vector<terrama2::services::interpolator::core::InterpolatorData>& data,
                   std::vector<double>& dists, const size_t& nneighbors)
{
  data.clear();
  dists.clear();

  terrama2::services::interpolator::core::InterpolatorData d;
  te::gm::Point pt(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
  terrama2::core::DataSetSeries dset;

  d.pt_ = &pt;
  d.series_ = dset;

  data.resize(nneighbors, d);
  dists.resize(nneighbors, std::numeric_limits<double>::max());
}

/// End utilities functions section.
/*! -------------------------------*/

terrama2::services::interpolator::core::Interpolator::Interpolator(InterpolatorParamsPtr params) :
  Process()
{
  interpolationParams_ = params;
  te::gm::Envelope env;
  tree_.reset(new InterpolatorTree(env));
  tree_->setBucketSize(1);

  this->id = interpolationParams_->id_;
  this->serviceInstanceId = interpolationParams_->serviceInstanceId_;
  this->active = true;
}

terrama2::services::interpolator::core::Interpolator::~Interpolator()
{

}

te::rst::RasterPtr terrama2::services::interpolator::core::Interpolator::makeRaster()
{
  te::gm::Envelope* env = new te::gm::Envelope(interpolationParams_->bRect_);

  if(!env->isValid())
  {
    QString errMsg = QObject::tr("The bounding rect is not valid.");

    TERRAMA2_LOG_WARNING() << errMsg.toStdString();
    throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
  }


  te::rst::Raster* r;

  // Reseting the kd-tree
  /////////////////////////////////////////////////////////////////////////
  if(!tree_->isEmpty())
    tree_->clear();

  fillTree();
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //  Creating and configuring the output raster.
  double resolutionX = interpolationParams_->resolutionX_;
  double resolutionY = interpolationParams_->resolutionY_;
  int srid = interpolationParams_->srid_;

  te::rst::Grid* grid = new te::rst::Grid(resolutionX, resolutionY, env, srid);

  // Creating bands
  te::rst::BandProperty* bProp = new te::rst::BandProperty(0, te::dt::DOUBLE_TYPE, "");
  std::vector<te::rst::BandProperty*> vecBandProp(1, bProp);

  // Creating raster info
  std::map<std::string, std::string> conInfo;

  // create raster
  r = te::rst::RasterFactory::make("MEM", grid, vecBandProp, conInfo);
  /////////////////////////////////////////////////////////////////////////

  return te::rst::RasterPtr(r);
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

    terrama2::core::Filter filter = interpolationParams_->filter_;

    auto uriMap = dataAccessor->getFiles(filter, remover);

    auto dataMap = dataAccessor->getSeries(uriMap, filter, remover);

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

    auto imgBBox = interpolationParams_->bRect_;

    for (auto it : dataMap)
    {
      auto dataSet = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(it.first);
      auto dataSeries = it.second;
      auto pt1 = *(dataSet->position.get());
      InterpolatorData node;
      node.pt_ = &pt1;
      node.series_ = dataSeries;

      if(!node.isValid() || (!imgBBox.contains(*node.pt_->getMBR())))
        continue;

      auto pair = std::make_pair(pt1, node);
      dataSetVec.push_back(pair);

      pts.add(new te::gm::Point(pt1));
    }

    tree_->setMBR(*pts.getMBR());
    tree_->build(dataSetVec);

    if(!interpolationParams_->bRect_.isValid())
      interpolationParams_->bRect_ = tree_->getMBR();

    /////////////////////////////////////////////////////////////////////////
    TERRAMA2_LOG_INFO() << QObject::tr("Tree filled successfully.");
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << QObject::tr("Fail to build the kd-tree.");
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown error.");
    TERRAMA2_LOG_INFO() << QObject::tr("Fail to build the kd-tree!");
  }
}

terrama2::services::interpolator::core::NNInterpolator::NNInterpolator(InterpolatorParamsPtr params) :
  Interpolator(params)
{

}

te::rst::RasterPtr terrama2::services::interpolator::core::NNInterpolator::makeInterpolation()
{
  te::rst::RasterPtr r = makeRaster();
  /////////////////////////////////////////////////////////////////////////
  //  Making the interpolation using the nearest neighbor.
  te::gm::Point pt1;
  std::vector<InterpolatorData> res;
  std::vector<double> dist;
  unsigned int resolutionY = r->getGrid()->getNumberOfRows(),
      resolutionX = r->getGrid()->getNumberOfColumns();

  for(unsigned int row = 0; row < resolutionY; row++)
    for(unsigned int col = 0; col < resolutionX; col++)
    {
      double x,
          y;

      res.clear();
      dist.clear();

      r->getGrid()->gridToGeo(static_cast<double>(col), static_cast<double>(row), x, y);

      pt1.setX(x);
      pt1.setY(y);

      // Finding the nearest neighbor.
      FillEmptyData(res, dist, 1);
      tree_->nearestNeighborSearch(pt1, res, dist, 1);

      // Getting the attribute and setting the interpolation value
      terrama2::core::DataSetSeries ds = (res.begin())->series_;
      terrama2::core::SynchronizedDataSetPtr dSet = ds.syncDataSet;

      if(dSet->isNull(0, interpolationParams_->attributeName_))
        continue;

      double value = dSet->getDouble(0, interpolationParams_->attributeName_);

      r->setValue(col, row, value, 0);
    }
  /////////////////////////////////////////////////////////////////////////

  return r;
}

terrama2::services::interpolator::core::AvgDistInterpolator::AvgDistInterpolator(InterpolatorParamsPtr params) :
  Interpolator(params)
{

}

te::rst::RasterPtr terrama2::services::interpolator::core::AvgDistInterpolator::makeInterpolation()
{
  te::rst::RasterPtr r = makeRaster();

  /////////////////////////////////////////////////////////////////////////
  //  Making the interpolation using the nearest neighbor.
  te::gm::Point pt1;
  std::vector<InterpolatorData> res;
  std::vector<double> dist;
  unsigned int resolutionY = r->getGrid()->getNumberOfRows(),
      resolutionX = r->getGrid()->getNumberOfColumns();

  for(unsigned int row = 0; row < resolutionY; row++)
    for(unsigned int col = 0; col < resolutionX; col++)
    {
      double x,
          y;

      res.clear();
      dist.clear();

      r->getGrid()->gridToGeo(static_cast<double>(col), static_cast<double>(row), x, y);

      pt1.setX(x);
      pt1.setY(y);

      // Finding the nearest neighbor.
      FillEmptyData(res, dist, interpolationParams_->numNeighbors_);
      tree_->nearestNeighborSearch(pt1, res, dist, interpolationParams_->numNeighbors_);

      double value = 0;
      double numNeighs = 0;

      for(size_t i = 0; i < res.size(); i++)
      {
        // Getting the attribute and setting the interpolation value
        terrama2::core::DataSetSeries ds = res[i].series_;
        terrama2::core::SynchronizedDataSetPtr dSet = ds.syncDataSet;

        if(!res[i].isValid())
          continue;

        numNeighs++;

        value += dSet->getDouble(0, interpolationParams_->attributeName_);
      }

      r->setValue(col, row, value / numNeighs, 0);
    }
  /////////////////////////////////////////////////////////////////////////

  return r;
}


terrama2::services::interpolator::core::SqrAvgDistInterpolator::SqrAvgDistInterpolator(InterpolatorParamsPtr params) :
  Interpolator(params)
{

}

te::rst::RasterPtr terrama2::services::interpolator::core::SqrAvgDistInterpolator::makeInterpolation()
{
  te::rst::RasterPtr r = makeRaster();

  /////////////////////////////////////////////////////////////////////////
  //  Making the interpolation using the nearest neighbor.
  te::gm::Point pt1;
  std::vector<InterpolatorData> res;
  std::vector<double> dist;
  unsigned int resolutionY = r->getGrid()->getNumberOfRows(),
      resolutionX = r->getGrid()->getNumberOfColumns();

  SqrAvgDistInterpolatorParams* auxPar = dynamic_cast<SqrAvgDistInterpolatorParams*>(interpolationParams_.get());

  unsigned int powF = auxPar->pow_;

  for(unsigned int row = 0; row < resolutionY; row++)
    for(unsigned int col = 0; col < resolutionX; col++)
    {
      double x,
          y;

      res.clear();
      dist.clear();

      r->getGrid()->gridToGeo(static_cast<double>(col), static_cast<double>(row), x, y);

      pt1.setX(x);
      pt1.setY(y);

      // Finding the nearest neighbor.
      FillEmptyData(res, dist, interpolationParams_->numNeighbors_);
      tree_->nearestNeighborSearch(pt1, res, dist, interpolationParams_->numNeighbors_);

      double value = 0;
      double den = 0;

      for(size_t i = 0; i < res.size(); i++)
      {
        // Getting the attribute and setting the interpolation value
        terrama2::core::DataSetSeries ds = res[i].series_;
        terrama2::core::SynchronizedDataSetPtr dSet = ds.syncDataSet;

        if(!res[i].isValid())
          continue;

        double wi = 1 / std::pow(std::sqrt(dist[i]), powF);

        value += wi * dSet->getDouble(0, interpolationParams_->attributeName_);
        den += wi;
      }

      r->setValue(col, row, value / den, 0);
    }
  /////////////////////////////////////////////////////////////////////////

  return r;
}
