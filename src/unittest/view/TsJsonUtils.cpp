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
  \file unittest/view/TsJsonUtils.cpp

  \brief Tests for View JsonUtils class

  \author Vinicius Campanha
*/

#include <terralib/se.h>

//TerraMA2
#include "TsJsonUtils.hpp"

#include <terrama2/services/view/core/JSonUtils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>


te::se::Stroke* CreateStroke(te::se::Graphic* graphicFill,
                             const std::string& width, const std::string& opacity,
                             const std::string& dasharray, const std::string& linecap, const std::string& linejoin)
{
  te::se::Stroke* stroke = new te::se::Stroke;

  if(graphicFill)
    stroke->setGraphicFill(graphicFill);

  if(!width.empty())
    stroke->setWidth(width);

  if(!opacity.empty())
    stroke->setOpacity(opacity);

  if(!dasharray.empty())
    stroke->setDashArray(dasharray);

  if(!linecap.empty())
    stroke->setLineCap(linecap);

  if(!linejoin.empty())
    stroke->setLineJoin(linecap);

  return stroke;
}

te::se::Stroke* CreateStroke(const std::string& color, const std::string& width,
                             const std::string& opacity, const std::string& dasharray,
                             const std::string& linecap, const std::string& linejoin)
{
  te::se::Stroke* stroke = CreateStroke(0, width, opacity, dasharray, linecap, linejoin);

  if(!color.empty())
    stroke->setColor(color);

  return stroke;
}

te::se::Fill* CreateFill(const std::string& color, const std::string& opacity)
{
  te::se::Fill* fill = new te::se::Fill;

  if(!color.empty())
    fill->setColor(color);

  if(!opacity.empty())
    fill->setOpacity(opacity);

  return fill;
}

te::se::Symbolizer* CreatePolygonSymbolizer()
{
  te::se::Fill* fill = CreateFill("#5e5eeb", "100");
  te::se::Stroke* stroke = CreateStroke("#000000", "1", "", "", "", "");
  te::se::PolygonSymbolizer* symbolizer = new te::se::PolygonSymbolizer;
  symbolizer->setFill(fill);
  symbolizer->setStroke(stroke);
  return symbolizer;
}

te::se::Symbolizer* CreateLineSymbolizer()
{
  te::se::Stroke* stroke = CreateStroke("#000000", "1", "", "", "", "");
  te::se::LineSymbolizer* symbolizer = new te::se::LineSymbolizer;
  symbolizer->setStroke(stroke);
  return symbolizer;
}

void TsJsonUtils::testGoNBackJSon()
{
  try
  {
    terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
    terrama2::services::view::core::ViewPtr viewPtr(view);

    view->id = 1;
    view->projectId = 1;
    view->serviceInstanceId = 1;
    view->active = true;
    view->resolutionWidth = 800;
    view->resolutionHeight = 600;
    view->srid = 4326;

    terrama2::core::Schedule schedule;
    schedule.id = 1;
    schedule.frequency = 2;
    schedule.frequencyUnit = "min";

    view->schedule = schedule;

    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:39:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:45:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");

    view->filtersPerDataSeries.emplace(1, filter);
    view->filtersPerDataSeries.emplace(2, filter);
    view->filtersPerDataSeries.emplace(3, filter);
    view->filtersPerDataSeries.emplace(4, filter);

    view->dataSeriesList.push_back(1);
    view->dataSeriesList.push_back(2);
    view->dataSeriesList.push_back(3);
    view->dataSeriesList.push_back(4);

    terrama2::services::view::core::ViewStyle geomStyle;
    geomStyle.setPolygonSymbolizer(CreatePolygonSymbolizer());

//    view->stylesPerDataSeries.emplace(2, geomStyle);

    QJsonObject obj = terrama2::services::view::core::toJson(viewPtr);

    terrama2::services::view::core::ViewPtr viewBackPtr = terrama2::services::view::core::fromViewJson(obj);

    QCOMPARE(viewBackPtr->id, viewPtr->id);
    QCOMPARE(viewBackPtr->projectId, viewPtr->projectId);
    QCOMPARE(viewBackPtr->serviceInstanceId, viewPtr->serviceInstanceId);
    QCOMPARE(viewBackPtr->active, viewPtr->active);
    QCOMPARE(viewBackPtr->resolutionWidth, viewPtr->resolutionWidth);
    QCOMPARE(viewBackPtr->resolutionHeight, viewPtr->resolutionHeight);

    QCOMPARE(viewBackPtr->schedule.id, viewPtr->schedule.id);
    QCOMPARE(viewBackPtr->schedule.frequency, viewPtr->schedule.frequency);
    QCOMPARE(viewBackPtr->schedule.frequencyUnit, viewPtr->schedule.frequencyUnit);

    QCOMPARE(viewBackPtr->dataSeriesList.size(), viewPtr->dataSeriesList.size());

    for(uint32_t i = 0; i < viewPtr->dataSeriesList.size(); i++)
    {
      QCOMPARE(viewBackPtr->dataSeriesList.at(i), viewPtr->dataSeriesList.at(i));
    }

    QCOMPARE(viewBackPtr->filtersPerDataSeries.size(), viewPtr->filtersPerDataSeries.size());

    for(auto& it : viewPtr->filtersPerDataSeries)
    {
      // TODO: enable when convert filter/json is implemented
//      QCOMPARE(*viewBackPtr->filtersPerDataSeries.at(it.first).discardBefore, *it.second.discardBefore);
//      QCOMPARE(*viewBackPtr->filtersPerDataSeries.at(it.first).discardAfter, *it.second.discardAfter);
    }

  }
  catch(...)
  {
    QFAIL("Unknow exception!");
  }
}
