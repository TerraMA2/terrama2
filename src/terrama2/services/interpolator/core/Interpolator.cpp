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
#include <terralib/sam/kdtree.h>


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


terrama2::services::interpolator::core::Interpolator::Interpolator(terrama2::services::interpolator::core::InterpolatorParamsPtr params) :
  Process(),
  interpolationParams_(params)
{
  te::gm::Envelope env;
  tree_.reset(new InterpolatorTree(env));

  fillTree();
}

void terrama2::services::interpolator::core::Interpolator::fillTree()
{
  DataSeriesId dId = interpolationParams_->series;

  try
  {
    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto dataManager = std::make_shared<terrama2::services::interpolator::core::DataManager>();
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

    std::unique_ptr<terrama2::core::Filter> filter(interpolationParams_->filter.release());

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
    for (auto it : dataMap)
    {
      auto dataSet = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(it.first);
      auto dataSeries = it.second;

      tree_->insert(*dataSet->position.get(), dataSeries);
    }

//    /////////////////////////////////////////////////////////////////////////
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
  return RasterPtr();
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
