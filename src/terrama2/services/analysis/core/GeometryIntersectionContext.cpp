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
  \file terrama2/services/analysis/core/GeometryIntersectionContext.cpp

  \brief Contains occurrence analysis contextDataSeries.
*/

#include "GeometryIntersectionContext.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>

std::string getIdentifierProperty(const terrama2::services::analysis::core::AnalysisDataSeries& analysisDataSeries)
{
  std::string identifier;
  try
  {
    identifier = analysisDataSeries.metadata.at("identifier");
  }
  catch (...)
  {
    /* code */
  }

  return identifier;
}


terrama2::services::analysis::core::GeometryIntersectionContext::GeometryIntersectionContext(terrama2::services::analysis::core::DataManagerPtr dataManager,
                                                                                             terrama2::services::analysis::core::AnalysisPtr analysis,
                                                                                             std::shared_ptr<te::dt::TimeInstantTZ> startTime)
  : BaseContext(dataManager, analysis, startTime)
{

}

void terrama2::services::analysis::core::GeometryIntersectionContext::load()
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto dataManagerPtr = dataManager_.lock();
  if(!dataManagerPtr)
  {
    QString errMsg(QObject::tr("Invalid data manager."));
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
  }

  auto analysis = getAnalysis();

  for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    auto dataSeriesPtr = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
    auto datasets = dataSeriesPtr->datasetList;
    // Static Data Series (Monitored)
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      if(analysis->type == AnalysisType::GEOMETRIC_INTERSECTION_TYPE)
      {
        assert(datasets.size() == 1);
        auto dataset = datasets[0];

        auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
        terrama2::core::Filter filter;
        filter.discardAfter = getStartTime();
        filter.lastValues = std::make_shared<size_t>(1);

        //accessing data
        terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
        auto seriesMap = accessor->getSeries(filter, remover_);
        auto series = seriesMap[dataset];

        std::string identifier = getIdentifierProperty(analysisDataSeries);

        auto dataSeriesContext = std::make_shared<GeoIntersectionDataSeries>();

        if(!series.syncDataSet)
        {
          QString errMsg(QObject::tr("No data available for DataSeries %1").arg(dataSeriesPtr->id));
          throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
        }

        if(!series.syncDataSet->dataset())
        {
          QString errMsg(QObject::tr("Adding an invalid dataset to the analysis context: DataSeries %1").arg(dataSeriesPtr->id));
          throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
        }

        std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

        dataSeriesContext->series = series;
        dataSeriesContext->identifier = identifier;
        dataSeriesContext->geometryPos = geomPropertyPosition;

        staticDataSeries_ = dataSeriesContext;
      } // endif(analysis->type == AnalysisType::GEOMETRIC_INTERSECTION_TYPE)
    }
    else // Dynamic Data Series
    {
      assert(datasets.size() == 1);
      auto dataset = datasets[0];

      auto dataProvider = dataManagerPtr->findDataProvider(dataSeriesPtr->dataProviderId);
      terrama2::core::Filter filter;
      filter.discardAfter = getStartTime();
      filter.lastValues = std::make_shared<size_t>(1);

      //accessing data
      terrama2::core::DataAccessorPtr accessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeriesPtr);
      auto seriesMap = accessor->getSeries(filter, remover_);
      auto series = seriesMap[dataset];

      std::string identifier = getIdentifierProperty(analysisDataSeries);

      auto dataSeriesContext = std::make_shared<GeoIntersectionDataSeries>();

      if(!series.syncDataSet)
      {
        QString errMsg(QObject::tr("No data available for DataSeries %1").arg(dataSeriesPtr->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      if(!series.syncDataSet->dataset())
      {
        QString errMsg(QObject::tr("Adding an invalid dataset to the analysis context: DataSeries %1").arg(dataSeriesPtr->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      std::size_t geomPropertyPosition = te::da::GetFirstPropertyPos(series.syncDataSet->dataset().get(), te::dt::GEOMETRY_TYPE);

      dataSeriesContext->series = series;
      dataSeriesContext->identifier = identifier;
      dataSeriesContext->geometryPos = geomPropertyPosition;

      dynamicDataSeries_ = dataSeriesContext;
    }
  }
}

std::shared_ptr<terrama2::services::analysis::core::GeoIntersectionDataSeries> terrama2::services::analysis::core::GeometryIntersectionContext::getStaticDataSeries()
{
  return staticDataSeries_;
}
