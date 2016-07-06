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
  \file src/terrama2/services/view/core/View.cpp

  \brief View service functions

  \author Vinicius Campanha
*/

// STL
#include <cmath>

// TerraLib
#include <terralib/common/StringUtils.h>
#include <terralib/qt/widgets/canvas/Canvas.h>

// TerraMA2
#include "View.hpp"
#include "ViewStyle.hpp"
#include "MemoryDataSetLayer.hpp"
#include "DataManager.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"

void terrama2::services::view::core::makeView(ViewId viewId, std::shared_ptr< terrama2::services::view::core::ViewLogger > logger, std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access DataManager");
    return;
  }

  try
  {
    RegisterId logId = 0;
    if(logger.get())
      logId = logger->start(viewId);

    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting view %1 generation.").arg(viewId);

    auto lock = dataManager->getLock();

    auto viewPtr = dataManager->findView(viewId);

    // VINICIUS: filter for each dataSeries
    std::unordered_map< terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr > inputDataSeriesList;

    for(uint32_t dataSeriesId : viewPtr->dataSeriesList)
    {
      terrama2::core::DataSeriesPtr inputDataSeries = dataManager->findDataSeries(dataSeriesId);
      terrama2::core::DataProviderPtr inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

      inputDataSeriesList.emplace(inputDataSeries, inputDataProvider);
    }

    lock.unlock();

    std::vector<std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries>> seriesList;

    for(auto inputDataSeries : inputDataSeriesList)
    {
      auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataSeries.second, inputDataSeries.first);

      // VINICIUS: filter, date?
      terrama2::core::Filter filter;

      std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > series = dataAccessor->getSeries(filter);

      seriesList.push_back(series);
    }

    drawSeriesList(seriesList, viewPtr->resolutionWidth, viewPtr->resolutionHeight);

    TERRAMA2_LOG_INFO() << QObject::tr("View %1 generated successfully.").arg(viewId);

    if(logger.get())
      logger->done(terrama2::core::TimeUtils::nowUTC(), logId);
  }
  catch(const terrama2::Exception&)
  {
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << QObject::tr("Build of %1 finished with error(s).").arg(viewId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unkown error.");
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);
  }
}


void terrama2::services::view::core::drawSeriesList(std::vector<std::map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries>>& seriesList, uint32_t resolutionWidth, uint32_t resolutionHeigth)
{
  std::vector< std::shared_ptr<te::map::MemoryDataSetLayer> > layersList;
  uint32_t layerID = 0;

  // Create layers from series
  for(auto& serie : seriesList)
  {
    std::shared_ptr<te::da::DataSet> dataSet = serie.begin()->second.syncDataSet->dataset();
    std::shared_ptr<te::da::DataSetType> teDataSetType = serie.begin()->second.teDataSetType;

    // TODO: A terralib dataset can have a geom and raster at same time?
    if(teDataSetType->hasRaster())
    {
      std::size_t rpos = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::RASTER_TYPE);

      dataSet->moveFirst();
      auto raster(dataSet->getRaster(rpos));

      te::gm::Envelope* extent = raster->getExtent();

      // Creates a DataSetLayer of raster
      std::shared_ptr<te::map::MemoryDataSetLayer> rasterLayer(new te::map::MemoryDataSetLayer(te::common::Convert2String(++layerID), raster->getName(), dataSet, teDataSetType));
      rasterLayer->setDataSetName(teDataSetType->getDatasetName());
      rasterLayer->setExtent(*extent);
      rasterLayer->setRendererType("ABSTRACT_LAYER_RENDERER");
      rasterLayer->setSRID(raster->getSRID());

      // VINICIUS: Set Style
      MONO_0_Style(rasterLayer);

      layersList.push_back(rasterLayer);
    }

    if(teDataSetType->hasGeom())
    {
      auto geomProperty = te::da::GetFirstGeomProperty(teDataSetType.get());

      dataSet->moveFirst();

      std::shared_ptr< te::gm::Envelope > extent(dataSet->getExtent(teDataSetType->getPropertyPosition(geomProperty)));

      // Creates a Layer
      std::shared_ptr< te::map::MemoryDataSetLayer > geomLayer(new te::map::MemoryDataSetLayer(te::common::Convert2String(++layerID), geomProperty->getName(), dataSet, teDataSetType));
      geomLayer->setDataSetName(teDataSetType->getName());
      geomLayer->setVisibility(te::map::VISIBLE);
      geomLayer->setExtent(*extent);
      geomLayer->setStyle(CreateFeatureTypeStyle(geomProperty->getGeometryType()));
      geomLayer->setRendererType("ABSTRACT_LAYER_RENDERER");
      geomLayer->setSRID(geomProperty->getSRID());

      // VINICIUS: set style

      layersList.push_back(geomLayer);
    }
  }

  // Draw layers

  te::gm::Envelope extent;
  int srid = 0;

  for(auto& layer : layersList)
  {
    if(!extent.isValid())
      extent = layer->getExtent();
    else
      extent.Union(layer->getExtent());

    // VINICIUS: which SRID use? using from the first layer for now
    if(srid == 0)
      srid = layer->getSRID();
  }

  // Creates a canvas
  double llx = extent.m_llx;
  double lly = extent.m_lly;
  double urx = extent.m_urx;
  double ury = extent.m_ury;

  std::unique_ptr<te::qt::widgets::Canvas> canvas(new te::qt::widgets::Canvas(resolutionWidth, resolutionHeigth));
  canvas->calcAspectRatio(llx, lly, urx, ury);
  canvas->setWindow(llx, lly, urx, ury);
  canvas->setBackgroundColor(te::color::RGBAColor(255, 255, 255, TE_OPAQUE));

  bool cancel = false;

  for(auto& layer : layersList)
  {
    layer->draw(canvas.get(), extent, srid, 0, &cancel);
  }

  // Save view

  canvas->save("GeneretadImage", te::map::PNG);

  canvas->clear();
}


